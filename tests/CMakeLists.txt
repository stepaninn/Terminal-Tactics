find_package(Catch2 3 QUIET)

if(NOT Catch2_FOUND)
    message(WARNING "Catch2 3 was not found; unit tests will not be built")
    return()
endif()

function(add_terminal_tactics_test name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE core_lib project_headers matrix Catch2::Catch2WithMain)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_terminal_tactics_test(test_matrix test_matrix.cpp)
add_terminal_tactics_test(test_cells test_cells.cpp)
add_terminal_tactics_test(test_components test_components.cpp)
add_terminal_tactics_test(test_items test_items.cpp)
add_terminal_tactics_test(test_entities test_entities.cpp)
add_terminal_tactics_test(test_level test_level.cpp)
add_terminal_tactics_test(test_service test_service.cpp)
add_terminal_tactics_test(test_factory test_factory.cpp)
add_terminal_tactics_test(test_all
        test_cells.cpp
        test_components.cpp
        test_entities.cpp
        test_factory.cpp
        test_items.cpp
        test_level.cpp
        test_matrix.cpp
        test_service.cpp)
