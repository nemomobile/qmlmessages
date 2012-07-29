#include "clienthandler.h"
#include "conversationchannel.h"

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
    /* This function assumes that no more than one conversation's channels can be satisfied in
     * a single call. To my knowledge, that is safe, but there's no easy way to assert
     * on it, and implementing it otherwise is very difficult. */
    ConversationChannel *existingConversation = 0;

    foreach (const ChannelRequestPtr &r, requestsSatisfied) {
        QHash<QString,ConversationChannel*>::Iterator it = pendingRequests.find(r->objectPath());
        if (it != pendingRequests.end()) {
            Q_ASSERT(!existingConversation);
            existingConversation = *it;
            pendingRequests.erase(it);
        }
    }

    foreach (const ChannelPtr &c, channels) {
        if (!existingConversation) {
            ConversationChannel *conversation = new ConversationChannel(this);
            conversation->setChannel(c);
            emit incomingChat(conversation);
        }
        // XXX Do we ever need to pass the channel? It would work around the succeeded() thing.
    }

    context->setFinished();
}

void ClientHandler::addChannelRequest(const ChannelRequestPtr &request, ConversationChannel *c)
{
    // XXX Crashable if the ConversationChannel is deleted...
    pendingRequests.insert(request->objectPath(), c);
    emit outgoingChat(c);
}

