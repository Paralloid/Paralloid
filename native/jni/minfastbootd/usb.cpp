/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "usb.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/usb/ch9.h>
#include <linux/usb/functionfs.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

#define D(...)
#define MAX_PACKET_SIZE_FS 64
#define MAX_PACKET_SIZE_HS 512
#define MAX_PACKET_SIZE_SS 1024

#define USB_FFS_BULK_SIZE 16384

// Number of buffers needed to fit MAX_PAYLOAD, with an extra for ZLPs.
#define USB_FFS_NUM_BUFS ((4 * MAX_PAYLOAD / USB_FFS_BULK_SIZE) + 1)

static int getMaxPacketSize(int ffs_fd) {
    usb_endpoint_descriptor desc;
    if (ioctl(ffs_fd, FUNCTIONFS_ENDPOINT_DESC, reinterpret_cast<unsigned long>(&desc))) {
        D("[ could not get endpoint descriptor! (%d) ]", errno);
        return MAX_PACKET_SIZE_HS;
    } else {
        return desc.wMaxPacketSize;
    }
}

static int usb_ffs_write(usb_handle* h, const void* data, int len) {
    D("about to write (fd=%d, len=%d)", h->bulk_in.get(), len);

    const char* buf = static_cast<const char*>(data);
    int orig_len = len;
    while (len > 0) {
        int write_len = std::min(USB_FFS_BULK_SIZE, len);
        int n = write(h->bulk_in, buf, write_len);
        if (n < 0) {
            D("ERROR: fd = %d, n = %d: %s", h->bulk_in.get(), n, strerror(errno));
            return -1;
        }
        buf += n;
        len -= n;
    }

    D("[ done fd=%d ]", h->bulk_in.get());
    return orig_len;
}

static int usb_ffs_read(usb_handle* h, void* data, int len, bool allow_partial) {
    D("about to read (fd=%d, len=%d)", h->bulk_out.get(), len);

    char* buf = static_cast<char*>(data);
    int orig_len = len;
    unsigned count = 0;
    while (len > 0) {
        int read_len = std::min(USB_FFS_BULK_SIZE, len);
        int n = read(h->bulk_out, buf, read_len);
        if (n < 0) {
            D("ERROR: fd = %d, n = %d: %s", h->bulk_out.get(), n, strerror(errno));
            return -1;
        }
        buf += n;
        len -= n;
        count += n;

        // For fastbootd command such as "getvar all", len parameter is always set 64.
        // But what we read is actually less than 64.
        // For example, length 10 for "getvar all" command.
        // If we get less data than expected, this means there should be no more data.
        if (allow_partial && n < read_len) {
            orig_len = count;
            break;
        }
    }

    D("[ done fd=%d ]", h->bulk_out.get());
    return orig_len;
}

static void usb_ffs_close(usb_handle* h) {
    std::cout << "closing functionfs transport";

    h->bulk_out.reset();
    h->bulk_in.reset();

    // Notify usb_adb_open_thread to open a new connection.
    h->lock.lock();
    h->open_new_connection = true;
    h->lock.unlock();
    h->notify.notify_one();
}

usb_handle* create_usb_handle(unsigned num_bufs, unsigned io_size) {
    usb_handle* h = new usb_handle();
    
    // Just don't bother with aio
    h->write = usb_ffs_write;
    h->read = usb_ffs_read;
    h->io_size = io_size;
    h->close = usb_ffs_close;
    return h;
}
