#include <paralloid/images.h>

using namespace std;

vector<BootableImage> BootableImage::scanImages(fs::path base_path) {
    auto images = vector<BootableImage>();
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

bool FlashableTargetDesc::storageDeviceMounted() {
    if (storage_device == "sdcard" && !fs::exists("/dev/.sdcard_mounted")) {
        return false;
    }
        
    if (storage_device == "userdata" && !fs::exists("/dev/.userdata_mounted")) {
        return false;
    }
        
    return true;
}

bool FlashableTargetDesc::storageDeviceMountError() {
    if (storage_device == "userdata" && fs::exists("/dev/.userdata_mount_error")) {
        return true;
    }
    
    // TODO: sdcard?
    
    return false;
}

BootableImage FlashableTargetDesc::toBootableImage() {
    auto base_path = fs::path(storage_device == "sdcard" ?
        EXT_SDCARD_BASE_PATH : USERDATA_BASE_PATH);
    return BootableImage(base_path, image_name);
}

std::optional<FlashableTargetDesc> FlashableTargetDesc::parse(std::string desc) {
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
