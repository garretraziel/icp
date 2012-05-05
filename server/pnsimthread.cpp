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

PNSimThread::PNSimThread(int socketDescriptor, QObject *parent) :
    QThread(parent),socketDescriptor(socketDescriptor)
{
    isLogged = false;
    usersFile = "./users.dat";
    logFile = "./log.dat";
    simDirectory = "./sims";

}

void PNSimThread::readIncoming(){
    qDebug() << "incomming!";
    if(commSock->bytesAvailable() < (int)sizeof(qint64))
        return;

    QDataStream in(commSock);
    in.setVersion(QDataStream::Qt_4_0);

    in >> block;
    qDebug() << "velikost: " << block;
    if(commSock->bytesAvailable() < block) return;

    QString command;
    in >> command;
    QString message = "";
    block = 0;
    if (!handleCommand(command,message)) {
        commSock->write(createMessage(message));
        commSock->disconnectFromHost();
        return;
    }
    if (message != "")
        commSock->write(createMessage(message));


}

void PNSimThread::handleDisconnection(){
    //todo
    this->exit();
}

void PNSimThread::run()
{

    commSock = new QTcpSocket;
    connect(commSock, SIGNAL(readyRead()), this, SLOT(readIncoming()),Qt::DirectConnection);
    connect(commSock, SIGNAL(disconnected()), this, SLOT(handleDisconnection()), Qt::DirectConnection);

    if (!commSock->setSocketDescriptor(socketDescriptor)) {
        emit error(commSock->error());
        return;
    }
    block = 0;
    commSock->waitForConnected(-1);

    qDebug() << "davam connect";

    this->exec();
    qDebug() << "odpojeno";
}

bool PNSimThread::handleCommand(QString command, QString &message)
{
    qDebug() << command;
    QString strcmd;
    StrToStrMap args;

    if (!getCommand(command,strcmd,args)) {
        message = "<err info=\"Bad command\"/>";
        return false;
    }

    if (strcmd == "register") {
        if (!args.contains("name") || !args.contains("password") || args["name"] == "" || args["password"] == "") {
            qDebug() << "nemuzu lognout";
            message = "<err info=\"Password and name cannot be blank\"/>";
            return false;
        }
        int result = registerUser(args["name"],args["password"]);
        if (result != 0) {
            if (result == 1) {
                qDebug() << "already registere";
                message = "<err info=\"User is already registered\"/>";
                return false;
            } else if (result == 2) {
                qDebug() << "spatny soubor";
                message = "<err info=\"Cannot read usersfile, probably server's problem\"/>";
                return false;
            } else {
                qDebug() << "asi obsahuje dvojtecku";
                message = "<err info=\"Bad login or password\"/>";
                return false;
            }
        } else {
            isLogged = true;
            message = "<ok/>";
            qDebug() << "zalogovat, zaregistrovan";
        }
    } else if (strcmd == "login") {
        if (!args.contains("name") || !args.contains("password") || args["name"] == "" || args["password"] == "") {
            qDebug() << "nemuzu lognout";
            message = "<err info=\"Password and name cannot be blank\"/>";
            return false;
        }
        int result = logUser(args["name"], args["password"]);
        if (result != 0) {
            if (result == 1) {
                qDebug() << "doesn't exists";
                message = "<err info=\"User doesn't exist\"/>";
                return false;
            } else if (result == 2) {
                qDebug() << "bad password";
                message = "<err info=\"Bad password\"/>";
                return false;
            } else {
                qDebug() << "file with users doesn't exists";
                message = "<err info=\"User file doesn't exist\"/>";
                return false;
            }
        } else {
            isLogged = true;
            message = "<ok/>";
            qDebug() << "zalogovan";
        }
    } else if (!isLogged) {
        message = "<err info=\"Not logged on\"/>";
        qDebug() << "not logged!";
        return false;
    } else {
        if (strcmd == "list-simuls") {
            qDebug() << "vypisuju";
            message = getSimulations();
            return true;
        } else if (strcmd == "simul-that") {
            qDebug() << "loaduju";
            message = loadSim(args["name"],args["version"]);
            return true;
        }
    }
    return true;
}

int PNSimThread::logUser(QString login, QString password)
{
    QFile users(usersFile);

    if (!users.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("Error: cannot open file with users");
        return 3;
    }

    QTextStream filestream(&users);
    while (!filestream.atEnd()) {
        QString line = filestream.readLine();
        QStringList items = line.split(':');
        if (items.size() != 2) continue;
        if (items[0] == login && items[1] == password) return 0;
        if (items[0] == login) return 2;
    }

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

    QFile users(usersFile);

    if (!users.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qCritical("Error: cannot open file with users");
        return 2;
    }

    QTextStream filestream(&users);
    while (!filestream.atEnd()) {
        QString line = filestream.readLine();
        QStringList items = line.split(':');
        if (items.size() != 2) continue;
        if (items[0] == login) return 1;
    }
    filestream << login << ":" << password << endl;
    return 0;
}

QString PNSimThread::getSimulations()
{
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
            qCritical() << "Error: cannot open file";
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
    qDebug() << result;
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
        qCritical() << "Bad XML request";
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
    if (!QDir(simDirectory).exists()) {
        return "false";
    }
    QDir dir(simDirectory);
    dir.setFilter(QDir::Files | QDir::Readable | QDir::Writable);

    QFileInfoList files = dir.entryInfoList();

    foreach(QFileInfo info, files) {
        QFile file(info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "Error: cannot open file";
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
                qCritical("Cannot open file.");
                return "false";
            }

            QString str;
            QTextStream stream(&file);

            while (!stream.atEnd()) {
                str += stream.readLine();
            }

            PetriSim *simulation = new PetriSim();

            simulation->setState(str);

            simulations.push_back(simulation);

            return simulation->getState();
        }
    }

    return "false";
}

PNSimThread::~PNSimThread()
{
    foreach (PetriSim *simulation, simulations) {
        delete simulation;
    }
}
