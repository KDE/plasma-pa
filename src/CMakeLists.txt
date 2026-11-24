add_definitions(-DTRANSLATION_DOMAIN=\"kcm_pulseaudio\")

set(CMAKE_CXX_VISIBILITY_PRESET default)

add_library(plasma-volume)

qt_extract_metatypes(plasma-volume)

target_sources(plasma-volume PRIVATE
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
    microphonetest.cpp microphonetest.h
    globalconfigskeleton.cpp globalconfigskeleton.h
    qml/listitemmenu.cpp qml/listitemmenu.h
    qml/microphoneindicator.cpp qml/microphoneindicator.h
    qml/globalservice.cpp qml/globalservice.h
    qml/dbusinterface.cpp qml/dbusinterface.h
    qml/types.h
)

target_include_directories(plasma-volume PUBLIC qml)

ecm_qt_declare_logging_category(plasma-volume HEADER debug.h IDENTIFIER PLASMAPA
    CATEGORY_NAME log_plasmapa DESCRIPTION "Plasma pa" EXPORT PLASMAPA)

set_target_properties(plasma-volume PROPERTIES
                      VERSION ${PROJECT_VERSION}
                      SOVERSION ${PROJECT_VERSION_MAJOR}
                      EXPORT_NAME LibPlasmaVolume)

set_property(SOURCE dbus/osdService.xml APPEND PROPERTY CLASSNAME OsdServiceInterface)
qt_add_dbus_interface(dbus_SRCS dbus/osdService.xml osdservice)
qt_add_dbus_interface(dbus_SRCS qml/org.freedesktop.DBus.Properties.xml org.freedesktop.DBus.Properties)

target_sources(plasma-volume PRIVATE ${dbus_SRCS})

kconfig_target_kcfg_file(plasma-volume
    FILE globalconfig.kcfg
    CLASS_NAME GlobalConfig
    INCLUDE_FILES globalconfigskeleton.h
    INHERITS GlobalConfigSkeleton
    GENERATE_PROPERTIES
    MUTATORS
    NOTIFIERS
    PARENT_IN_CONSTRUCTOR
    GLOBAL_ENUMS
    GENERATE_MOC
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

ecm_add_qml_module(plasma-volume URI org.kde.plasma.private.volume VERSION 0.1
    NO_GENERATE_PLUGIN_SOURCE
    NO_PLUGIN_OPTIONAL)

target_sources(plasma-volumeplugin PRIVATE qml/plugin.cpp qml/plugin.h)

target_link_libraries(plasma-volumeplugin PRIVATE KF6::PulseAudioQt PkgConfig::LIBPULSE)

ecm_target_qml_sources(plasma-volume VERSION 0.1 SOURCES
    qml/PulseObjectFilterModel.qml
)

ecm_finalize_qml_module(plasma-volume)

install(TARGETS plasma-volume ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(kcm)
add_subdirectory(kded)

ecm_qt_install_logging_categories(EXPORT PLASMAPA FILE plasmapa.categories DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

install(FILES globalconfig.kcfg DESTINATION ${KDE_INSTALL_KCFGDIR} RENAME plasmapa.kcfg)
