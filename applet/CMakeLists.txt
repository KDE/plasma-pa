set(yolo_SRC
    contents/ui/BaseItem.qml
    contents/ui/BaseDeviceItem.qml
    contents/ui/Header.qml
    contents/ui/main.qml
    contents/ui/SinkInputItem.qml
    contents/ui/SinkItem.qml
    contents/ui/SourceListItem.qml
)
plasma_install_package(. org.kde.plasma.volume)
add_custom_target(QmlFiles ALL echo SOURCES ${yolo_SRC})

