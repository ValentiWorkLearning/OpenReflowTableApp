add_library(reflow_rest_client STATIC reflow_client.cpp)

target_include_directories(
    reflow_rest_client
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

target_link_libraries(
    reflow_rest_client
    PUBLIC
    QCoro6::Coro
    QCoro6::Network
    nlohmann_json::nlohmann_json
    fmt::fmt
    spdlog::spdlog
    tl::expected
)

target_compile_features(reflow_rest_client PUBLIC cxx_std_20)
