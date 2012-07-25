#include "clienthandler.h"
#include "chatmodel.h"

#include <TelepathyQt4/ChannelClassSpec>
#include <TelepathyQt4/ReceivedMessage>
#include <TelepathyQt4/TextChannel>

using namespace Tp;

ClientHandler::ClientHandler()
    : AbstractClientHandler(ChannelClassSpec::textChat())
{
}

ClientHandler::~ClientHandler()
{
}

bool ClientHandler::bypassApproval() const
{
    return true;
}

void ClientHandler::handleChannels(const MethodInvocationContextPtr<> &context, const AccountPtr &account,
                                   const ConnectionPtr &connection, const QList<ChannelPtr> &channels,
                                   const QList<ChannelRequestPtr> &requestsSatisfied, const QDateTime &userActionTime,
                                   const HandlerInfo &handlerInfo)
{
    qDebug() << "ClientHandler::handleChannels with" << channels.size() << "channels";
    foreach (const ChannelPtr &c, channels) {
        TextChannelPtr channel = SharedPtr<TextChannel>::dynamicCast<Channel>(c);
        // XXX Do we need to track channels that already have a model?
        ChatModel *model = new ChatModel(channel, this);
        emit newChatModel(model);
    }
    context->setFinished();
}

