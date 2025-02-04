add_definitions(-DTRANSLATION_DOMAIN=\"kcm_pulseaudio\")

ecm_add_qml_module(plasma-volume-declarative URI org.kde.plasma.private.volume VERSION 0.1
    NO_GENERATE_PLUGIN_SOURCE
    NO_PLUGIN_OPTIONAL)

set(CMAKE_CXX_VISIBILITY_PRESET default)

set(plasma-volume_SRCS
    modulemanager.cpp modulemanager.h
    volumemonitor.cpp volumemonitor.h
    canberracontext.cpp canberracontext.h
    speakertest.cpp speakertest.h
    gsettingsitem.cpp gsettingsitem.h
    soundthemeconfig.cpp soundthemeconfig.h
    volumefeedback.cpp volumefeedback.h
    audioicon.cpp audioicon.h
    preferreddevice.cpp preferreddevice.h
    devicerenamemodel.cpp devicerenamemodel.h
    devicerenamesaver.cpp devicerenamesaver.h
    percentvalidator.cpp percentvalidator.h
)

ecm_qt_declare_logging_category(plasma-volume_SRCS HEADER debug.h IDENTIFIER PLASMAPA
	CATEGORY_NAME log_plasmapa DESCRIPTION "Plasma pa" EXPORT PLASMAPA)

add_library(plasma-volume
    ${plasma-volume_SRCS}
)

set_target_properties(plasma-volume PROPERTIES
                      VERSION ${PROJECT_VERSION}
                      SOVERSION ${PROJECT_VERSION_MAJOR}
                      EXPORT_NAME LibPlasmaVolume)

qt_add_dbus_interface(plasma-volume_SRCS qml/org.freedesktop.DBus.Properties.xml org.freedesktop.DBus.Properties)
target_sources(plasma-volume-declarative PRIVATE
    ${plasma-volume_SRCS}
    qml/listitemmenu.cpp qml/listitemmenu.h
    qml/plugin.cpp qml/plugin.h
    qml/microphoneindicator.cpp qml/microphoneindicator.h
    qml/globalservice.cpp qml/globalservice.h
    qml/dbusinterface.cpp
)
target_include_directories(plasma-volume-declarative PRIVATE qml/)

ecm_target_qml_sources(plasma-volume-declarative VERSION 0.1 SOURCES
    qml/PulseObjectFilterModel.qml
)

set_property(SOURCE dbus/osdService.xml APPEND PROPERTY CLASSNAME OsdServiceInterface)
qt_add_dbus_interface(dbus_SRCS dbus/osdService.xml osdservice)

ecm_generate_export_header(plasma-volume-declarative
    BASE_NAME PulseAudioQt
    GROUP_BASE_NAME KF
    VERSION ${PROJECT_VERSION}
)

target_sources(plasma-volume-declarative PRIVATE ${dbus_SRCS})
target_sources(plasma-volume PRIVATE ${dbus_SRCS})

kconfig_add_kcfg_files(plasma-volume-declarative GENERATE_MOC globalconfig.kcfgc)
kconfig_add_kcfg_files(plasma-volume GENERATE_MOC globalconfig.kcfgc)

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
    KF6::PulseAudioQt
    Canberra::Canberra
    PkgConfig::LIBPULSE
    PkgConfig::LIBPULSE_MAINLOOP
    PkgConfig::GIO
)

target_link_libraries(plasma-volume PRIVATE
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
    KF6::PulseAudioQt
    Canberra::Canberra
    PkgConfig::LIBPULSE
    PkgConfig::LIBPULSE_MAINLOOP
    PkgConfig::GIO
)

ecm_finalize_qml_module(plasma-volume-declarative)
install(TARGETS plasma-volume ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(kcm)
add_subdirectory(kded)

ecm_qt_install_logging_categories(EXPORT PLASMAPA FILE plasmapa.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

