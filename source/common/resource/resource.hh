// SPDX-License-Identifier: BSD-2-Clause
#pragma once

constexpr static unsigned int PURGE_INIT        = 0x0001; // expect the resource to be purged after game::init
constexpr static unsigned int PURGE_INIT_LATE   = 0x0002; // expect the resource to be purged after game::init_late
constexpr static unsigned int PURGE_UPDATE      = 0x0004; // expect the resource to be purged after game::update
constexpr static unsigned int PURGE_UPDATE_LATE = 0x0008; // expect the resource to be purged after game::update_late
constexpr static unsigned int PURGE_SESSION_END = 0x0010; // expect the resource to be purged after the session ends
constexpr static unsigned int PURGE_PRECACHE    = 0x0100; // expect the resource to persist until the game terminates
constexpr static unsigned int PURGE_IMMEDIATELY = 0xFFFF; // expect the resource to be purged as soon as possible
constexpr static unsigned int PURGE_EVERYTHING  = 0xFFFF; // used in resource::purge to cleanup everything that is there

// This interface is to be implemented to load and unload
// a specific resource of a specific type; each resource loading
// can be modified by passing an additional but optional flag argument
class IResourceLoader {
public:
    virtual ~IResourceLoader(void) = default;
    virtual const void *load(const std::string &path, unsigned int flags) const = 0;
    virtual void unload(const void *object) const = 0;
};

namespace resource::detail
{
void register_loader(const std::type_index &type, std::shared_ptr<IResourceLoader> &loader);
const void *load(const std::type_index &type, const std::string &path, unsigned int purge_mode, unsigned int flags);
const void *find(const std::type_index &type, const std::string &path);
} // namespace resource::detail

namespace resource
{
void shutdown(void);
void purge(unsigned int purge_bits);
} // namespace resource

namespace resource
{
template<typename T, typename LoaderType>
void register_loader(std::shared_ptr<LoaderType> &loader);
template<typename T>
const T *load(const std::string &path, unsigned int purge_mode, unsigned int flags = 0U);
template<typename T>
const T *find(const std::string &path);
} // namespace resource

template<typename T, typename LoaderType>
void resource::register_loader(std::shared_ptr<LoaderType> &loader)
{
    resource::detail::register_loader(std::type_index(typeid(T)), std::static_pointer_cast<IResourceLoader>(loader));
}

template<typename T>
const T *resource::load(const std::string &path, unsigned int purge_mode, unsigned int flags)
{
    return reinterpret_cast<const T *>(resource::detail::load(std::type_index(typeid(T)), path, purge_mode, flags));
}

template<typename T>
const T *resource::find(const std::string &path)
{
    return reinterpret_cast<const T *>(resource::detail::find(std::type_index(typeid(T)), path));
}
