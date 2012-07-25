#include <QApplication>

#include <TelepathyQt4/Constants>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/ClientRegistrar>

#include "src/accountsmodel.h"
#include "src/clienthandler.h"

using namespace Tp;

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    Tp::registerTypes();
    Tp::enableDebug(true);
    Tp::enableWarnings(true);

    //AccountsModel *m = new AccountsModel;

    ClientRegistrarPtr registrar = ClientRegistrar::create();
    AbstractClientPtr handler = AbstractClientPtr::dynamicCast(SharedPtr<ClientHandler>(new ClientHandler));
    registrar->registerClient(handler, "qmlmessages");

    return a.exec();
}
