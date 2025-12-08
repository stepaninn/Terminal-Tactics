find_package(Catch2 3 REQUIRED)

add_executable(test_matrix test_matrix.cpp)

target_link_libraries(test_matrix PRIVATE matrix Catch2::Catch2WithMain)
