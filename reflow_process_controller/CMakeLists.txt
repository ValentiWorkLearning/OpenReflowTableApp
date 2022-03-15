add_library(reflow_process_controller STATIC reflow_controller.cpp)

target_include_directories(reflow_process_controller PUBLIC ${CMAKE_CURRENT_LIST_DIR}/inc)
target_sources(reflow_process_controller PRIVATE ${CMAKE_CURRENT_LIST_DIR}/inc/reflow_process_controller/reflow_controller.hpp)
target_link_libraries(reflow_process_controller PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick reflow_rest_client)
