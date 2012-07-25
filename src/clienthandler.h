#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <TelepathyQt4/AbstractClientHandler>

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
};

#endif

