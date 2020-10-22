include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

add_library(kcm_pulseaudio MODULE module.cpp)

target_link_libraries(kcm_pulseaudio
    Qt5::Quick
    KF5::CoreAddons
    KF5::Declarative
    KF5::I18n
    KF5::QuickAddons
    ${PULSEAUDIO_LIBRARY}
    ${PULSEAUDIO_MAINLOOP_LIBRARY}
)

kcoreaddons_desktop_to_json(kcm_pulseaudio "kcm_pulseaudio.desktop")

install(FILES kcm_pulseaudio.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(TARGETS kcm_pulseaudio DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)

kpackage_install_package(package kcm_pulseaudio kcms)
