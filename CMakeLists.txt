cmake_minimum_required(VERSION 3.19)

project(TFLite-examples VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Multimedia
    Concurrent
    LinguistTools
    REQUIRED
)

add_subdirectory(external/BaseUI)
add_subdirectory(external/tensorflow/tensorflow/lite)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

qt_add_executable(tflite-examples
    src/boxesmodel.cpp
    src/boxesmodel.h
    src/main.cpp
    src/tflite.cpp
    src/tflite.h
    src/utils.cpp
    src/utils.h
)

set_property(TARGET tflite-examples APPEND PROPERTY
    QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
)

set_target_properties(tflite-examples
    PROPERTIES
        CXX_STANDARD              17
        CXX_STANDARD_REQUIRED     YES
        CXX_EXTENSIONS            NO
)

target_include_directories(tflite-examples PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_compile_definitions(tflite-examples
    PRIVATE
        $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)

target_compile_options(tflite-examples
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall
            -Wextra
            -Wpedantic
        >

        $<$<CXX_COMPILER_ID:MSVC>:
            /W4
        >
)

target_link_libraries(tflite-examples
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Qml
        Qt::Quick
        Qt::QuickControls2
        Qt::Multimedia
        Qt::Concurrent
        tensorflow-lite
        baseui
)

set(qml_files
    qml/AboutPage.qml
    qml/MainPage.qml
    qml/CameraPage.qml
    qml/CustomDialog.qml
    qml/ImageClassificationCameraPage.qml
    qml/ImageClassificationPage.qml
    qml/ImageClassificationVideoPage.qml
    qml/ImageClassificationView.qml
    qml/main.qml
    qml/ObjectDetectionCameraPage.qml
    qml/ObjectDetectionPage.qml
    qml/ObjectDetectionVideoPage.qml
    qml/ObjectDetectionView.qml
    qml/PoseEstimationCameraPage.qml
    qml/PoseEstimationPage.qml
    qml/PoseEstimationVideoPage.qml
    qml/PoseEstimationView.qml
    qml/SettingsPage.qml
    qml/VideoPage.qml
)

qt_add_translations(tflite-examples
    TS_FILES
        translations/tflite-examples_it.ts
    SOURCES
        ${qml_files}
    LUPDATE_OPTIONS
        -no-obsolete
)

qt_add_qml_module(tflite-examples
    URI TFLite
    VERSION 1.0
    QML_FILES
        ${qml_files}
    SOURCES
        src/imageclassification.cpp
        src/imageclassification.h
        src/objectdetection.cpp
        src/objectdetection.h
        src/poseestimation.cpp
        src/poseestimation.h
        src/settings.cpp
        src/settings.h
        src/system.cpp
        src/system.h
        src/videofilter.cpp
        src/videofilter.h
    NO_RESOURCE_TARGET_PATH
)

set(QML_IMPORT_PATH "${CMAKE_SOURCE_DIR}/external/BaseUI/qml"
    CACHE STRING "Qt Creator extra qml import paths" FORCE
)
