#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <private/utils.h>

#define INTERNAL_SYSTEM_PATH "/dev/block/mapper/system_phh"
#define EXT_SDCARD_BASE_PATH "/dev/mntSD/Diverter"
#define USERDATA_BASE_PATH   "/dev/mntDATA/unencrypted/Diverter"

namespace fs = std::filesystem;

class BootableImage {
private:
    fs::path base_path;
    std::string image_name;
public:
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

static std::vector<BootableImage> scanImages(fs::path base_path) {
    auto images = std::vector<BootableImage>();
    // An "image" is defined as a directory under base_path that contains
    // at least system.img (there could be extra images such as product.img)
    for (auto& p: fs::directory_iterator(base_path)) {
        if (!p.is_directory()) continue;
        if (!fs::exists(p / fs::path("system.img"))) continue;
        auto relative_path = fs::relative(p, base_path);
        images.push_back(BootableImage(base_path, relative_path.string()));
    }
    return images;
}

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
    
    bool storageDeviceMounted() {
        if (storage_device == "sdcard" && !fs::exists("/dev/.sdcard_mounted")) {
            return false;
        }
        
        if (storage_device == "userdata" && !fs::exists("/dev/.userdata_mounted")) {
            return false;
        }
        
        return true;
    }
    
    // This conversion loses information about "partition_name" in the return value
    BootableImage toBootableImage() {
        auto base_path = fs::path(storage_device == "sdcard" ? EXT_SDCARD_BASE_PATH : USERDATA_BASE_PATH);
        return BootableImage(base_path, image_name);
    }
    
    // Format: {userdata,sdcard}_<image_name>_<partition_name>
    static std::optional<FlashableTargetDesc> parse(std::string desc) {
        FlashableTargetDesc ret;
        
        auto splitted = split(desc, '_');

        if (splitted.size() < 2 || splitted.size() > 3) {
            return std::nullopt;
        }

        ret.storage_device = splitted[0];
        if (ret.storage_device != "sdcard" && ret.storage_device != "userdata") {
            return std::nullopt;
        }

        ret.image_name = splitted[1];
        
        if (splitted.size() == 3) {
            ret.partition_name = splitted[2];
            if (ret.partition_name != "system" && ret.partition_name != "product") {
                return std::nullopt;
            }
        }
        
        return ret;
    }
};
