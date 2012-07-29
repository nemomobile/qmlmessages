#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <TelepathyQt4/AbstractClientHandler>
#include <TelepathyQt4/PendingChannelRequest>

class ConversationChannel;

class ClientHandler : public QObject, public Tp::AbstractClientHandler
{
    Q_OBJECT
public:
    ClientHandler();
    virtual ~ClientHandler();

    static ClientHandler *instance();

    virtual bool bypassApproval() const;
    virtual void handleChannels(const Tp::MethodInvocationContextPtr<> &context, const Tp::AccountPtr &account,
                                const Tp::ConnectionPtr &connection, const QList<Tp::ChannelPtr> &channels,
                                const QList<Tp::ChannelRequestPtr> &requestsSatisfied, const QDateTime &userActionTime,
                                const HandlerInfo &handlerInfo);

public slots:
    void addChannelRequest(const Tp::ChannelRequestPtr &request, ConversationChannel *conversation);

signals:
    void incomingChat(ConversationChannel *conversation);
    void outgoingChat(ConversationChannel *conversation);

private:
    QHash<QString,ConversationChannel*> pendingRequests;
};

#endif

