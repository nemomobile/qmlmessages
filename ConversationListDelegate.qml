import QtQuick 1.1
import com.nokia.meego 1.0

Image {
    id: contactCardPortrait

    height: photo.height + itemMargins
    anchors.left: parent.left
    anchors.right: parent.right
    anchors.leftMargin: itemMargins
    anchors.rightMargin: itemMargins

    property string stringTruncater: qsTr("...")
    property int itemMargins: 10

    function getTruncatedString(valueStr, stringLen) {
        var MAX_STR_LEN = stringLen;
            //Make sure string is no longer than MAX_STR_LEN characters
            //Use MAX_STR_LEN - stringTruncater.length to make room for ellipses
            if (valueStr.length > MAX_STR_LEN) {
                valueStr = valueStr.substring(0, MAX_STR_LEN - stringTruncater.length);
                valueStr = valueStr + stringTruncater;
            }
        return valueStr;
    }

    //: Remove favorite flag / remove contact from favorites list
    property string unfavoriteTranslated: qsTr("Unfavorite")

    //: Add favorite flag / add contact to favorites list
    property string favoriteTranslated: qsTr("Favorite", "Verb")

    //: Truncate string - used when a string is too long for the display area
    property string ellipse: qsTr("(...)")

    signal clicked
    signal pressAndHold(int mouseX, int mouseY, string uuid, string name)

    source: "image://themedimage/widgets/common/list/list"

    Image {
        id: photo
        source: model.personavatarPath
    }

    Label {
        id: nameFirst
        text: model.persondisplayLabel
        anchors {
            left: photo.right;
            verticalCenter: parent.verticalCenter;
            leftMargin: photo.height/8
        }
        smooth: true
    }

    Image {
        id: favorite
        source: model.personfavorite  ? "image://themedimage/icons/actionbar/favorite-selected" : "image://themedimage/icons/actionbar/favorite"
        anchors {right: contactCardPortrait.right; top: nameFirst.top; rightMargin: photo.height/8;}
    }

    MouseArea {
        id: mouseArea
        anchors.fill: contactCardPortrait
        onClicked: {
            contactCardPortrait.clicked();
        }
    }

    states: State {
        name: "pressed"; when: mouseArea.pressed == true
        PropertyChanges { target: contactCardPortrait; opacity: .7}
    }

}

