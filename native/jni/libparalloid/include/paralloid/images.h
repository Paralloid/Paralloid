#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <paralloid/utils.h>

#define INTERNAL_SYSTEM_PATH "/dev/block/mapper/system_orig"
#define EXT_SDCARD_BASE_PATH "/dev/mntSD/Paralloid"
#define USERDATA_BASE_PATH   "/dev/mntDATA/unencrypted/Paralloid"

namespace fs = std::filesystem;

// A BootableImage is a directory anywhere in the filesystem that
//   1) Has at least a file named system.img inside
//   2) May or may not have additional system partitions such as product.img
// Note that it is the __directory__ that is considered a "BootableImage",
// not individual partition images located inside that directory. This is due to
// the fact that there may be multiple partition images to support one single
// bootable system, for example, system.img + product.img
// The `boot-target` script will automatically mount all supported partition images
// in the directory, not just system.img.
class BootableImage {
private:
    fs::path base_path;
    std::string image_name;
public:
    static std::vector<BootableImage> scanImages(fs::path base_path);
    
    BootableImage(fs::path base_path, std::string image_name)
      : base_path(base_path), image_name(image_name) {};
      
    std::string imageName() {
        return image_name;
    }
    
    fs::path imagePath() {
        return base_path / fs::path(image_name);
    }
    
    std::optional<fs::path> partitionImagePath(std::string partition_name) {
        if (partition_name != "system" && partition_name != "product") {
            return std::nullopt;
        } else {
            return imagePath() / (partition_name + ".img");
        }
    }
    
    fs::path systemImagePath() {
        return *partitionImagePath("system");
    }
    
    fs::path productImagePath() {
        return *partitionImagePath("product");
    }
    
    // TODO: support more images like system_ext
};

// A flashable target describes the full path to a partition in a BootableImage
// In addition, it does not care about the actual path of the underlying
// storage device -- the storage is described by either "userdata" or "sdcard",
// instead of the full base_path as in BootableImage.
// The partition name can be omitted in some operations though, in which case
// this class is just a shorthand for the full path to a BootableImage
// for the user.
struct FlashableTargetDesc {
    // "userdata" or "sdcard"
    std::string storage_device;
    // name of the image
    std::string image_name;
    // name of the partition
    std::optional<std::string> partition_name;
    
    bool storageDeviceMounted();
    
    // This conversion loses information about "partition_name" in the return value
    BootableImage toBootableImage();
    
    // Format: {userdata,sdcard}_<image_name>_<partition_name>
    static std::optional<FlashableTargetDesc> parse(std::string desc);
};
