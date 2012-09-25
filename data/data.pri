desktop.path = $${INSTALL_ROOT}/usr/share/applications
desktop.files = data/$${PROJECT_NAME}.desktop

client.path = $${INSTALL_ROOT}/usr/share/telepathy/clients
client.files = data/qmlmessages.client

service.path = $${INSTALL_ROOT}/usr/share/dbus-1/services
service.files = data/org.freedesktop.Telepathy.Client.qmlmessages.service \
                data/org.nemomobile.qmlmessages.service

context.path = $${INSTALL_ROOT}/usr/share/contextkit/providers
context.files = data/org.nemomobile.qmlmessages.context.context

INSTALLS += desktop client service context
