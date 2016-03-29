project(plasma-volume-control)
cmake_minimum_required(VERSION 2.8.12)

set(PROJECT_VERSION "5.6.1")
set(PROJECT_VERSION_MAJOR 5)

find_package(ECM 0.0.14 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_pulseaudio\")

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    DBus
    Widgets
    Quick
)
find_package(KF5 REQUIRED COMPONENTS
    CoreAddons
    Declarative
    DocTools
    GlobalAccel
    I18n
    Plasma
)
find_package(PulseAudio REQUIRED)
find_package(GLIB2 REQUIRED)

add_subdirectory(applet)
add_subdirectory(src)
add_subdirectory(data)
# add_subdirectory(doc) # pending https://git.reviewboard.kde.org/r/124692/

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
