// SPDX-License-Identifier: BSD-2-Clause
#include "common/precompiled.hh"
#include "common/resource/image.hh"

#include "common/fstools.hh"

const void *ImageLoader::load(const std::string &path, unsigned int flags) const
{
    std::vector<std::uint8_t> buffer = {};

    if(!fstools::read_bytes(path, buffer)) {
        spdlog::warn("image: {}: {}", path, fstools::error());
        return nullptr;
    }

    stbi_set_flip_vertically_on_load((flags & IMAGE_LOAD_VFLIP) != 0U);

    int image_width, image_height;
    int buffer_size = static_cast<int>(buffer.size());
    auto buffer_data = reinterpret_cast<const stbi_uc *>(buffer.data());
    void *pixels = nullptr;

    if((flags & IMAGE_LOAD_GRAYSCALE) != 0U)
        pixels = stbi_load_from_memory(buffer_data, buffer_size, &image_width, &image_height, nullptr, STBI_grey);
    else pixels = stbi_load_from_memory(buffer_data, buffer_size, &image_width, &image_height, nullptr, STBI_rgb_alpha);

    if(!pixels) {
        spdlog::warn("image: {}: stbi_load: parse failed", path);
        return nullptr;
    }

    if(!image_width || !image_height) {
        spdlog::warn("image: {}: stbi_load: non-valid image dimensions", path);
        stbi_image_free(pixels);
        return nullptr;
    }

    auto image = new Image;
    image->width = image_width;
    image->height = image_height;
    image->pixels = pixels;

    return image;
}

void ImageLoader::unload(const void *object) const
{
    delete reinterpret_cast<const Image *>(object);
}
