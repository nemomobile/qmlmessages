/* Copyright (C) 2012 John Brooks <john.brooks@dereferenced.net>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

