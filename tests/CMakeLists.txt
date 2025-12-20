find_package(Catch2 3 REQUIRED)

add_executable(test_matrix test_matrix.cpp)

target_link_libraries(test_matrix PRIVATE matrix Catch2::Catch2WithMain)

add_executable(test_cells test_cells.cpp)

target_link_libraries(test_cells PRIVATE core_lib project_headers Catch2::Catch2WithMain)
