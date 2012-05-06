#include "pnsimthread.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QtNetwork>
#include <QtXml>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDataStream>
#include <QTextStream>
#include <QMutex>
#include <QRegExp>
#include "runsimthread.h"

QMutex sockmutex;

PNSimThread::PNSimThread(int socketDescriptor, QMutex *iomutex, QObject *parent) :
    QThread(parent),iomutex(iomutex),socketDescriptor(socketDescriptor)
{
    isLogged = false;
    usersFile = "./users.dat";
    logFile = "./log.dat";
    simDirectory = "./sims";
    maxid = 0;

}

void PNSimThread::readIncoming(){
    sockmutex.lock();
    qDebug() << "limit:" << commSock->readBufferSize();
    qDebug() << "[info] message from user";

    QDataStream in(commSock);
    in.setVersion(QDataStream::Qt_4_0);

    if(block == 0) {
        if(commSock->bytesAvailable() < (int)sizeof(qint64))
            return;

        in >> block;
    }

    qDebug() << "[info] size:" << block;
    qDebug() << "[info] available: " << commSock->bytesAvailable();
    if(commSock->bytesAvailable() < block) return;

    QString command;
    in >> command;
    QString message = "";
    block = 0;
    if (!handleCommand(command,message)) {
        commSock->write(createMessage(message));
        commSock->disconnectFromHost();
        sockmutex.unlock();
        return;
    }
    if (message != "") {
        commSock->write(createMessage(message));
    }

    foreach (QString id, idsToSend) {
        QString message = "<simul id=\""+id+"\">";
        message += simulations[(*outid).toInt()]->getState();
        message += "</simul>";
        qDebug() << "[info] sending simulation";
        commSock->write(createMessage(message));
    }
    idsToSend.clear();

    sockmutex.unlock();
}

void PNSimThread::handleDisconnection(){
    this->exit();
}

void PNSimThread::run()
{

    commSock = new QTcpSocket;
    simmutex = new QMutex;
    outid = new QString;

    connect(commSock, SIGNAL(readyRead()), this, SLOT(readIncoming()),Qt::DirectConnection);
    connect(commSock, SIGNAL(disconnected()), this, SLOT(handleDisconnection()), Qt::DirectConnection);

    if (!commSock->setSocketDescriptor(socketDescriptor)) {
        emit error(commSock->error());
        return;
    }
    block = 0;
    commSock->waitForConnected(-1);

    this->exec();

    delete commSock;
    delete simmutex;
    delete outid;
    qDebug() << "[info] user disconnected";
}

bool PNSimThread::handleCommand(QString command, QString &message)
{
    QString strcmd;
    StrToStrMap args;

    if (!getCommand(command,strcmd,args)) {
        message = "<err info=\"Bad command\"/>";
        return false;
    }

    if (strcmd == "register") {
        if (!args.contains("name") || !args.contains("password") || args["name"] == "" || args["password"] == "") {
            qCritical() << "[err] cannot login";
            message = "<err info=\"Password and name cannot be blank\"/>";
            return false;
        }
        int result = registerUser(args["name"],args["password"]);
        if (result != 0) {
            if (result == 1) {
                qCritical() << "[err] already registered";
                message = "<err info=\"User is already registered\"/>";
                return false;
            } else if (result == 2) {
                qCritical() << "[err] bad userfile";
                message = "<err info=\"Cannot read usersfile, probably server's problem\"/>";
                return false;
            } else {
                qCritical() << "[err] bad password";
                message = "<err info=\"Bad login or password\"/>";
                return false;
            }
        } else {
            isLogged = true;
            message = "<ok/>";
            qDebug() << "[info] user logged in";
        }
    } else if (strcmd == "login") {
        if (!args.contains("name") || !args.contains("password") || args["name"] == "" || args["password"] == "") {
            qCritical() << "[err] cannot login";
            message = "<err info=\"Password and name cannot be blank\"/>";
            return false;
        }
        int result = logUser(args["name"], args["password"]);
        if (result != 0) {
            if (result == 1) {
                qCritical() << "[err] user doesn't exist";
                message = "<err info=\"User doesn't exist\"/>";
                return false;
            } else if (result == 2) {
                qCritical() << "[err] bad password";
                message = "<err info=\"Bad password\"/>";
                return false;
            } else {
                qCritical() << "[err] file with users doesn't exist";
                message = "<err info=\"User file doesn't exist\"/>";
                return false;
            }
        } else {
            isLogged = true;
            message = "<ok/>";
            qDebug() << "[info] user logged in";
        }
    } else if (!isLogged) {
        message = "<err info=\"Not logged on\"/>";
        qCritical() << "[err] user is not logged in!";
        return false;
    } else {
        if (strcmd == "list-simuls") {
            qCritical() << "[info] list simulations";
            message = getSimulations();
            return true;
        } else if (strcmd == "simul-that") {
            qDebug() << "[info] loading simulation";
            QString net = loadSim(args["name"],args["version"]);
            if (net == "false") {
                message = "<err info=\"Cannot load petrinet\"/>";
                return true;
            }
            message = "<simul id=\""+QString::number(maxid-1)+"\">";
            message += net;
            message += "</simul>";
            qDebug() << "[info] sending new simulation";
            return true;
        } else if (strcmd == "save-this") {
            command.remove(QRegExp("^<save-this>"));
            command.remove(QRegExp("</save-this>$"));
            if (!saveSimulation(command)) {
                message = "<err info=\"Cannot save simulation\"/>";
                return true;
            }
            PetriSim *simulation = new PetriSim();

            simulation->setState(command);

            simulations[maxid++] = simulation;

            message = "<simulid id=\""+QString::number(maxid-1)+"\">";

            qDebug() << "[info] sending simulation id";
            return true;
        } else if (strcmd == "run") {
            qDebug() << "[info] user wants to run simulation";
            runSimulation(args["id"],true);
        } else if (strcmd == "step") {
            qDebug() << "[info] user wants to step simulation";
            runSimulation(args["id"],false);
        }
    }
    return true;
}

int PNSimThread::logUser(QString login, QString password)
{
    iomutex->lock();
    QFile users(usersFile);

    if (!users.open(QIODevice::ReadOnly | QIODevice::Text)) {
        iomutex->unlock();
        qCritical("[err] cannot open file with users");
        return 3;
    }

    QTextStream filestream(&users);
    while (!filestream.atEnd()) {
        QString line = filestream.readLine();
        QStringList items = line.split(':');
        if (items.size() != 2) continue;
        if (items[0] == login && items[1] == password) {
            iomutex->unlock();
            return 0;
        }
        if (items[0] == login) {
            iomutex->unlock();
            return 2;
        }
    }

    iomutex->unlock();
    return 1;
}

QByteArray PNSimThread::createMessage(QString message)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (qint64)0;
    out << message;
    out.device()->seek(0);
    out << (qint64)(block.size() - sizeof(qint64));
    return block;
}

int PNSimThread::registerUser(QString login, QString password)
{
    if (login == "" || password == "" || login.count(':') != 0 || password.count(':') != 0) {
        return 3;
    }
    iomutex->lock();
    QFile users(usersFile);

    if (!users.open(QIODevice::ReadWrite | QIODevice::Text)) {
        iomutex->unlock();
        qCritical("[err] cannot open file with users");
        return 2;
    }

    QTextStream filestream(&users);
    while (!filestream.atEnd()) {
        QString line = filestream.readLine();
        QStringList items = line.split(':');
        if (items.size() != 2) continue;
        if (items[0] == login) {
            iomutex->unlock();
            return 1;
        }
    }
    filestream << login << ":" << password << endl;
    iomutex->unlock();
    return 0;
}

QString PNSimThread::getSimulations()
{
    iomutex->lock();
    if (!QDir(simDirectory).exists()) {
        QDir().mkdir(simDirectory);
    }
    QDir dir(simDirectory);

    dir.setFilter(QDir::Files | QDir::Readable);

    QString result;
    QXmlStreamWriter xml(&result);
    xml.writeStartDocument();
    xml.writeStartElement("simul-list");

    QFileInfoList files = dir.entryInfoList();

    foreach(QFileInfo info, files) {
        QFile soubor(info.absoluteFilePath());
        if (!soubor.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "[err] cannot open file";
            continue;
        }
        QXmlStreamReader inxml(&soubor);
        inxml.readNext();
        inxml.readNext();
        if (inxml.atEnd() || inxml.hasError()) {
            continue;
        }
        xml.writeEmptyElement("simul-item");
        xml.writeAttribute("author",inxml.attributes().value("author").toString());
        xml.writeAttribute("name",inxml.attributes().value("name").toString());
        xml.writeAttribute("version",inxml.attributes().value("version").toString());
        xml.writeAttribute("info",inxml.attributes().value("info").toString());
    }
    xml.writeEndElement();
    xml.writeEndDocument();
    iomutex->unlock();
    return result;
}

bool PNSimThread::getCommand(QString xml, QString &result, StrToStrMap &args)
{
    QXmlStreamReader input(xml);

    if (input.readNext() != QXmlStreamReader::StartDocument) {
        return false;
    }
    input.readNext();
    if (input.atEnd() || input.hasError()) {
        qCritical() << "[err] bad XML request";
        return false;
    }
    result = input.name().toString();
    foreach (QXmlStreamAttribute attrib, input.attributes()) {
        args[attrib.name().toString()] = attrib.value().toString();
    }
    return true;
}

QString PNSimThread::loadSim(QString name, QString version)
{
    iomutex->lock();
    if (!QDir(simDirectory).exists()) {
        iomutex->unlock();
        return "false";
    }
    QDir dir(simDirectory);
    dir.setFilter(QDir::Files | QDir::Readable | QDir::Writable);

    QFileInfoList files = dir.entryInfoList();

    foreach(QFileInfo info, files) {
        QFile file(info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "[err] cannot open file";
            continue;
        }
        QXmlStreamReader inxml(&file);
        inxml.readNext();
        inxml.readNext();
        if (inxml.atEnd() || inxml.hasError()) {
            continue;
        }
        if (inxml.attributes().value("name") == name && inxml.attributes().value("version") == version) {
            file.close();

            if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                iomutex->unlock();
                qCritical("[err] cannot open file.");
                return "false";
            }

            QString str;
            QTextStream stream(&file);

            while (!stream.atEnd()) {
                str += stream.readLine();
            }

            PetriSim *simulation = new PetriSim();

            simulation->setState(str);

            simulations[maxid++] = simulation;

            iomutex->unlock();
            return simulation->getState();
        }
    }

    iomutex->unlock();
    return "false";
}

PNSimThread::~PNSimThread()
{
    foreach (PetriSim *simulation, simulations) {
        delete simulation;
    }
}

bool PNSimThread::saveSimulation(QString xml)
{
    iomutex->lock();

    QDir dir(simDirectory);
    dir.setFilter(QDir::Files | QDir::Readable | QDir::Writable);
    int maxversion = 1;
    QXmlStreamReader simXml(xml);
    simXml.readNext();
    simXml.readNext();
    if(simXml.atEnd() || simXml.hasError()) {
        qCritical() << "[err] bad simulation file";
        iomutex->unlock();
        return false;
    }

    QString name = simXml.attributes().value("name").toString();

    QFileInfoList files = dir.entryInfoList();

    foreach(QFileInfo info, files) {
        QFile file(info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "[err] cannot open file";
            continue;
        }
        QXmlStreamReader inxml(&file);
        inxml.readNext();
        inxml.readNext();
        if (inxml.atEnd() || inxml.hasError()) {
            continue;
        }
        if (inxml.attributes().value("name") == name) {
            int thisversion = inxml.attributes().value("version").toString().toInt();
            if (thisversion >= maxversion) maxversion = thisversion+1;
        }
    }

    SimState state;
    state.setState(xml);
    state.version = maxversion;
    QString fileName = dir.absoluteFilePath(name+QString::number(maxversion)+".xml");
    QFile output(fileName);
    if (output.exists()) {
        qCritical() << "[err] file exists";
        iomutex->unlock();
        return false;
    }

    if (!output.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "[err] cannot open file";
        iomutex->unlock();
        return false;
    }

    QTextStream stream(&output);
    stream << state.getState();

    qDebug() << "[info] saved";
    iomutex->unlock();
    return true;
}

void PNSimThread::runSimulation(QString id, bool run_or_step)
{
    RunSimThread *thread = new RunSimThread(id,simulations[id.toInt()],simmutex,outid,run_or_step);
    connect(thread,SIGNAL(finished()),this,SLOT(handleSimuled()),Qt::DirectConnection);
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    thread->start();
}

void PNSimThread::handleSimuled()
{
    qDebug() << "[info] simulated: " << (*outid);
    simmutex->unlock();
    idsToSend.push_back((*outid));
    if (sockmutex.tryLock()) {
        commSock->blockSignals(true);
        foreach (QString id, idsToSend) {
            QString message = "<simul id=\""+id+"\">";
            message += simulations[(*outid).toInt()]->getState();
            message += "</simul>";
            qDebug() << "[info] sending simulation";
            commSock->write(createMessage(message));
        }
        idsToSend.clear();
        sockmutex.unlock();
        commSock->blockSignals(false);
    }
}
