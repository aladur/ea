cmake_minimum_required(VERSION 3.24)

set(CMP0135)

project(ea
        VERSION 0.1.0
        DESCRIPTION "A command line tool for encoding analysis"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(Boost_USE_STATIC_LIBS ON)
else()
    set(Boost_USE_STATIC_LIBS OFF)
endif()
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
# set(Boost_DEBUG ON)

include(GNUInstallDirs)

find_package(Boost CONFIG REQUIRED COMPONENTS program_options)
find_package(ICU REQUIRED COMPONENTS uc i18n data)

message(STATUS "Boost version: ${Boost_VERSION}")
message(STATUS "Boost include: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

# Build 3rd party libraries.
# Hide all warnings for 3rdparty code
if (MSVC)
    add_compile_options(/W0)
endif()
if (UNIX)
    add_compile_options(-w)
endif()

enable_testing()
include(CTest)

include (FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.18.0/googletest-1.18.0.tar.gz
  URL_HASH SHA256=6e3191c1455468b3fc35a417fb565c1c5071aee1b7e7f85e30cf48a98d37d8b5
)

# Prevent googletest from also being installed
option(INSTALL_GTEST OFF)

# For Windows: Prevent overriding the parent project's compiler/linker settings 
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(MSVC)
    set(Boost_USE_STATIC_LIBS=ON)
    add_definitions(-DUNICODE -D_UNICODE)

    add_compile_options(
        /utf-8
        /Gm- # Disable minimal rebuild
        /WX # Treat all warnings as errors
        /W4 # display level 1, 2, and 3 warnings, and all level 4 (informational) warnings that are not off by default
        /MP # Multi-processor compilation
        /permissive- # Conform to the C++ standard
        /Zo # Generate enhanced debugging information for optimized code in non-debug builds
        /sdl # Enable additional security checks
        /GS # Enable buffer security checks
        /fp:precise # Disable optimizations that could change the precision of floating-point calculations
        /Zc:wchar_t # Treat wchar_t as a built-in type
        /Zc:forScope # Force standard C++ behavior in for loop scope
        /Zc:inline # Remove unreferenced functions or data that are COMDAT or only have internal linkage
        /Gd # Use default calling convention
        /nologo # Suppress startup banner and informational messages
        /EHsc # Exception-handling behavior: Catch C++ exceptions only, extern "C" functions never throw an exception
        /Zc:__cplusplus # Enable updated __cplusplus macro. By default, Visual Studio always returns the value "199711L" for the __cplusplus preprocessor macro.
        /w44061 # enumerator 'identifier' in switch of enum 'enumeration' is not explicitly handled by a case label.
        /w44062 # enumerator 'identifier' in switch of enum 'enumeration' is not handled.
        /w44242 # 'identifier': conversion from 'type1' to 'type2', possible loss of data
        /w44254 # 'operator': conversion from 'type1' to 'type2', possible loss of data
        /w44265 # 'class' : class has virtual functions, but destructor is not virtual.
        /w44287 # 'operator': unsigned/negative constant mismatch
        /w44289 # nonstandard extension used : 'var' : loop control variable declared in the for-loop is used outside the for-loop scope
        /w44388 # 'token' : signed/unsigned mismatch
        /w44464 # A #include directive has a path that includes a '..' parent directory specifier.
        /w44545 # expression before comma evaluates to a function which is missing an argument list
        /w44546 # function call before comma missing argument list
        /w44547 # 'operator' : operator before comma has no effect; expected operator with side-effect
        /w44548 # expression before comma has no effect; expected expression with side-effect
        /w44549 # 'operator' : operator before comma has no effect; did you intend 'operator'?
        /w44555 # expression has no effect; expected expression with side-effect
        /w44596 # 'name': illegal qualified name in member declaration
        /w44905 # wide string literal cast to 'LPSTR'
        /w44906 # string literal cast to 'LPWSTR'
        /w45029 # nonstandard extension used: alignment attributes in C++ apply to variables, data members and tag types only
        /w45038 # data member 'member1' will be initialized after data member 'member2'
        /w45041 # 'member-name': out-of-line definition for constexpr static data member is not needed and is deprecated in C++17.
        /w45042 # 'function': function declarations at block scope cannot be specified 'inline' in standard C++; remove 'inline' specifier
    )
endif()
if(UNIX)
    add_compile_options(
        -finput-charset=UTF-8
        -fPIC
        -Wall
        -Wextra
        -Wpedantic
        -Werror
        -Wshadow
        -Wswitch-enum
        -Wfloat-conversion
        -Wundef
        -Wconversion
        -Wsign-conversion
        -Wfloat-conversion
        -Wsuggest-override
    )
    set(CMAKE_SKIP_BUILD_RPATH OFF)
    set(CMAKE_SKIP_INSTALL_RPATH OFF)
endif()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(examples)

# uninstall target
# see also file uninstall.cmake.in
if(NOT TARGET uninstall)
    configure_file(
      "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
      "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
      IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
      COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake)
endif()
