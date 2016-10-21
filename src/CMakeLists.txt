include_directories(${PULSEAUDIO_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR}     ${GCONF_INCLUDE_DIRS}
    ${GOBJECT_INCLUDE_DIRS})

set(SRC_LIST
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
    gconfitem.cpp
    source.cpp
    sourceoutput.cpp
    stream.cpp
    volumeobject.cpp
    debug.cpp
    server.cpp
    streamrestore.cpp
)

add_library(QPulseAudioPrivate SHARED ${SRC_LIST})
target_link_libraries(QPulseAudioPrivate
    Qt5::Core
    Qt5::Gui
    ${PULSEAUDIO_LIBRARY}
    ${PULSEAUDIO_MAINLOOP_LIBRARY}
    ${GCONF_LDFLAGS}
    ${GOBJECT_LDFLAGS}
)
install(TARGETS QPulseAudioPrivate ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(kcm)
add_subdirectory(qml)
