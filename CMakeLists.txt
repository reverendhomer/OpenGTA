cmake_minimum_required(VERSION 3.14)
project(OpenGTA)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL REQUIRED sdl)
pkg_check_modules(SDL_MIXER SDL_mixer)
pkg_check_modules(GLU REQUIRED glu)
pkg_check_modules(PHYSFS REQUIRED physfs)
pkg_check_modules(LUA lua)

include(compile-options.cmake)
include(cmake/force-compiler-colors.cmake)

configure_file(config.h.in config.h)

include_directories(
    coldet
    math
    opensteer/include
    util
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SDL_INCLUDE_DIRS}
    ${GLU_INCLUDE_DIRS}
    ${PHYSFS_INCLUDE_DIRS})

add_definitions(${SDL_CFLAGS} ${SDL_CFLAGS_OTHER})

add_library(opensteer-clock STATIC opensteer/src/Clock.cpp)

add_subdirectory(math)
add_subdirectory(util)

add_library(coldet-math3d STATIC coldet/math3d.cpp)
target_compile_definitions(coldet-math3d PRIVATE GCC)

add_library(ogta STATIC
    ai.cpp
    blockanim.cpp
    blockdata.cpp
    datahelper.cpp
    dataholder.cpp
    entity_controller.cpp
    font_cache.cpp
    game_objects.cpp
    id_sys.cpp
    localplayer.cpp
    navdata.cpp
    read_cmp.cpp
    read_fnt.cpp
    read_fxt.cpp
    read_g24.cpp
    read_gry.cpp
    read_ini.cpp
    read_sdt.cpp
    slope_height_func.cpp
    spritemanager.cpp
    train_system.cpp)
target_link_libraries(ogta ogta-math coldet-math3d)


add_library(ogta-gl STATIC
    gl_base.cpp
    gl_camera.cpp
    gl_cityview.cpp
    gl_font.cpp
    gl_frustum.cpp
    gl_screen.cpp
    gl_spritecache.cpp
    gl_texturecache.cpp
    util/physfsrwops.c)
target_link_libraries(ogta-gl ogta)

add_executable(viewer
    main2.cpp
    viewer.cpp)

target_link_libraries(viewer
    ogta
    ogta-gl
    ogta-util
    opensteer-clock
    ${SDL_LIBRARIES}
    ${GLU_LIBRARIES}
    ${PHYSFS_LIBRARIES})

add_executable(gfxextract EXCLUDE_FROM_ALL
    gfx_extract.cpp
    read_gry.cpp
    read_g24.cpp
    read_cmp.cpp
    navdata.cpp
    dataholder.cpp)

# TODO: refactor sources so this is not needed
target_link_options(gfxextract PRIVATE
    -Xlinker --unresolved-symbols
    -Xlinker ignore-all)

target_link_libraries(gfxextract
    ogta-util
    ${SDL_LIBRARIES}
    ${GLU_LIBRARIES}
    ${PHYSFS_LIBRARIES})

if(LUA_FOUND)
    add_subdirectory(lua_addon)
    include_directories(lua_addon)
    add_executable(luaviewer EXCLUDE_FROM_ALL
        main2.cpp
        viewer.cpp)
    target_compile_definitions(luaviewer PRIVATE WITH_LUA)

    add_executable(lua_map_test EXCLUDE_FROM_ALL
        main2.cpp
        tests/lua_map_test.cpp)

    target_link_libraries(lua_map_test
        ogta
        ogta-gl
        ogta-util
        ogta-lua
        ${PHYSFS_LIBRARIES}
        ${SDL_LIBRARIES})
else()
    message(WARNING "Lua not found! Will build without lua support")
endif()

add_executable(spriteplayer EXCLUDE_FROM_ALL
    main2.cpp
    sprite_anim_player.cpp)

target_link_libraries(spriteplayer
    ogta
    ogta-gl
    ogta-util
    opensteer-clock
    ${SDL_LIBRARIES}
    ${GLU_LIBRARIES}
    ${PHYSFS_LIBRARIES})

add_executable(slopeview EXCLUDE_FROM_ALL
    main.cpp
    tools/display_slopes.cpp
    navdata.cpp
    read_cmp.cpp
    common_sdl_gl.cpp)

target_link_libraries(slopeview
    ogta
    ogta-util
    ${SDL_LIBRARIES}
    ${PHYSFS_LIBRARIES}
    ${GLU_LIBRARIES})

# broken due to missing slope2_data.h
add_executable(blockview EXCLUDE_FROM_ALL
    main2.cpp
    tools/blockview.cpp)

target_link_libraries(blockview
    ${SDL_LIBRARIES}
    ${PHYSFS_LIBRARIES}
    ${GLU_LIBRARIES})

add_executable(g24 EXCLUDE_FROM_ALL
    read_g24.cpp
    read_gry.cpp)

target_compile_definitions(g24 PRIVATE G24_DUMPER)

target_link_libraries(g24
    ogta-util
    ${SDL_LIBRARIES}
    ${PHYSFS_LIBRARIES})

add_executable(objdump EXCLUDE_FROM_ALL
    main2.cpp
    read_gry.cpp
    tools/obj_dump.cpp)

target_link_libraries(objdump EXCLUDE_FROM_ALL
    ogta-util
    ${PHYSFS_LIBRARIES}
    ${SDL_LIBRARIES})

add_executable(objdump_map EXCLUDE_FROM_ALL
    main2.cpp
    tools/obj_dump.cpp
    read_gry.cpp
    read_cmp.cpp
    navdata.cpp)

target_compile_definitions(objdump_map PRIVATE DUMP_OBJ_IN_MAP)

target_link_libraries(objdump_map
    ogta
    ogta-util
    ${PHYSFS_LIBRARIES}
    ${SDL_LIBRARIES})

add_executable(car_dump EXCLUDE_FROM_ALL
    main2.cpp
    tools/car_dump.cpp)

target_link_libraries(car_dump
    ogta
    ogta-util
    ${PHYSFS_LIBRARIES}
    ${SDL_LIBRARIES})

# broken due to missing tests/test_plane.cpp
# add_executable(plane_test
#     tests/test_plane.cpp)

if(SDL_MIXER_FOUND)
    add_executable(sound_test EXCLUDE_FROM_ALL
        util/sound_device.cpp
        util/sound_fx_cache.cpp
        util/sound_music_player.cpp
        util/sound_resample2.cpp
        util/sound_system.cpp
        util/physfsrwops.c
        read_sdt.cpp)

    target_compile_definitions(sound_test PRIVATE SOUND_TEST WITH_SOUND)

    target_link_libraries(sound_test
        ogta-util
        ${SDL_LIBRARIES}
        ${SDL_MIXER_LIBRARIES}
        # SDL_sound-devel is missing pkg-config file on Fedora 34
        # I would bother adding this myself, but I'm switching to SDL2 anyways
        SDL_sound
        ${PHYSFS_LIBRARIES})
endif()
