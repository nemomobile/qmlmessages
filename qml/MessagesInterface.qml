import QtQuick 2.0
import org.nemomobile.dbus 1.0

DBusInterface {
    id: messagesInterface
    destination: "org.nemomobile.qmlcontacts"
    path: "/"
    iface: "org.nemomobile.qmlcontacts"

    function startConversation(localUid, remoteUid) {

    }

    function getContact() {

    }
}

