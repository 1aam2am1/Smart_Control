cmake_minimum_required(VERSION 3.12)

# Generate version flags
find_package(Git)
execute_process(COMMAND ${GIT_EXECUTABLE} status --porcelain
        OUTPUT_VARIABLE BUILD_STATE
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${GIT_EXECUTABLE} describe --always --abbrev=0 --dirty --tags
        OUTPUT_VARIABLE _GIT_DESCRIBE
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        OUTPUT_VARIABLE _GIT_HASH
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

string(TIMESTAMP _TIME "%Y-%m-%d %H:%M:%S")

if (BUILD_STATE)
    set(DIRTY "-dirty")
else ()
    set(DIRTY "")
endif ()

#configure_file("${SOURCE_DIR}/version.cpp.in" "${CMAKE_BINARY_DIR}/version.cpp" @ONLY)
file(WRITE "${CMAKE_BINARY_DIR}/version.cpp"
        "#include \"version.h\"\n"
        ""
        ""
        "const std::string Version::GIT_DIRTY = \"${DIRTY}\";"
        "const std::string Version::GIT_SHA = \"${_GIT_HASH}\";"
        "const std::string Version::GIT_TAG = \"${_GIT_DESCRIBE}\";"
        "const std::string Version::DATE = \"${_TIME}\";"
        ""
        "const std::string DeviceVersion::number = \"1753\";")