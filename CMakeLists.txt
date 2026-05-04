cmake_minimum_required(VERSION 3.10)
project(PasswordVault)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/auth.cpp
    src/crypto.cpp
    src/vault.cpp
    src/password_gen.cpp
    src/ui.cpp
    src/mongoose.c
)

# Executable
add_executable(password_vault ${SOURCES})

# Mongoose definitions for old MinGW
target_compile_definitions(password_vault PRIVATE 
    MG_ENABLE_PACKED_FS=0
    fseeko=fseeko64
)

# Link libraries if needed
if(WIN32)
    target_link_libraries(password_vault ws2_32 crypt32 wsock32)
endif()