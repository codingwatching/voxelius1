set(LIBRARIES_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")
set(LIBRARIES_LICENSE_TXT "${CMAKE_CURRENT_LIST_DIR}/LEGAL_NOTICES.txt")

add_subdirectory(win32)
add_subdirectory(win64)

add_subdirectory(source/enet)
add_subdirectory(source/fastnoiselite)
add_subdirectory(source/glad)
add_subdirectory(source/imgui)
add_subdirectory(source/miniz)
add_subdirectory(source/parson)
add_subdirectory(source/physfs)
add_subdirectory(source/spdlog)
add_subdirectory(source/stb)

set(LIBRARIES_BINARY_DIR "${LIBRARIES_BINARY_DIR}" PARENT_SCOPE)
set(LIBRARIES_INCLUDE_DIR "${LIBRARIES_INCLUDE_DIR}" PARENT_SCOPE)
set(LIBRARIES_LICENSE_TXT "${LIBRARIES_LICENSE_TXT}" PARENT_SCOPE)

install(FILES "${LIBRARIES_LICENSE_TXT}" DESTINATION ".")
