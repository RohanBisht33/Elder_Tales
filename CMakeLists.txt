cmake_minimum_required(VERSION 3.16)
project(Elder_Tales VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Aggressive Warnings (The "AAA" Standard)
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -pedantic
    -Wconversion
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
)

# Sanitizers (Debug only)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined,leak -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined,leak)
endif()

# Source Files
file(GLOB_RECURSE SOURCES "src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX "src/Legacy/.*")

# Executable
add_executable(Elder_Tales ${SOURCES} src/Core/SaveManager.cpp src/Core/UserManager.cpp)

# Include Directories (SYSTEM to suppress library warnings)
target_include_directories(Elder_Tales SYSTEM PRIVATE include)

# Threads (needed for future concurrency)
find_package(Threads REQUIRED)
target_link_libraries(Elder_Tales PRIVATE Threads::Threads)

# Tests
add_executable(test_determinism tests/test_determinism.cpp src/Core/RNG.cpp)
target_include_directories(test_determinism PRIVATE include)

add_executable(test_databases tests/test_databases.cpp src/Data/ItemDatabase.cpp src/Data/EnemyDatabase.cpp src/Core/Logger.cpp)
target_include_directories(test_databases PRIVATE include)

add_executable(test_combat tests/test_combat.cpp
    src/Domain/CombatService.cpp
    src/Core/RNG.cpp
    src/Core/GVM.cpp
    src/Core/Crypto.cpp
    src/Domain/AI/QLearningAgent.cpp
    src/Data/ItemDatabase.cpp
    src/Data/EnemyDatabase.cpp
    src/Core/ServiceLocator.cpp
    src/Core/RNG.cpp
    src/Core/GVM.cpp
    src/Core/Crypto.cpp
    src/Core/Logger.cpp)
target_include_directories(test_combat PRIVATE include)

add_executable(test_gvm tests/test_gvm.cpp src/Core/GVM.cpp src/Core/RNG.cpp src/Core/Crypto.cpp)
target_include_directories(test_gvm PRIVATE include)

add_executable(test_persistence tests/test_persistence.cpp src/Core/SaveManager.cpp src/Core/Logger.cpp)
target_include_directories(test_persistence PRIVATE include)
