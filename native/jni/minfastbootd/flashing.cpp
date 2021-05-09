#include "flashing.h"
#include "fastboot_device.h"

#include <fcntl.h>
#include <images.h>
#include <sparse/sparse.h>

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

constexpr uint32_t SPARSE_HEADER_MAGIC = 0xed26ff3a;

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

int OpenPartitionImage(BootableImage img, const std::string& partition_name, int size) {
    auto path = (img.imagePath() / (partition_name + ".img")).string();
    int fd = open(path.c_str(), O_CREAT | O_CLOEXEC | O_SYNC | O_RDWR, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP);
    
    if (fd < 0) {
        return -errno;
    }
    
    return fd;
}

// Flash* functions are from the original flashing.cpp in fastbootd
// They were originally intended for flashing block devices, but the
// same logic works for files (lseek() beyond the end simply extends
// the file)
int FlashRawDataChunk(int fd, const char* data, size_t len) {
    size_t ret = 0;
    while (ret < len) {
        int this_len = std::min(static_cast<size_t>(1048576UL * 8), len - ret);
        int this_ret = write(fd, data, this_len);
        if (this_ret < 0) {
            std::cout << "Failed to flash data of len " << this_len << " to fd " << fd << std::endl;
            return -errno;
        }
        data += this_ret;
        ret += this_ret;
    }
    return 0;
}

int FlashRawData(int fd, const std::vector<char>& downloaded_data) {
    int ret = FlashRawDataChunk(fd, downloaded_data.data(), downloaded_data.size());
    if (ret < 0) {
        return -errno;
    }
    return ret;
}

int WriteCallback(void* priv, const void* data, size_t len) {
    int fd = reinterpret_cast<long long>(priv);
    if (!data) {
        return lseek64(fd, len, SEEK_CUR) >= 0 ? 0 : -errno;
    }
    return FlashRawDataChunk(fd, reinterpret_cast<const char*>(data), len);
}

int FlashSparseData(int fd, std::vector<char>& downloaded_data) {
    struct sparse_file* file = sparse_file_import_buf(downloaded_data.data(), true, false);
    if (!file) {
        return -ENOENT;
    }
    return sparse_file_callback(file, false, false, WriteCallback, reinterpret_cast<void*>(fd));
}

int FlashFd(int fd, std::vector<char>& downloaded_data) {
    lseek64(fd, 0, SEEK_SET);
    if (downloaded_data.size() >= sizeof(SPARSE_HEADER_MAGIC) &&
        *reinterpret_cast<uint32_t*>(downloaded_data.data()) == SPARSE_HEADER_MAGIC) {
        return FlashSparseData(fd, downloaded_data);
    } else {
        return FlashRawData(fd, downloaded_data);
    }
}

int Flash(FastbootDevice* device, const std::string& target) {
    // Parse the "target" string first
    auto splitted = split(target, '_');
    
    // The format of `target` should be
    // {sdcard,userdata}_<image_name>_<partition>
    if (splitted.size() != 3) {
        return -EINVAL;
    }
    
    auto target_storage_device = splitted[0];
    if (target_storage_device != "sdcard" && target_storage_device != "userdata") {
        return -ENOENT;
    }
    if (target_storage_device == "sdcard" && !fs::exists("/dev/.sdcard_mounted")) {
        return -ENOENT;
    }
    if (target_storage_device == "userdata" && !fs::exists("/dev/.userdata_mounted")) {
        return -ENOENT;
    }
    
    auto image_name = splitted[1];
    auto partition_name = splitted[2];
    if (partition_name != "system" && partition_name != "product") {
        return -ENOENT;
    }
    
    auto base_path = fs::path(target_storage_device == "sdcard" ? EXT_SDCARD_BASE_PATH : USERDATA_BASE_PATH);
    auto img = BootableImage(base_path, image_name);
    
    // Create the directory for the target image
    if (!fs::exists(img.imagePath()) && !fs::create_directories(img.imagePath())) {
        return -EINVAL;
    }
    
    // Prepare the target partition image
    std::vector<char> data = std::move(device->download_data());
    if (data.size() == 0) {
        return -EINVAL;
    }
    
    int fd = OpenPartitionImage(img, partition_name, data.size());
    if (fd < 0) {
        return fd;
    }
    
    int res = FlashFd(fd, data);
    close(fd);
    
    if (res >= 0) {
        device->WriteInfo("Written to " + (img.imagePath() / (partition_name + ".img")).string());
    }
    
    return res;
}
