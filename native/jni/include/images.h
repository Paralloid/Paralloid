#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

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
