add_library(application)

target_sources(
    application
    PRIVATE
    application.cpp
    ${CMAKE_CURRENT_LIST_DIR}/inc/application/edit_popup_mode.h
)

target_include_directories(
    application
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/inc/application
)

target_link_libraries(
    application
    PUBLIC
    Qt6::Quick
    Qt6::Core
    PRIVATE
    tl::expected
    reflow_rest_client
    presets_model
)
target_compile_features(application PUBLIC cxx_std_20)
