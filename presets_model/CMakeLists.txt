add_library(presets_model STATIC presetslistmodel.cpp)
target_include_directories(presets_model PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(presets_model PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick reflow_rest_client)
