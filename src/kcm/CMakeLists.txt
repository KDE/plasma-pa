add_library(kcm_pulseaudio MODULE module.cpp)

target_include_directories(kcm_pulseaudio PRIVATE ${CMAKE_BINARY_DIR})

target_link_libraries(kcm_pulseaudio
    Qt::Quick
    KF5::CoreAddons
    KF5::Declarative
    KF5::I18n
    KF5::QuickAddons
)

kcoreaddons_desktop_to_json(kcm_pulseaudio "package/metadata.desktop")

install(FILES package/metadata.desktop RENAME kcm_pulseaudio.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(TARGETS kcm_pulseaudio DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)

kpackage_install_package(package kcm_pulseaudio kcms)
