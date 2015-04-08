add_library(plasma-volume-declarative SHARED plugin.cpp)
target_link_libraries(plasma-volume-declarative
    Qt5::Quick
    QPulseAudioPrivate
)
install(TARGETS plasma-volume-declarative DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/volume)
install(FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/volume)
