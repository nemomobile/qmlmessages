#include "clienthandler.h"
#include "chatmodel.h"

#include <TelepathyQt4/ChannelClassSpec>
#include <TelepathyQt4/ReceivedMessage>
#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/ChannelRequest>

using namespace Tp;

ClientHandler *clientHandler = 0;

ClientHandler::ClientHandler()
    : AbstractClientHandler(ChannelClassSpec::textChat())
{
    Q_ASSERT(!clientHandler);
    clientHandler = this;
}

ClientHandler::~ClientHandler()
{
}

ClientHandler *ClientHandler::instance()
{
    Q_ASSERT(clientHandler);
    return clientHandler;
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
    /* This function assumes that no more than one model's channels can be satisfied in
     * a single call. To my knowledge, that is safe, but there's no easy way to assert
     * on it, and implementing it otherwise is very difficult. */
    ChatModel *existingModel = 0;

    foreach (const ChannelRequestPtr &r, requestsSatisfied) {
        QHash<QString,ChatModel*>::Iterator it = requestPendingModels.find(r->objectPath());
        if (it != requestPendingModels.end()) {
            Q_ASSERT(!existingModel);
            existingModel = *it;
            requestPendingModels.erase(it);
        }
    }

    foreach (const ChannelPtr &c, channels) {
        TextChannelPtr channel = SharedPtr<TextChannel>::dynamicCast<Channel>(c);

        if (existingModel) {
            existingModel->setChannel(channel);
            existingModel = 0;
        } else {
            ChatModel *model = new ChatModel(channel, this);
            emit incomingChat(model);
        }
    }

    context->setFinished();
}

ChatModel *ClientHandler::addChannelRequest(const ChannelRequestPtr &request)
{
    ChatModel *model = new ChatModel(this);
    requestPendingModels.insert(request->objectPath(), model);
    emit outgoingChat(model);
    return model;
}

