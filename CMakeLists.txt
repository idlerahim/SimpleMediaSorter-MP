cmake_minimum_required(VERSION 3.16)

project(SimpleMediaSorter-MP VERSION 1.0.0.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Concurrent)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Source files
set(SOURCES
    src/main.cpp
    src/core/DatabaseManager.cpp
    src/core/JobManager.cpp
    src/core/JobModel.cpp
    src/core/WorkerFactory.cpp
    src/core/Utils.cpp
    src/core/ExifToolWrapper.cpp
    src/core/FFmpegWrapper.cpp
    src/ui/MainWindow.cpp
    src/ui/components/SidebarWidget.cpp
    src/ui/components/JobBaseWidget.cpp
    src/ui/modules/RenamerWidget.cpp
    src/ui/modules/SorterWidget.cpp
    src/ui/modules/BackupWidget.cpp
    src/ui/modules/ImageWidget.cpp
    src/ui/modules/VideoWidget.cpp
    src/ui/JobListView.cpp
    src/ui/JobDetailsPane.cpp
    src/ui/SettingsDialog.cpp
    src/core/Worker.cpp
    src/workers/CopyWorker.cpp
    src/workers/MetadataWorker.cpp
    src/workers/FFmpegWorker.cpp
    src/workers/SorterWorker.cpp
    src/workers/ImageWorker.cpp
)

set(HEADERS
    src/core/DatabaseManager.h
    src/core/JobManager.h
    src/core/JobModel.h
    src/core/WorkerFactory.h
    src/core/Utils.h
    src/core/Worker.h
    src/core/ExifToolWrapper.h
    src/core/FFmpegWrapper.h
    src/core/FFmpegWrapper.h
    src/ui/MainWindow.h
    src/ui/components/SidebarWidget.h
    src/ui/components/JobBaseWidget.h
    src/ui/modules/RenamerWidget.h
    src/ui/JobListView.h
    src/ui/JobDetailsPane.h
    src/ui/SettingsDialog.h
    src/workers/CopyWorker.h
    src/workers/MetadataWorker.h
    src/workers/FFmpegWorker.h
)

add_executable(SimpleMediaSorter-MP
    ${SOURCES}
    ${HEADERS}
    resources.rc
)

target_link_libraries(SimpleMediaSorter-MP PRIVATE
    Qt6::Widgets
    Qt6::Sql
    Qt6::Concurrent
)

# Copy icon to build directory
file(COPY ${CMAKE_SOURCE_DIR}/SimpleMediaSorter-MP_icon.png DESTINATION ${CMAKE_BINARY_DIR})

# Create a resource file if needed later
# qt_add_resources(SMSorter "resources"
#     PREFIX "/"
#     FILES resources/icons/app_icon.png
# )
