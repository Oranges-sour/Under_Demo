#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.6)

add_definitions(-std=c++14)

set(APP_NAME Under_Demo)

project(${APP_NAME})

set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

include(CocosBuildSet)
add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/game/game_frame/GameFrame.cpp
     Classes/game/game_map/GameMap.cpp
     Classes/game/game_map/implements/MapDecorationCreator1.cpp
     Classes/game/game_map/implements/MapGenerator1.cpp
     Classes/game/game_map/implements/MapGenerator2.cpp
     Classes/game/game_map/implements/MapHelper1.cpp
     Classes/game/game_map/implements/MapPhysics1.cpp
     Classes/game/game_map/implements/MapPreRenderer1.cpp
     Classes/game/game_object/GameObject.cpp
     Classes/game/game_object/implements/bullet/bullet_1/Bullet1.cpp
     Classes/game/game_object/implements/bullet/bullet_1/Bullet1AI.cpp
     Classes/game/game_object/implements/bullet/bullet_1/Bullet1Physics.cpp
     Classes/game/game_object/implements/mpa_decoration/decoration_1/Decoration1.cpp
     Classes/game/game_object/implements/mpa_decoration/decoration_1/Decoration1AI.cpp
     Classes/game/game_object/implements/mpa_decoration/decoration_2/Decoration2.cpp
     Classes/game/game_object/implements/mpa_decoration/decoration_2/Decoration2AI.cpp
     Classes/game/game_object/implements/particle/particle_1/Particle1.cpp
     Classes/game/game_object/implements/particle/particle_1/Particle1AI.cpp
     Classes/game/game_object/implements/particle/particle_1/Particle1Physics.cpp
     Classes/game/game_object/implements/physics_component/PhysicsComponent.cpp
     Classes/game/game_object/implements/player/player_1/Player1.cpp
     Classes/game/game_object/implements/player/player_1/Player1AI.cpp
     Classes/game/game_object/implements/player/player_1/Player1Physics.cpp
     Classes/game/game_world/GameWorld.cpp
     Classes/game/game_world/implements/WorldRenderer1.cpp
     Classes/scene/GameScene.cpp
     Classes/scene/StartScene.cpp
     Classes/utility/GameObjectInfo.cpp
     Classes/utility/math/MyMath.cpp
     Classes/utility/math/Random.cpp
     Classes/utility/PhysicsShapeCache.cpp
     Classes/utility/SpritePool.cpp
     Classes/utility/touch/Joystick.cpp
     Classes/utility/touch/TouchesPool.cpp
     Classes/web/Connection.cpp
     Classes/web/ConnectionStatue.cpp
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/game/game_frame/GameFrame.h
     Classes/game/game_map/GameMap.h
     Classes/game/game_map/implements/MapDecorationCreator1.h
     Classes/game/game_map/implements/MapGenerator1.h
     Classes/game/game_map/implements/MapGenerator2.h
     Classes/game/game_map/implements/MapHelper1.h
     Classes/game/game_map/implements/MapPhysics1.h
     Classes/game/game_map/implements/MapPreRenderer1.h
     Classes/game/game_object/GameObject.h
     Classes/game/game_object/implements/bullet/bullet_1/Bullet1.h
     Classes/game/game_object/implements/bullet/bullet_1/Bullet1AI.h
     Classes/game/game_object/implements/bullet/bullet_1/Bullet1Physics.h
     Classes/game/game_object/implements/mpa_decoration/decoration_1/Decoration1.h
     Classes/game/game_object/implements/mpa_decoration/decoration_1/Decoration1AI.h
     Classes/game/game_object/implements/mpa_decoration/decoration_2/Decoration2.h
     Classes/game/game_object/implements/mpa_decoration/decoration_2/Decoration2AI.h
     Classes/game/game_object/implements/particle/particle_1/Particle1.h
     Classes/game/game_object/implements/particle/particle_1/Particle1AI.h
     Classes/game/game_object/implements/particle/particle_1/Particle1Physics.h
     Classes/game/game_object/implements/physics_component/PhysicsComponent.h
     Classes/game/game_object/implements/player/player_1/Player1.h
     Classes/game/game_object/implements/player/player_1/Player1AI.h
     Classes/game/game_object/implements/player/player_1/Player1Physics.h
     Classes/game/game_world/GameWorld.h
     Classes/game/game_world/implements/WorldRenderer1.h
     Classes/scene/GameScene.h
     Classes/scene/StartScene.h
     Classes/utility/GameObjectInfo.h
     Classes/utility/json/addon_json.hpp
     Classes/utility/json/json.h
     Classes/utility/math/iVec2.h
     Classes/utility/math/MyMath.h
     Classes/utility/math/Random.h
     Classes/utility/PhysicsShapeCache.h
     Classes/utility/QuadTree.h
     Classes/utility/SpritePool.h
     Classes/utility/Tools.h
     Classes/utility/touch/Joystick.h
     Classes/utility/touch/TouchesPool.h
     Classes/web/Connection.h
     Classes/web/ConnectionStatue.h
     )

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

target_link_libraries(${APP_NAME} cocos2d)
target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
                              MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist"
                              )
    elseif(IOS)
        cocos_pak_xcode(${APP_NAME} INFO_PLIST "iOSBundleInfo.plist.in")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
        set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "")
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "iPhone Developer")
    endif()
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    set(APP_RES_DIR "$<TARGET_FILE_DIR:${APP_NAME}>/Resources")
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
