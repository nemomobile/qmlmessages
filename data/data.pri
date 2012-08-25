client.path = $${INSTALL_ROOT}/usr/share/telepathy/clients
client.files = data/qmlmessages.client

service.path = $${INSTALL_ROOT}/usr/share/dbus-1/services
service.files = data/org.freedesktop.Telepathy.Client.qmlmessages.service

INSTALLS += client service
