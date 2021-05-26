#include "flashing.h"
#include "fastboot_device.h"

#include <fcntl.h>
#include <linux/fs.h>
#include <sparse/sparse.h>

#include <cstdlib>
#include <iostream>
#include <vector>

#include <paralloid/images.h>

constexpr uint32_t SPARSE_HEADER_MAGIC = 0xed26ff3a;

int OpenPartitionImage(BootableImage img, const std::string& partition_name) {
    auto path = img.partitionImagePath(partition_name)->string();
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
    int cur_pos = lseek64(fd, 0, SEEK_CUR);
    // Truncate the file to the end of the current data chunk
    // to allow overwriting a larger image file with a smaller
    // one (otherwise the file on disk would always occupy
    // the maximum amount of space it ever did occupy)
    // We ignore any error this might return, since not all
    // underlying filesystems do support ftruncate correctly
    // in all cases (a notable example is VFAT), especially
    // the case where ftruncate expands the file.
    // Note that the image might be splitted into multiple sparse
    // chunks and flashed across multiple Flash() invocations,
    // so we cannot just truncate the file to the size of the buffer
    // passed in via the Flash() call.
    ftruncate(fd, cur_pos + len);
    
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

int FlashPartition(FastbootDevice* device, const std::string& partition) {
    // TODO: support resizing dynamic partitions and possibly other HLOS partitions
    if (partition == "paralloid") {
        auto _root_blkdev = getenv("PARALLOID_ROOT_BLKDEV");
        if (_root_blkdev == nullptr) {
            return -ENOENT;
        }
        
        std::vector<char> data = std::move(device->download_data());
        if (data.size() == 0) {
            return -EINVAL;
        }
        
        int fd = open(("/dev/" + std::string(_root_blkdev)).c_str(), O_RDWR | O_SYNC);
        if (fd < 0) {
            return -EINVAL;
        }
        
        // Remove any possible write protection on the block device
        int ro_state = 0;
        ioctl(fd, BLKROSET, &ro_state);
        int res = FlashFd(fd, data);
        close(fd);
        
        return res;
    } else {
        return -ENOENT;
    }
}

// Flash a partition image described by targetDesc
int FlashTarget(FastbootDevice* device, FlashableTargetDesc targetDesc) {
    if (!targetDesc.storageDeviceMounted()) {
        if (targetDesc.storage_device == "userdata" && targetDesc.storageDeviceMountError()) {
            device->WriteInfo("We failed to mount the userdata partition.");
            device->WriteInfo("This is most likely caused by metadata encryption being enabled on your device.");
            device->WriteInfo("In the current state, we cannot install images to the userdata partition.");
        }
        return -ENOENT;
    }
    
    auto img = targetDesc.toBootableImage();
    
    // Create the directory for the target image
    if (!fs::exists(img.imagePath()) && !fs::create_directories(img.imagePath())) {
        return -EINVAL;
    }
    
    // Prepare the target partition image
    std::vector<char> data = std::move(device->download_data());
    if (data.size() == 0) {
        return -EINVAL;
    }
    
    int fd = OpenPartitionImage(img, *targetDesc.partition_name);
    if (fd < 0) {
        return fd;
    }
    
    int res = FlashFd(fd, data);
    close(fd);
    
    if (res >= 0) {
        device->WriteInfo("Written to " + img.partitionImagePath(*targetDesc.partition_name)->string());
    }
    
    return res;
}

int Flash(FastbootDevice* device, const std::string& target) {
    auto targetDesc = FlashableTargetDesc::parse(target);
    
    if (targetDesc == std::nullopt) {
        return FlashPartition(device, target);
    } else {
        return FlashTarget(device, *targetDesc);
    }
}
