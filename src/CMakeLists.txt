include_directories(${PULSEAUDIO_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR})

set(SRC_LIST
    client.cpp
    context.cpp
    pulseaudio.cpp
    pulseobject.cpp
    sink.cpp
    sinkinput.cpp
    source.cpp
    sourceoutput.cpp
)
qt5_wrap_ui(SRC_LIST clientview.ui)
add_executable(${PROJECT_NAME} ${SRC_LIST} clientview.cpp main.cpp)
target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    ${PULSEAUDIO_LIBRARY}
    ${PULSEAUDIO_MAINLOOP_LIBRARY}
)

add_library(plasma-volume-declarative SHARED plugin.cpp ${SRC_LIST})
target_link_libraries(plasma-volume-declarative
    Qt5::Quick
    ${PULSEAUDIO_LIBRARY}
    ${PULSEAUDIO_MAINLOOP_LIBRARY}
)
install(TARGETS plasma-volume-declarative DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/volume)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/volume)

add_subdirectory(kcm)
