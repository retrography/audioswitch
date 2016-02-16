cmake_minimum_required(VERSION 3.3)
set(appName AudioSwitch)
project(${appName})

# System Libraries
find_library(coreAudio CoreAudio)
find_library(coreServices CoreServices)

set(systemLibs
        ${coreAudio}
        ${coreServices})

# Source Files
set(SOURCE_FILES
        device.c
        device.h
        interface.c
        interface.h
        main.c)

# Targets
add_executable(audioswitch ${SOURCE_FILES})
target_link_libraries(audioswitch ${systemLibs})

# Install
install(TARGETS audioswitch
        RUNTIME DESTINATION bin)