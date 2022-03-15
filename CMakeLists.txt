cmake_minimum_required(VERSION 3.22)

project(OpenOven LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package(...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.17/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                EXPECTED_HASH SHA256=3bef79da16c2e031dc429e1dac87a08b9226418b300ce004cc125a82687baeef
                TLS_VERIFY ON)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES
                    jsoncpp/1.9.5
                    fmt/8.0.1
                    nlohmann_json/3.10.4
                    ms-gsl/3.1.0
                    tl-expected/20190710
                    spdlog/1.9.2
                    zlib/1.2.11

                    GENERATORS cmake_find_package
                    IMPORTS "bin, *.dll -> ${CMAKE_CURRENT_BINARY_DIR}"
                    IMPORTS "lib, *.dylib* -> ${CMAKE_CURRENT_BINARY_DIR}"
)

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})


find_package(QT NAMES Qt6 COMPONENTS Core Quick Widgets Xml Svg REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Widgets Widgets Xml Svg REQUIRED)

find_package(jsoncpp REQUIRED)
find_package(ZLIB REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(tl-expected REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(3rdparty)

#https://stackoverflow.com/questions/55870281/how-to-use-find-package-on-a-package-added-from-top-level-add-subdirectory


option(USE_QTQUICKCOMPILED_RESOURCES OFF)

if(${USE_QTQUICKCOMPILED_RESOURCES})
    find_package(Qt6QuickCompiler)
    set(PROJECT_ADD_RESOURCES qtquick_compiler_add_resources)
else()
    set(PROJECT_ADD_RESOURCES qt_add_resources)
endif()

cmake_language(CALL ${PROJECT_ADD_RESOURCES} RESOURCES  resources/qml.qrc)


set(PROJECT_SOURCES
        main.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OpenOvenApp
        ${PROJECT_SOURCES}
        ${RESOURCES}
    )
else()
    if(ANDROID)
        add_library(OpenOvenApp SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(OpenOvenApp
          ${PROJECT_SOURCES}
          ${RESOURCES}
        )
    endif()
endif()

target_compile_definitions(OpenOvenApp
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

target_link_libraries(OpenOvenApp
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml Qt${QT_VERSION_MAJOR}::Svg
  fmt::fmt
  nlohmann_json::nlohmann_json
  Microsoft.GSL::GSL
  tl::expected
  spdlog::spdlog
  )

add_subdirectory(application)
add_subdirectory(rest_client)
add_subdirectory(presets_model)
add_subdirectory(reflow_process_controller)

target_compile_features(
    OpenOvenApp
    PRIVATE
    cxx_std_20
)

target_link_libraries(OpenOvenApp PRIVATE application)
set(TARGET_NAME OpenOvenApp)

if (WIN32)
    get_target_property(QT6_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT6_WINDEPLOYQT_EXECUTABLE ${QT6_QMAKE_EXECUTABLE} PATH)
    set(QT6_WINDEPLOYQT_EXECUTABLE "${QT6_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
       COMMAND ${QT6_WINDEPLOYQT_EXECUTABLE} --qmldir ${CMAKE_SOURCE_DIR} "$<$<CONFIG:Debug>:--debug>$<$<CONFIG:Release>:--release>" --no-compiler-runtime --no-translations $<TARGET_FILE_DIR:${TARGET_NAME}>)

   include(InstallRequiredSystemLibraries)
      foreach(lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
          get_filename_component(filename "${lib}" NAME)
          add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
              COMMAND "${CMAKE_COMMAND}" -E
                  copy_if_different "${lib}" \"$<TARGET_FILE_DIR:${TARGET_NAME}>\"
          )
      endforeach()
endif(WIN32)
