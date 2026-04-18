cmake_minimum_required(VERSION 3.10.0)
option(ENABLE_VCPKG_BOOTSTRAP "Bootstrap vcpkg in the build dir and install manifest deps" OFF)

find_program(SYSTEM_VCPKG_EXECUTABLE NAMES vcpkg vcpkg.exe)

if(ENABLE_VCPKG_BOOTSTRAP)
    set(VCPKG_ROOT "${CMAKE_BINARY_DIR}/vcpkg")

    if(SYSTEM_VCPKG_EXECUTABLE)
        get_filename_component(SYSTEM_VCPKG_DIR "${SYSTEM_VCPKG_EXECUTABLE}" DIRECTORY)
        set(VCPKG_ROOT "${SYSTEM_VCPKG_DIR}")
        message(STATUS "Found system vcpkg: ${SYSTEM_VCPKG_EXECUTABLE}; using system vcpkg root ${VCPKG_ROOT}")

        set(_vcpkg_feature_args "")
        if(VCPKG_MANIFEST_FEATURES)
            foreach(_feature ${VCPKG_MANIFEST_FEATURES})
                list(APPEND _vcpkg_feature_args "--x-feature" "${_feature}")
            endforeach()
        endif()

        execute_process(
            COMMAND "${SYSTEM_VCPKG_EXECUTABLE}" "install" "--triplet" "${VCPKG_TRIPLET}"
                    "--x-install-root" "${CMAKE_BINARY_DIR}/vcpkg_installed" ${_vcpkg_feature_args}
            RESULT_VARIABLE _vcpkg_install_rc
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )
    else()
        message(FATAL_ERROR "vcpkg not found in PATH. Install vcpkg and ensure it's on PATH, or run configure with ENABLE_VCPKG_BOOTSTRAP=OFF to skip bootstrap.")
    endif()

    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE PATH "vcpkg toolchain file" FORCE)
endif()

project(gqlxy-core VERSION 0.1.0 LANGUAGES CXX)

include(cmake/dependencies.cmake)

add_library(gqlxy_core STATIC)
target_compile_features(gqlxy_core PUBLIC cxx_std_20)
target_include_directories(gqlxy_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(gqlxy_core
    PUBLIC
        cppgraphqlgen::graphqlpeg
        nlohmann_json::nlohmann_json
)

add_subdirectory(src)

add_library(gqlxy::core ALIAS gqlxy_core)

include(CTest)
add_subdirectory(tests)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/gqlxy-core-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/gqlxy-coreConfig.cmake
    INSTALL_DESTINATION lib/cmake/gqlxy-core
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/gqlxy-coreConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(TARGETS gqlxy_core
    EXPORT gqlxy-coreTargets
)

install(EXPORT gqlxy-coreTargets
    FILE gqlxy-coreTargets.cmake
    NAMESPACE gqlxy::
    DESTINATION lib/cmake/gqlxy-core
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/gqlxy-coreConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/gqlxy-coreConfigVersion.cmake
    DESTINATION lib/cmake/gqlxy-core
)
