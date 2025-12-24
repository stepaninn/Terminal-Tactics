find_package(Catch2 3 REQUIRED)

add_executable(test_matrix test_matrix.cpp)

target_link_libraries(test_matrix PRIVATE matrix Catch2::Catch2WithMain)

add_executable(test_cells test_cells.cpp)

target_link_libraries(test_cells PRIVATE core_lib project_headers Catch2::Catch2WithMain)

add_executable(test_components test_components.cpp)

target_link_libraries(test_components PRIVATE core_lib project_headers Catch2::Catch2WithMain)

add_executable(test_items test_items.cpp)

target_link_libraries(test_items PRIVATE core_lib project_headers Catch2::Catch2WithMain)

add_executable(test_entities test_entities.cpp)

target_link_libraries(test_entities PRIVATE core_lib project_headers Catch2::Catch2WithMain)

add_executable(test_level test_level.cpp)

target_link_libraries(test_level PRIVATE core_lib project_headers Catch2::Catch2WithMain)

add_executable(test_service test_service.cpp)

target_link_libraries(test_service PRIVATE core_lib project_headers Catch2::Catch2WithMain)
