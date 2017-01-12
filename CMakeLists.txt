project(plasma-volume-control)
cmake_minimum_required(VERSION 2.8.12)

set(PROJECT_VERSION "5.9.90")
set(PROJECT_VERSION_MAJOR 5)

set (QT_MIN_VERSION "5.4.0")
set (ECM_MIN_VERSION "0.0.14")

find_package(ECM ${ECM_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_pulseaudio\")

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMOptionalAddSubdirectory)

include(FindPkgConfig)
pkg_check_modules(GCONF REQUIRED gconf-2.0)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Gui
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
find_package(PulseAudio 5.0.0 REQUIRED)
find_package(Canberra REQUIRED)
find_package(GLIB2 REQUIRED)

add_subdirectory(applet)
add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(doc)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
