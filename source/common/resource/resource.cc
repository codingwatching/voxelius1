#include "common/precompiled.hh"
#include "common/resource/resource.hh"

struct Resource final {
    unsigned int purge_mode;
    const void *data_ptr;
};

struct ResourceMap final {
    emhash8::HashMap<std::string, Resource> data;
    std::shared_ptr<IResourceLoader> loader;
};

static emhash8::HashMap<std::type_index, ResourceMap> resources = {};

void resource::detail::register_loader(const std::type_index &type, std::shared_ptr<IResourceLoader> &loader)
{
    const auto rmap = resources.find(type);

    if(rmap != resources.cend()) {
        spdlog::critical("resource: loader for [{}] is already registered!", type.name());
        std::terminate();
    }

    if(loader == nullptr) {
        spdlog::critical("resource: loader for [{}] is nullptr", type.name());
        std::terminate();
    }

    ResourceMap new_rmap;
    new_rmap.loader = loader;
    new_rmap.data.clear();

    resources.insert_or_assign(type, std::move(new_rmap));
}

const void *resource::detail::load(const std::type_index &type, const std::string &path, unsigned int purge_mode, unsigned int flags)
{
    const auto rmap = resources.find(type);

    if(rmap == resources.cend()) {
        spdlog::warn("resource: [{}]: resource type not registered", type.name());
        return nullptr;
    }

    const auto res = rmap->second.data.find(path);

    if(res != rmap->second.data.cend()) {
        spdlog::warn("resource: [{}] {}: resource already loaded", type.name(), path);
        return res->second.data_ptr;
    }

    Resource new_res = {};
    new_res.purge_mode = purge_mode;
    new_res.data_ptr = rmap->second.loader->load(path, flags);

    if(!new_res.data_ptr) {
        spdlog::warn("resource: [{}] {}: load failed", type.name(), path);
        return nullptr;
    }

    return rmap->second.data.insert_or_assign(path, std::move(new_res)).first->second.data_ptr;
}

const void *resource::detail::find(const std::type_index &type, const std::string &path)
{
    const auto rmap = resources.find(type);

    if(rmap == resources.cend()) {
        spdlog::warn("resource: [{}]: resource type not registered", type.name());
        return nullptr;
    }

    const auto res = rmap->second.data.find(path);

    if(res != rmap->second.data.cend())
        return res->second.data_ptr;
    return nullptr;
}

void resource::shutdown(void)
{
    resource::purge(PURGE_EVERYTHING);

    resources.clear();
}

void resource::purge(unsigned int purge_bits)
{
    for(auto &rmap : resources) {
        auto res = rmap.second.data.cbegin();

        while(res != rmap.second.data.cend()) {
            if((res->second.purge_mode & purge_bits) != 0U) {
                spdlog::debug("resource: purging [{}] {}", rmap.first.name(), res->first);
                rmap.second.loader->unload(res->second.data_ptr);
                res = rmap.second.data.erase(res);
                continue;
            }

            res++;
        }
    }
}
