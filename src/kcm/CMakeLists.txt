kcoreaddons_add_plugin(kcm_pulseaudio SOURCES module.cpp resources.qrc INSTALL_NAMESPACE "plasma/kcms/systemsettings")

target_link_libraries(kcm_pulseaudio
    Qt::Quick
    KF6::CoreAddons
    KF6::KCMUtilsQuick
)

kcmutils_generate_desktop_file(kcm_pulseaudio)

kpackage_install_package(package kcm_pulseaudio kcms)
