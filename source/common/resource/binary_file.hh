// SPDX-License-Identifier: BSD-2-Clause
#pragma once
#include "common/resource/resource.hh"

struct BinaryFile final {
    std::uint8_t *buffer;
    std::size_t length;
};

class BinaryFileLoader final : public IResourceLoader {
public:
    explicit BinaryFileLoader(void) = default;
    virtual ~BinaryFileLoader(void) override = default;
    virtual const void *load(const std::string &path, unsigned int flags) const override;
    virtual void unload(const void *object) const override;
};
