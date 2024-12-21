// SPDX-License-Identifier: BSD-2-Clause
#include "client/precompiled.hh"
#include "client/resource/texture2D.hh"

#include "common/resource/image.hh"

const void *Texture2DLoader::load(const std::string &path, unsigned int flags) const
{
    unsigned int image_flags;

    if((flags & TEXTURE2D_LOAD_VFLIP) != 0U)
        image_flags = IMAGE_LOAD_VFLIP;
    else image_flags = 0U;

    if(auto image = resource::load<Image>(path, PURGE_IMMEDIATELY, image_flags)) {
        auto texture = new Texture2D;

        glGenTextures(1, &texture->handle);
        glBindTexture(GL_TEXTURE_2D, texture->handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

        if((flags & TEXTURE2D_LOAD_CLAMP_S) != 0U)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

        if((flags & TEXTURE2D_LOAD_CLAMP_T) != 0U)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if((flags & TEXTURE2D_LOAD_LINEAR_MAG) != 0U)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if((flags & TEXTURE2D_LOAD_LINEAR_MIN) != 0U)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        texture->imgui = reinterpret_cast<ImTextureID>(static_cast<std::uintptr_t>(texture->handle));
        texture->height = image->height;
        texture->width = image->width;

        return texture;
    }

    return nullptr;
}

void Texture2DLoader::unload(const void *object) const
{
    delete reinterpret_cast<const Texture2D *>(object);
}
