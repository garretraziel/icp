/**
 * @file communicator.cpp
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Trida, obstaravajici komunikaci se serverem
 *
 */

#include "communicator.h"
#include <QDataStream>
#include <QByteArray>
#include <QMessageBox>
#include <QDebug>
#include <QXmlStreamReader>
#include <QDataStream>
#include <QRegExp>
#include "mainwindow.h"
#include <QHostAddress>

Communicator communicator(mw);


/**
  * Konstruktor vytvarejici Communicator
  * @param parent predek pro uklid
  */
Communicator::Communicator(QWidget *parent):QObject(parent)
{
    commSock = new QTcpSocket;

}

/**
  * Destruktor Communicator
  */
Communicator::~Communicator()
{
    if (connected()) {
        commSock->disconnectFromHost();
    }

    delete commSock;
}

/**
  * Zjisteni, zda je klient pripojen k serveru
  * @return true pokud je pripojen k serveru
  */
bool Communicator::connected()
{
    if (commSock->state() == QAbstractSocket::ConnectedState) {
        return true;
    } else {
        return false;
    }
}

/**
  * Pripoji k serveru
  * @param hostname hostname nebo IP
  * @param port port, na kterem server nasloucha
  * @return true pri uspechu
  */
bool Communicator::connect(QString hostname, QString port)
{
    commSock->connectToHost(hostname, port.toUInt(),QIODevice::ReadWrite);
    commSock->waitForConnected(-1);
    return connected();
}

/**
  * Posle prikaz serveru
  * @param command prikaz
  * @return true pri uspechu
  */
bool Communicator::sendCommand(QString command)
{
    // vytvori odesilanou zpravu a na zacatek da jeji delku
    if (!connected()) return false;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)0;
    out << command;
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));

    commSock->write(block);

    return true;
}

#define returnWhenTimeout commSock->waitForReadyRead(); \
                          if (commSock->state() == QTcpSocket::UnconnectedState) \
                            return

/**
  * Ziska prikaz poslany ze serveru
  * @param command reference na ziskany prikaz
  * @return true pri uspechu
  */
bool Communicator::recvCommand(QString &command)
{
    // nacte zpravu (ceka dokud nebude dostatek bytu ke cteni)
    quint32 block;
    while (commSock->bytesAvailable() < (int)sizeof(quint32)) {
        returnWhenTimeout false;
    }
    QDataStream in(commSock);
    in.setVersion(QDataStream::Qt_4_0);

    in >> block;

    while (commSock->bytesAvailable() < block) {
        returnWhenTimeout false;
    }
    in >> command;

    return true;
}

/**
  * Zaregistrue nebo zaloguje uzivatele
  * @param what kterou akci provest ("login", "register")
  * @param name jmeno uzivatele
  * @param password heslo
  * @param message reference na informacni zpravu
  * @return true pri uspechu
  */
inline bool Communicator::login_or_register(QString what, QString name, QString password, QString &message)
{
    // vytvori zpravu
    QString sendMessage = "<";
    sendMessage += what;
    sendMessage += " name=\"";
    sendMessage += name;
    sendMessage += "\" password=\"";
    sendMessage += password;
    sendMessage += "\"/>";

    // zasle zpravu
    if (!sendCommand(sendMessage)) {
        message = "Error: cannot send message to server";
        return false;
    }

    // ziska odpoved
    QString recMessage;
    if (!recvCommand(recMessage)) {
        message = "Error: server didn't response";
        return false;
    }

    //zkontroluje jestli neni error a propoji signaly, vrati true
    if(isNotError(recMessage, message)){
        loginName = name;
        QObject::connect(commSock, SIGNAL(readyRead()), this, SLOT(handleIncomming()));
        QObject::connect(commSock, SIGNAL(disconnected()), this, SLOT(setOffline()));
        commSock->blockSignals(false);
        mw->setStatusLabel("Online, connected to: "+commSock->peerName()+
                           " ("+commSock->peerAddress().toString()+":"+QString::number(commSock->peerPort())+")",
                           "#008800");
        block = 0;
        return true;
    }
    else {
        loginName = "";
        return false;
    }
}

/**
  * Zaloguje uzivatele
  * @param name uzivatelske jmeno
  * @param password heslo
  * @param message navratova informacni zprava
  * @return true pri uspechu
  */
bool Communicator::login(QString name, QString password, QString &message)
{
    return login_or_register("login",name,password,message);
}

/**
  * Zaregistruje uzivatele
  * @param name uzivatelske jmeno
  * @param password heslo
  * @param message navratova informacni zprava
  * @return true pri uspechu
  */
bool Communicator::registerUser(QString name, QString password, QString &message)
{
    return login_or_register("register",name,password,message);
}

/**
  * Slot na zobrazeni chybove hlasky pri chybe socketu
  * @param socketError typ chyby socketu
  */
void Communicator::displayError(QAbstractSocket::SocketError socketError)
{
    //podle druhu chyby informuje...
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(NULL, "Connection refused","Connection was refused by the server. Make sure that server is running.");
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(NULL, "Host not found","Hostname was not found. Check hostname and port again.");
        break;
    default:
        QMessageBox::information(NULL, "Error", "Error occured during connecting to server.");
        break;
    }
}

/**
  * Zkontroluje, zda zprava od serveru neni error
  * @param recMessage reference na zpravu ziskanou od serveru
  * @param message reference na informacni zpravu
  * @return true, pokud neni error
  */
bool Communicator::isNotError(QString & recMessage, QString & message){

    QXmlStreamReader xml(recMessage);

    if (xml.readNext() != QXmlStreamReader::StartDocument) {
        message = "Server talks rubbishly.";
        return false;
    }
    xml.readNext();
    if (xml.atEnd() || xml.hasError()) {
        message = "Server talks rubbishly.";
        return false;
    }
    if (xml.name() == "err") {
        message = xml.attributes().value("info").toString();
        return false;
    }

    return true;

}

/**
  * Ulozi simulaci na server
  * @param XmlSimState retezec obsahujici XML stav simulace
  * @param message navratova informacni zprava
  * @return true pri uspechu
  */
bool Communicator::saveSimState(QString xmlSimState, QString & message){

    // vytvori zpravu
    QString sendMessage = "<save-this>"+xmlSimState+"</save-this>";
    commSock->blockSignals(true);

    // zasle
    sendCommand(sendMessage);

    // zsika odpoved
    QString recMessage;
    if (!recvCommand(recMessage)) {
        message = "Error: server didn't response";
        commSock->blockSignals(false);
        return false;
    }
    //zkontroluje chybu
    QString errmessage;
    if (!isNotError(recMessage,errmessage)) {
        message = errmessage;
        commSock->blockSignals(false);
        return false;
    }
    //zpracuje xml
    QXmlStreamReader xml(recMessage);
    xml.readNext();
    xml.readNext();

    if (xml.name() != "simulid") {
        message = "Error: received bad response";
        commSock->blockSignals(false);
        return false;
    }

    mw->setID(xml.attributes().value("id").toString());

    commSock->blockSignals(false);
    return true;
}

/**
  * Ziskani simulaci
  * @param sims reference na seznam simulaci
  * @return true pri uspechu
  */
bool Communicator::getSimulations(simList &sims){
    QString message = "<list-simuls/>";
    // zasle zpravu
    commSock->blockSignals(true);
    if(!sendCommand(message)){
        commSock->blockSignals(false);
        return false;
    }
    QString recMessage;
    // ziska zpravu
    if (!recvCommand(recMessage)) {
        commSock->blockSignals(false);
        return false;
    }
    // postupne ...
    QXmlStreamReader xml(recMessage);
    if (xml.readNext() != QXmlStreamReader::StartDocument) {
        commSock->blockSignals(false);
        return false;
    }
    // ... zpracuje ...
    xml.readNext();
    if (xml.atEnd() || xml.hasError()) {
        commSock->blockSignals(false);
        return false;
    }
    // ... xml ...
    if (xml.name() != "simul-list"){
        commSock->blockSignals(false);
        return false;
    }
    // ... a nastavi vlastnosti site
    while (!xml.atEnd()) {
        if (xml.readNextStartElement()) {
            StringVector list;
            list.push_back(xml.attributes().value("name").toString());
            list.push_back(xml.attributes().value("version").toString());
            list.push_back(xml.attributes().value("author").toString());
            list.push_back(xml.attributes().value("info").toString());


            sims.push_back(list);
        }
    }
    commSock->blockSignals(false);
    return true;
}

/**
  * Nacita ze serveru danou simulaci
  * @param name nazev simulace
  * @param version verze simulace
  * @return true pri uspechu
  */
bool Communicator::loadThis(QString name, QString version){
    QString command;
    //vytvori zpravu a posle
    QXmlStreamWriter xml(&command);
    xml.writeStartDocument();
    xml.writeEmptyElement("simul-that");
    xml.writeAttribute("name",name);
    xml.writeAttribute("version",version);
    xml.writeEndDocument();
    return sendCommand(command);
}

/**
  * Zpracuje prikaz prijaty od serveru
  * @param command prikaz
  * @return true pri uspechu
  */
bool Communicator::handleCommand(QString command){
    QXmlStreamReader xml(command);
    //zracuje prijate xml
    if(xml.readNext()!=QXmlStreamReader::StartDocument){
        errorMsg = "Error: cannot load sim";
        emit simError();
        return false;
    }

    xml.readNext();

    if(xml.atEnd() || xml.hasError()) {
        errorMsg = "Cannot load sim";
        emit simError();
        return false;
    }

    QString strcmd = xml.name().toString();

    if (strcmd == "err") {
        errorMsg = xml.attributes().value("info").toString();
        emit simError();
        return false;
    }
    // emituje signal jesli, kdyz je prijata zprava okay
    if(strcmd == "simul"){
        simID = xml.attributes().value("id").toString();
        command.remove(QRegExp("^<simul[^>]+>"));
        command.remove(QRegExp("</simul>$"));
        sim = command;
        emit simOk();
    }


    return true;
}

/**
  * Slot na zpracovani prichozi zpravy
  */
void Communicator::handleIncomming(){

    QDataStream in(commSock);
    in.setVersion(QDataStream::Qt_4_0);
    // pocka az muze precist delku
    if (block == 0) {
        if(commSock->bytesAvailable() < (int)sizeof(quint32))
            return;

        in >> block;
    }
    // a nacte zpravu dane delky
    if(commSock->bytesAvailable() < block) return;

    QString command;
    in >> command;

    block = 0;
    if (!handleCommand(command)) {

        return;
    }
}

/**
  * Spusti simulaci
  * @param id unikatni ID (ziskane od serveru)
  * @param run_or_step true pokud run, false pokud step
  * @return true pri uspechu
  */
bool Communicator::runSimulation(QString id, bool run_or_step)
{
    QString command;
    if (run_or_step) {
        command = "<run id=\""+id+"\"/>";
    } else {
        command = "<step id=\""+id+"\"/>";
    }

    return sendCommand(command);
}

/**
  * Slot na nastaveni offline hlasky v statusbaru mainwindow
  */
void Communicator::setOffline(){
    //mw->setStatusLabel("Offline","#ff0000");
    //commSock->blockSignals(true);
}

/**
  * Blokuje signaly se socketu
  * @param b true zapne blokovani, false vypne
  */
void Communicator::blockSocket(bool b){
    commSock->blockSignals(b);
    QObject::disconnect(this,SLOT(setOffline()));
}
