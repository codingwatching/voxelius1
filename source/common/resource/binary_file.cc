// SPDX-License-Identifier: BSD-2-Clause
#include "common/precompiled.hh"
#include "common/resource/binary_file.hh"

#include "common/fstools.hh"

const void *BinaryFileLoader::load(const std::string &path, unsigned int flags) const
{
    PHYSFS_File *file = PHYSFS_openRead(path.c_str());

    if(!file) {
        spdlog::warn("binary_file: {}: {}", path, fstools::error());
        return nullptr;
    }

    BinaryFile new_file;
    new_file.length = PHYSFS_fileLength(file);
    new_file.buffer = new std::uint8_t[new_file.length];

    PHYSFS_readBytes(file, new_file.buffer, new_file.length);
    PHYSFS_close(file);

    return new BinaryFile(new_file);
}

void BinaryFileLoader::unload(const void *object) const
{
    const BinaryFile *file = reinterpret_cast<const BinaryFile *>(object);
    delete[] file->buffer;
    delete file;
}
