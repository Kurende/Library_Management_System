cmake_minimum_required(VERSION 3.5)

project(Library_Management_System VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(Qt6 COMPONENTS Widgets Sql PrintSupport REQUIRED)

# Source files
set(PROJECT_SOURCES
    main.cpp
    
    # UI
    ui/MainWindow.cpp
    ui/MainWindow.h
    ui/MainWindow.ui
    
    # Database
    database/DatabaseManager.cpp
    database/DatabaseManager.h
    
    # Authentication
    auth/AuthManager.cpp
    auth/AuthManager.h
    
    # Models
    models/User.cpp
    models/User.h
    models/Learner.cpp
    models/Learner.h
    models/Book.cpp
    models/Book.h
    models/Transaction.cpp
    models/Transaction.h
    
    # Utils
    utils/Encryption.cpp
    utils/Encryption.h
)

# Resources
set(PROJECT_RESOURCES
    resources.qrc
)

# Create executable
add_executable(Library_Management_System
    ${PROJECT_SOURCES}
    ${PROJECT_RESOURCES}
)

# Link Qt libraries
target_link_libraries(Library_Management_System PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::PrintSupport
)

# Include directories
target_include_directories(Library_Management_System PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/database
    ${CMAKE_CURRENT_SOURCE_DIR}/auth
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# Set output directory
set_target_properties(Library_Management_System PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS Library_Management_System
    RUNTIME DESTINATION bin
)

# Copy database to build directory (if exists)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/database/library_system.db")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/database/library_system.db"
         DESTINATION "${CMAKE_BINARY_DIR}/bin")
endif()

# Platform-specific settings
if(WIN32)
    set_target_properties(Library_Management_System PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Deploy Qt dependencies on Windows
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt5_DIR}/../../../bin")
        if(WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET Library_Management_System POST_BUILD
                COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:LibraryManagementSystem>
            )
        endif()
    endif()
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "Library Management System Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Qt Version: ${Qt5_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "")
