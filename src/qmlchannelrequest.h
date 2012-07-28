#ifndef QMLCHANNELREQUEST_H
#define QMLCHANNELREQUEST_H

#include <QObject>
#include <TelepathyQt4/PendingChannelRequest>
#include <TelepathyQt4/ChannelRequest>

class ChatModel;

/* Represents a channel request from end to end, with signals useful
 * to QML. Given a PendingChannelRequest (such as one from
 * Tp::Account::ensureTextChat or AccountsModel::ensureTextChat), this
 * will wait for the ChannelRequest, register it with ClientHandler to
 * get a ChatModel, and notify of the eventual success or failure of
 * the request.
 *
 * This cannot be done in pure QML due to difficulties with managing
 * Tp::SharedPtr subclasses. */
class QmlChannelRequest : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ChatModel* model READ model NOTIFY chatModelReady)

public:
    QmlChannelRequest(QObject *parent = 0);

    Q_INVOKABLE void start(Tp::PendingChannelRequest *request);

    ChatModel *model() const { return mModel; }

signals:
    void chatModelReady(ChatModel *model);
    void succeeded();
    void failed(const QString &errorName, const QString &errorMessage);
    void finished();

private slots:
    void channelRequestCreated(const Tp::ChannelRequestPtr &request);

private:
    Tp::PendingChannelRequest *mPendingRequest;
    Tp::ChannelRequestPtr mRequest;
    ChatModel *mModel;
};

#endif

