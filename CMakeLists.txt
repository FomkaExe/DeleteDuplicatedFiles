cmake_minimum_required(VERSION 3.30)

set(PROJECT_NAME Delete-Duplicated-Files)

project(${PROJECT_NAME} VERSION 0.1.1 LANGUAGES CXX)

set(SOURCE_FILES 
    src/mainwindow.cpp
    src/main.cpp
    src/fsitem.cpp
    src/duplicatefsmodel.cpp
)
set(HEADER_FILES
    src/mainwindow.h
    src/fsitem.h
    src/duplicatefsmodel.h
    src/FileType.h
)
set(UI_FILES src/mainwindow.ui)
set(RESOURCE_FILES resources.qrc)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_resources(PROJECT_RESOURCES ${RESOURCE_FILES})

qt_add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
    ${UI_FILES}
    ${HEADER_FILES}
    ${PROJECT_RESOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
