#ifndef CONVERSATIONCHANNEL_H
#define CONVERSATIONCHANNEL_H

#include <QObject>
#include <TelepathyQt4/PendingChannelRequest>
#include <TelepathyQt4/ChannelRequest>
#include <TelepathyQt4/Channel>

class ChatModel;

/* ConversationChannel represents a Tp::Channel, from pending requests
 * through to the lifetime of the channel itself, and provides operations
 * on that channel.
 *
 * They can be created either by starting a new channel, e.g. with
 * Tp::Account::ensureTextChat or AccountsModel::ensureTextChat, and calling
 * start(), or by ClientHandler in response to an incoming channel (which goes
 * through setChannel()).
 *
 * Currently, notification when they're created is done entirely through
 * ClientHandler, but that should probably change. It's also currently
 * responsible for creating the ChatModel, but that might be better done within
 * QML now.
 *
 * XXX The lifetime and deletion of this object is currently undefined, and it's
 * expected to leak.
 * XXX It might be nice to use PIMPL, too.
 */
class ConversationChannel : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
   
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(QString contactId READ contactId NOTIFY contactIdChanged)

    Q_PROPERTY(ChatModel* model READ model NOTIFY chatModelReady)

public:
    enum State {
        Null,
        PendingRequest,
        Requested,
        PendingReady,
        Ready,
        Error
    };

    ConversationChannel(QObject *parent = 0);

    Q_INVOKABLE void start(Tp::PendingChannelRequest *request);
    void setChannel(const Tp::ChannelPtr &channel);

    State state() const { return mState; }
    QString contactId() const;
    ChatModel *model() const { return mModel; }

public slots:
    void sendMessage(const QString &text);

signals:
    void stateChanged(int newState);
    void chatModelReady(ChatModel *model);
    void contactIdChanged();

    void requestSucceeded();
    void requestFailed(const QString &errorName, const QString &errorMessage);

    void messageSending(const QString &text);

private slots:
    void channelRequestCreated(const Tp::ChannelRequestPtr &request);
    void channelRequestSucceeded(const Tp::ChannelPtr &channel);
    void channelRequestFailed(const QString &errorName, const QString &errorMessage);
    void channelReady();

private:
    Tp::PendingChannelRequest *mPendingRequest;
    Tp::ChannelRequestPtr mRequest;
    Tp::ChannelPtr mChannel;
    State mState;
    ChatModel *mModel;

    void setState(State newState);
};

#endif

