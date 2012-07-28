#include "qmlchannelrequest.h"
#include "clienthandler.h"

#include <TelepathyQt4/ChannelRequest>

QmlChannelRequest::QmlChannelRequest(QObject *parent)
    : QObject(parent), mPendingRequest(0), mModel(0)
{
    connect(this, SIGNAL(succeeded()), SIGNAL(finished()));
    connect(this, SIGNAL(failed(QString,QString)), SIGNAL(finished()));
}

void QmlChannelRequest::start(Tp::PendingChannelRequest *pendingRequest)
{
    Q_ASSERT(!mPendingRequest);

    mPendingRequest = pendingRequest;
    connect(mPendingRequest, SIGNAL(channelRequestCreated(Tp::ChannelRequestPtr)),
            SLOT(channelRequestCreated(Tp::ChannelRequestPtr)));
}

void QmlChannelRequest::channelRequestCreated(const Tp::ChannelRequestPtr &r)
{
    Q_ASSERT(mRequest.isNull());
    Q_ASSERT(!r.isNull());

    mRequest = r;
    connect(mRequest.data(), SIGNAL(succeeded(Tp::ChannelPtr)), SIGNAL(succeeded()));
    connect(mRequest.data(), SIGNAL(failed(QString,QString)), SIGNAL(failed(QString,QString))); 

    mModel = ClientHandler::instance()->addChannelRequest(mRequest);
    emit chatModelReady(mModel);

    mPendingRequest = 0;
}

