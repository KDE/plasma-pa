import QtQuick 2.0
import QtQuick.Layouts 1.0

Rectangle {
    property ListView view

    Layout.fillWidth: true

    visible: (view.count != 0) && (view.count != (index + 1))
    color: systemPalette.mid
    height: 1

    SystemPalette {
        id: systemPalette
    }
}
