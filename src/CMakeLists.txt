include_directories(${PULSEAUDIO_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR})

set(cpp_SRCS
    modulemanager.cpp
    qml/globalactioncollection.cpp
    qml/plugin.cpp
    qml/volumeosd.cpp
    qml/volumefeedback.cpp
)

set(qml_SRCS
    qml/qmldir
    qml/PulseObjectFilterModel.qml
)

set_property(SOURCE qml/dbus/osdService.xml APPEND PROPERTY CLASSNAME OsdServiceInterface)
qt5_add_dbus_interface(dbus_SRCS qml/dbus/osdService.xml osdservice)

if (USE_GCONF)
    include_directories(${GCONF_INCLUDE_DIRS} ${GOBJECT_INCLUDE_DIRS})
    set(cpp_SRCS ${cpp_SRCS} gconfitem.cpp)
endif()

if (USE_GSETTINGS)
    include_directories(${GIO_INCLUDE_DIRS} ${GOBJECT_INCLUDE_DIRS})
    set(cpp_SRCS ${cpp_SRCS} gsettingsitem.cpp)
endif()

add_library(plasma-volume-declarative SHARED ${dbus_SRCS} ${cpp_SRCS} ${qml_SRCS})
target_link_libraries(plasma-volume-declarative
    Qt5::Gui
    Qt5::DBus
    Qt5::Quick
    KF5::PulseAudioQt
    KF5::GlobalAccel
    Canberra::Canberra
    ${CANBERRA_LIBRARIES}
)

if (USE_GCONF)
    target_link_libraries(plasma-volume-declarative
        ${GCONF_LDFLAGS}
    )
endif()

if (USE_GSETTINGS)
    target_link_libraries(plasma-volume-declarative
        ${GIO_LIBRARIES}
        GLIB2::GLIB2
    )
endif()

set(PRIVATE_QML_INSTALL_DIR ${KDE_INSTALL_QMLDIR}/org/kde/plasma/private/volume)
install(TARGETS plasma-volume-declarative DESTINATION ${PRIVATE_QML_INSTALL_DIR})
install(FILES ${qml_SRCS} DESTINATION ${PRIVATE_QML_INSTALL_DIR})

add_subdirectory(kcm)
