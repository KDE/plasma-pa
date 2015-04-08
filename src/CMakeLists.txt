include_directories(${PULSEAUDIO_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR})

set(SRC_LIST
    card.cpp
    client.cpp
    context.cpp
    pulseaudio.cpp
    pulseobject.cpp
    sink.cpp
    sinkinput.cpp
    source.cpp
    sourceoutput.cpp
)

add_library(QPulseAudioPrivate SHARED ${SRC_LIST})
target_link_libraries(QPulseAudioPrivate
    Qt5::Core
    ${PULSEAUDIO_LIBRARY}
    ${PULSEAUDIO_MAINLOOP_LIBRARY}
)
install(TARGETS QPulseAudioPrivate ${INSTALL_TARGETS_DEFAULT_ARGS})

add_subdirectory(kcm)
add_subdirectory(qml)
