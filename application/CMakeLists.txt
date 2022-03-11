add_library(application)

target_sources(
    application
    PRIVATE
    application.cpp
)

target_include_directories(
    application
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/inc/app
)

target_link_libraries(
    application
    PUBLIC
    Qt6::Quick
    Qt6::Core
    PRIVATE
    tl::expected
)
