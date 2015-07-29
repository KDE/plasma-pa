project(plasma-volume-control)
cmake_minimum_required(VERSION 2.8.12)

set(PROJECT_VERSION "5.3.90")
set(PROJECT_VERSION_MAJOR 5)

find_package(ECM 0.0.14 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    Quick
)
find_package(KF5 REQUIRED COMPONENTS
    ConfigWidgets
    CoreAddons
    Declarative
    GlobalAccel
    I18n
    Plasma
)
find_package(PulseAudio REQUIRED)
find_package(GLIB2 REQUIRED)

add_subdirectory(applet)
add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
