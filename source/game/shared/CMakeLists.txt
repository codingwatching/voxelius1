add_library(shared STATIC
    "${CMAKE_CURRENT_LIST_DIR}/entity/chunk.hh"
    "${CMAKE_CURRENT_LIST_DIR}/entity/head.hh"
    "${CMAKE_CURRENT_LIST_DIR}/entity/player.hh"
    "${CMAKE_CURRENT_LIST_DIR}/entity/transform.cc"
    "${CMAKE_CURRENT_LIST_DIR}/entity/transform.hh"
    "${CMAKE_CURRENT_LIST_DIR}/entity/velocity.cc"
    "${CMAKE_CURRENT_LIST_DIR}/entity/velocity.hh"
    "${CMAKE_CURRENT_LIST_DIR}/event/chunk_create.hh"
    "${CMAKE_CURRENT_LIST_DIR}/event/chunk_update.hh"
    "${CMAKE_CURRENT_LIST_DIR}/event/voxel_set.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/chunk.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/chunk.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/chunk_coord.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/chunk_coord.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/chunk_coord_2d.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/game_voxels.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/game_voxels.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/local_coord.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/local_coord.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/ray_dda.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/ray_dda.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/universe.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/universe.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/unloader.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/unloader.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/vdef.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/vdef.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/voxel_coord.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/voxel_coord.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/voxel_id.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/world.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/world.hh"
    "${CMAKE_CURRENT_LIST_DIR}/world/world_coord.cc"
    "${CMAKE_CURRENT_LIST_DIR}/world/world_coord.hh"
    "${CMAKE_CURRENT_LIST_DIR}/worldgen/overworld.cc"
    "${CMAKE_CURRENT_LIST_DIR}/worldgen/overworld.hh"
    "${CMAKE_CURRENT_LIST_DIR}/worldgen/worldgen.cc"
    "${CMAKE_CURRENT_LIST_DIR}/worldgen/worldgen.hh"
    "${CMAKE_CURRENT_LIST_DIR}/const.hh"
    "${CMAKE_CURRENT_LIST_DIR}/globals.cc"
    "${CMAKE_CURRENT_LIST_DIR}/globals.hh"
    "${CMAKE_CURRENT_LIST_DIR}/motd.cc"
    "${CMAKE_CURRENT_LIST_DIR}/motd.hh"
    "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh"
    "${CMAKE_CURRENT_LIST_DIR}/protocol.cc"
    "${CMAKE_CURRENT_LIST_DIR}/protocol.hh"
    "${CMAKE_CURRENT_LIST_DIR}/setup.cc"
    "${CMAKE_CURRENT_LIST_DIR}/setup.hh")
target_include_directories(shared PRIVATE "${PROJECT_SOURCE_DIR}/source")
target_include_directories(shared PRIVATE "${PROJECT_SOURCE_DIR}/source/game")
target_precompile_headers(shared PRIVATE "${CMAKE_CURRENT_LIST_DIR}/precompiled.hh")
target_link_libraries(shared PUBLIC enet FNL miniz parson)
target_link_libraries(shared PUBLIC common)
