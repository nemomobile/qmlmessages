#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <TelepathyQt4/AbstractClientHandler>

class ChatModel;

class ClientHandler : public QObject, public Tp::AbstractClientHandler
{
    Q_OBJECT
public:
    ClientHandler();
    virtual ~ClientHandler();

    virtual bool bypassApproval() const;
    virtual void handleChannels(const Tp::MethodInvocationContextPtr<> &context, const Tp::AccountPtr &account,
                                const Tp::ConnectionPtr &connection, const QList<Tp::ChannelPtr> &channels,
                                const QList<Tp::ChannelRequestPtr> &requestsSatisfied, const QDateTime &userActionTime,
                                const HandlerInfo &handlerInfo);

signals:
    void newChatModel(ChatModel *chatModel);
};

#endif

