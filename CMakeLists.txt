cmake_minimum_required(VERSION 3.28.3)

project(MyGameProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(--coverage)
add_link_options(--coverage)

add_library(project_headers INTERFACE)

target_include_directories(project_headers
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/libs/*.cpp)

add_library(core_lib STATIC ${SRC_FILES}
        libs/model/entity/components/HealthComponent.cpp
        libs/model/entity/components/TimePointsComponent.cpp
        libs/model/entity/components/WeaponComponent.cpp
        libs/model/entity/components/InventoryComponent.cpp
        libs/model/entity/entities/items/AmmoBag.cpp
        libs/model/entity/entities/items/Weapon.cpp
        libs/model/repository/cells/ItemStorage.cpp
        libs/model/service/events/EventBus.cpp
        libs/model/repository/Level.cpp
        libs/model/entity/entities/items/Medkit.cpp
        libs/model/service/World.cpp
        libs/model/service/VisionService.cpp)

target_link_libraries(core_lib PUBLIC matrix project_headers)

#add_subdirectory(libs)
add_subdirectory(matrix)
add_subdirectory(tests)
