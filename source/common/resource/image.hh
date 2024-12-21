// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "common/resource/resource.hh"

constexpr static unsigned int IMAGE_LOAD_VFLIP      = 0x0001;
constexpr static unsigned int IMAGE_LOAD_GRAYSCALE  = 0x0002;

struct Image final {
    int width {};
    int height {};
    void *pixels {};
};

class ImageLoader final : public IResourceLoader {
public:
    explicit ImageLoader(void) = default;
    virtual ~ImageLoader(void) override = default;
    virtual const void *load(const std::string &path, unsigned int flags) const override;
    virtual void unload(const void *object) const override;
};
