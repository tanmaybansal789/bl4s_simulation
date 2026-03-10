cmake_minimum_required(VERSION 3.16)
project(BL4S_Simulation)

# Generate compile_commands.json for IntelliSense/clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Require C++17 ──────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Find Geant4 (with Qt UI + visualisation drivers) ──────────────────────────
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# ── Source files ──────────────────────────────────────────────────────────────
file(GLOB SOURCES src/*.cc)
file(GLOB HEADERS include/*.hh)

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(bl4s_sim main.cc ${SOURCES})
target_include_directories(bl4s_sim PRIVATE include)
target_link_libraries(bl4s_sim ${Geant4_LIBRARIES})

# ── Copy macros into the build directory so they are always available ─────────
file(GLOB MACROS macros/*.mac)
foreach(MAC ${MACROS})
    configure_file(${MAC} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()
