//OsdIpEditor.qml
import QtQuick 2.0

Rectangle {
    id: ipEdit
    width: rectWidth
    height: rectHeight
    color: "transparent"

    property int rectWidth: 360
    property int rectHeight: 34
    property int boxWidth: 42
    property int highlIdx: 0 //indice della prima casella a sx (per evidenziazione)
    property string ipEditTxt: ""
    property bool ipEnabled: true

    property int ip0
    property int ip1
    property int ip2
    property int ip3

    signal ip0Up
    signal ip0Dwn
    signal ip1Up
    signal ip1Dwn
    signal ip2Up
    signal ip2Dwn
    signal ip3Up
    signal ip3Dwn

    Text {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left

        color: ipEnabled ? color_HeavyDark : "gray"
        font { family: "Myriad Pro"; weight: Font.Normal; pixelSize: 14 }
        text: ipEditTxt
    }

    Row {
        id: ipRow
        x: 105
        anchors.verticalCenter: parent.verticalCenter
        width: 260; height: 32
        spacing: 3

        OsdSpinBox {
            id: box0
            boxEnabled: ipEnabled
            digitBoxWidth: boxWidth
            highlighted: highlIdx === 0
            currVal: ip0
            onSpinUp: ip0Up()
            onSpinDown: ip0Dwn()
        }

        OsdSpinBox {
            id: box1
            boxEnabled: ipEnabled
            digitBoxWidth: boxWidth
            highlighted: highlIdx === 1
            currVal: ip1
            onSpinUp: ip1Up()
            onSpinDown: ip1Dwn()
        }

        OsdSpinBox {
            id: box2
            boxEnabled: ipEnabled
            digitBoxWidth: boxWidth
            highlighted: highlIdx === 2
            currVal: ip2
            onSpinUp: ip2Up()
            onSpinDown: ip2Dwn()
        }

        OsdSpinBox {
            id: box3
            boxEnabled: ipEnabled
            digitBoxWidth: boxWidth
            highlighted: highlIdx === 3
            currVal: ip3
            onSpinUp: ip3Up()
            onSpinDown: ip3Dwn()
        }
    }
}






























