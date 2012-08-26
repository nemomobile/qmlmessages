desktop.path = $${INSTALL_ROOT}/usr/share/applications
desktop.files = data/$${PROJECT_NAME}.desktop

client.path = $${INSTALL_ROOT}/usr/share/telepathy/clients
client.files = data/qmlmessages.client

service.path = $${INSTALL_ROOT}/usr/share/dbus-1/services
service.files = data/org.freedesktop.Telepathy.Client.qmlmessages.service \
                data/org.nemomobile.qmlmessages.service

INSTALLS += desktop client service
