configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    @ONLY
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(libea_SOURCES
    CharacterStatistics.cpp
    EncodedScanner.cpp
    EncodingApplication.cpp
    HelperFunctions.cpp
    RichLinePrinter.cpp
    TypeDefinitions.cpp
)

set(libea_HEADER
    CharacterStatistics.h
    Codepoint.h
    Definitions.h
    EncodedScanner.h
    EncodingApplication.h
    HelperFunctions.h
    RichLinePrinter.h
    TypeDefinitions.h
    version.h
)

add_library(libea STATIC ${libea_SOURCES} ${libea_HEADER})
add_library(ea::lib ALIAS libea)

target_link_libraries(libea
    PRIVATE
        Boost::program_options
        Boost::headers
        ICU::uc
        ICU::i18n
        ICU::data
)

set(ea_HEADER
    EncodingApplication.h
)

add_executable(ea main.cpp ${ea_HEADER})
add_executable(ea::ea ALIAS ea)

if(WIN32)
    target_compile_definitions(ea PRIVATE _WINDOWS)
endif()

if(MSVC)
    target_sources(ea PRIVATE "${CMAKE_SOURCE_DIR}/cmake/utf8.manifest")
endif()

target_include_directories(ea
    PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
)
target_link_libraries(ea
    PRIVATE
        ea::lib
        Boost::program_options
        ICU::uc
        ICU::i18n
        ICU::data
)

install(TARGETS ea)
