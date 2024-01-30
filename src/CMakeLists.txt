add_definitions(-DTRANSLATION_DOMAIN=\"kcm_pulseaudio\")

ecm_add_qml_module(plasma-volume-declarative URI org.kde.plasma.private.volume)

target_sources(plasma-volume-declarative PRIVATE
    card.cpp
    client.cpp
    context.cpp
    device.cpp
    maps.cpp
    operation.cpp
    port.cpp
    profile.cpp
    pulseaudio.cpp
    pulseobject.cpp
    sink.cpp
    sinkinput.cpp
    modulemanager.cpp
    source.cpp
    sourceoutput.cpp
    stream.cpp
    volumemonitor.cpp
    volumeobject.cpp
    debug.cpp
    server.cpp
    streamrestore.cpp
    module.cpp
    canberracontext.cpp
    speakertest.cpp
    gsettingsitem.cpp
    soundthemeconfig.cpp
    qml/globalactioncollection.cpp
    qml/listitemmenu.cpp
    qml/plugin.cpp
    qml/microphoneindicator.cpp
    qml/volumeosd.cpp
    qml/volumefeedback.cpp
)

ecm_target_qml_sources(plasma-volume-declarative VERSION 0.1 SOURCES
    qml/PulseObjectFilterModel.qml
)

ecm_generate_export_header(plasma-volume-declarative
    BASE_NAME PulseAudioQt
    GROUP_BASE_NAME KF
    VERSION ${PROJECT_VERSION}
)

set_property(SOURCE qml/dbus/osdService.xml APPEND PROPERTY CLASSNAME OsdServiceInterface)
qt_add_dbus_interface(dbus_SRCS qml/dbus/osdService.xml osdservice)

target_sources(plasma-volume-declarative PRIVATE ${dbus_SRCS})

kconfig_add_kcfg_files(plasma-volume-declarative GENERATE_MOC qml/globalconfig.kcfgc)

target_link_libraries(plasma-volume-declarative PRIVATE
    Qt::Core
    Qt::Gui
    Qt::DBus
    Qt::Quick
    Qt::Widgets
    KF6::GlobalAccel
    KF6::I18n
    KF6::StatusNotifierItem
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    Canberra::Canberra
    PkgConfig::LIBPULSE
    PkgConfig::LIBPULSE_MAINLOOP
    PkgConfig::GIO
)

ecm_finalize_qml_module(plasma-volume-declarative)

add_subdirectory(kcm)
