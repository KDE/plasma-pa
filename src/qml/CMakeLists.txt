set(qml_SRCS
    qmldir
    PulseObjectFilterModel.qml
)

set(cpp_SRCS
    globalactioncollection.cpp
    plugin.cpp
    volumeosd.cpp
    volumefeedback.cpp
)

set_property(SOURCE dbus/osdService.xml APPEND PROPERTY CLASSNAME OsdServiceInterface)
qt5_add_dbus_interface(dbus_SRCS dbus/osdService.xml osdservice)

add_library(plasma-volume-declarative SHARED ${dbus_SRCS} ${cpp_SRCS} ${qml_SRCS})
target_link_libraries(plasma-volume-declarative
    Qt5::DBus
    Qt5::Quick
    KF5::GlobalAccel
    QPulseAudioPrivate
    ${CANBERRA_LIBRARIES}
)

set(PRIVATE_QML_INSTALL_DIR ${QML_INSTALL_DIR}/org/kde/plasma/private/volume)
install(TARGETS plasma-volume-declarative DESTINATION ${PRIVATE_QML_INSTALL_DIR})
install(FILES ${qml_SRCS} DESTINATION ${PRIVATE_QML_INSTALL_DIR})
