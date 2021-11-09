add_definitions(-DTRANSLATION_DOMAIN=\"kcm_pulseaudio\")

set(cpp_SRCS
    modulemanager.cpp
    volumemonitor.cpp
    debug.cpp
    canberracontext.cpp
    speakertest.cpp
    qml/globalactioncollection.cpp
    qml/listitemmenu.cpp
    qml/plugin.cpp
    qml/microphoneindicator.cpp
    qml/volumeosd.cpp
    qml/volumefeedback.cpp
)

set(qml_SRCS
    qml/qmldir
    qml/PulseObjectFilterModel.qml
)

set_property(SOURCE qml/dbus/osdService.xml APPEND PROPERTY CLASSNAME OsdServiceInterface)
qt_add_dbus_interface(dbus_SRCS qml/dbus/osdService.xml osdservice)

if (USE_GCONF)
    set(cpp_SRCS ${cpp_SRCS} gconfitem.cpp)
endif()

if (USE_GSETTINGS)
    set(cpp_SRCS ${cpp_SRCS} gsettingsitem.cpp)
endif()

add_library(plasma-volume-declarative SHARED ${dbus_SRCS} ${cpp_SRCS} ${qml_SRCS})
target_link_libraries(plasma-volume-declarative
    Qt::Core
    Qt::Gui
    Qt::DBus
    Qt::Quick
    KF5::PulseAudioQt
    KF5::GlobalAccel
    KF5::I18n
    KF5::Notifications
    Canberra::Canberra
    PkgConfig::LIBPULSE
    PkgConfig::LIBPULSE_MAINLOOP
)

if (USE_GCONF)
    target_link_libraries(plasma-volume-declarative PkgConfig::GCONF PkgConfig::GOBJECT)
endif()

if (USE_GSETTINGS)
    target_link_libraries(plasma-volume-declarative PkgConfig::GIO)
endif()

set(PRIVATE_QML_INSTALL_DIR ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/volume)
install(TARGETS plasma-volume-declarative DESTINATION ${PRIVATE_QML_INSTALL_DIR})
install(FILES ${qml_SRCS} DESTINATION ${PRIVATE_QML_INSTALL_DIR})

add_subdirectory(kcm)
