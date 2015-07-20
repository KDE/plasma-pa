set(QML_SRCS
    qmldir
    ClientIcon.qml
)

add_library(plasma-volume-declarative SHARED plugin.cpp globalactioncollection.cpp ${QML_SRCS})
target_link_libraries(plasma-volume-declarative
    Qt5::Quick
    KF5::GlobalAccel
    QPulseAudioPrivate
)

set(PRIVATE_QML_INSTALL_DIR ${QML_INSTALL_DIR}/org/kde/plasma/private/volume)
install(TARGETS plasma-volume-declarative DESTINATION ${PRIVATE_QML_INSTALL_DIR})
install(FILES ${QML_SRCS} DESTINATION ${PRIVATE_QML_INSTALL_DIR})
