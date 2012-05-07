/**
 * @file pnsimthread.cpp
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Trida pro vlakno, ktere komunikuje s uzivatelem a spousti simulace.
 */

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
#include <QTime>
#include <QDate>
#include "runsimthread.h"

QMutex sockmutex; ///< semafor pro pristup k socketu

PNSimThread::PNSimThread(int socketDescriptor, QMutex *iomutex, QObject *parent) :
    QThread(parent),iomutex(iomutex),socketDescriptor(socketDescriptor)
{
    isLogged = false; //pripojeny uzivatel NENI zalogovan
    usersFile = "./users.dat";
    logFile = "./log.dat";
    simDirectory = "./sims";
    maxid = 0; //pro cislovani otevrenych simulaci
}

void PNSimThread::readIncoming(){
    sockmutex.lock(); //kdyby doslo k dokonceni simulace, NESMI se zatim odeslat
    qDebug() << "[info] message from user";

    QDataStream in(commSock);
    in.setVersion(QDataStream::Qt_4_0);

    if(block == 0) { //k dispozici jeste neni ani velikost zpravy
        if(commSock->bytesAvailable() < (int)sizeof(quint32)) {
            sockmutex.unlock();
            return;
        }

        in >> block; //k dispozici uz je velikost zpravy, nactu
    }

    qDebug() << "[info] size:" << block;
    qDebug() << "[info] available: " << commSock->bytesAvailable();
    if(commSock->bytesAvailable() < block) { //k dispozici jeste neni cela zprava
        sockmutex.unlock();
        return;
    }
    //k dispozici JE cela zprava, nactu ji
    QString command;
    in >> command;
    QString message = "";
    block = 0;
    if (!handleCommand(command,message)) { //pokusim se vykonat prikaz
        commSock->write(createMessage(message)); //pokud vratil false, zapisu chybu
        commSock->disconnectFromHost(); //a odpojim klienta
        sockmutex.unlock();
        return;
    }
    if (message != "") { //prikaz se provedl spravne, zapisu zpravu, pokud potrebuji
        commSock->write(createMessage(message));
    }

    foreach (QString id, idsToSend) { //pokud se mezi tim vyhodnotily nejake simulace, ted je mohu poslat
        QString message = "<simul id=\""+id+"\">";
        message += simulations[(*outid).toInt()]->getState();
        message += "</simul>";
        qDebug() << "[info] sending simulation";
        commSock->write(createMessage(message));
    }
    idsToSend.clear();

    sockmutex.unlock(); //odemku semafor, aby se mohly simulace zapisovat i asynchronne
}

void PNSimThread::handleDisconnection(){
    this->exit(); //pri odpojeni zastavim vlakno
}

void PNSimThread::run()
{
    commSock = new QTcpSocket; //pripojil se novy uzivatel, vytvorim socket
    simmutex = new QMutex;
    outid = new QString;

    //pripojim signaly pro cteni ze socketu a pro odpojeni
    //pouzivani Qt::DirectConnection asi neni moc ciste ale je to jediny zpusob, jak
    // jednoduse pouzivat signaly v ramci vlakna
    connect(commSock, SIGNAL(readyRead()), this, SLOT(readIncoming()),Qt::DirectConnection);
    connect(commSock, SIGNAL(disconnected()), this, SLOT(handleDisconnection()), Qt::DirectConnection);

    //nepovedl se vytvorit socket
    if (!commSock->setSocketDescriptor(socketDescriptor)) {
        emit error(commSock->error());
        return;
    }
    block = 0; //blok inicializuji
    commSock->waitForConnected(-1);

    this->exec(); //zapnu naslouchaci smycku

    delete commSock; //uvolnim pamet
    delete simmutex;
    delete outid;
    qDebug() << "[info] user disconnected";
}

bool PNSimThread::handleCommand(QString command, QString &message)
{
    QString strcmd; //prikaz samotny
    StrToStrMap args; //argumenty

    if (!getCommand(command,strcmd,args)) { //proparsuju prikaz a argumenty
        message = "<err info=\"Bad command\"/>";
        return false;
    }

    if (strcmd == "register") { //uzivatel se chce zaregistrovat
        if (!args.contains("name") || !args.contains("password") || args["name"] == "" || args["password"] == "") {
            //chyba ve jmenu a heslu
            qCritical() << "[err] cannot login";
            message = "<err info=\"Password and name cannot be blank\"/>";
            return false;
        }
        int result = registerUser(args["name"],args["password"]);
        if (result != 0) {
            //z nejakeho duvodu se nepodarilo zaregistrovat
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
            //uzivatel se spravne zaregistroval
            isLogged = true;
            userName = args["name"];
            message = "<ok/>";
            qDebug() << "[info] user logged in";
        }
    } else if (strcmd == "login") {
        //uzivatel se chce prihlasit
        if (!args.contains("name") || !args.contains("password") || args["name"] == "" || args["password"] == "") {
            qCritical() << "[err] cannot login";
            message = "<err info=\"Password and name cannot be blank\"/>";
            return false;
        }
        //zadal jmeno i heslo
        int result = logUser(args["name"], args["password"]);
        if (result != 0) {
            //z nejakeho duvodu se nepodarilo prihlasit
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
            //uzivatel se spravne prihlasil
            isLogged = true;
            userName = args["name"];
            message = "<ok/>";
            qDebug() << "[info] user logged in";
        }
    } else if (!isLogged) {
        //uzivatel chce provadet prikazy, ale neni prihlasen
        message = "<err info=\"Not logged on\"/>";
        qCritical() << "[err] user is not logged in!";
        return false;
    } else {
        //vse v poradku
        if (strcmd == "list-simuls") {
            //uzivatel chce vypsat simulace na serveru
            qDebug() << "[info] list simulations";
            message = getSimulations();
            return true;
        } else if (strcmd == "simul-that") {
            //uzivatel chce simulovat na serveru ulozenou simulaci
            qDebug() << "[info] loading simulation";
            QString net = loadSim(args["name"],args["version"]); //nactu simulaci
            if (net == "false") {
                message = "<err info=\"Cannot load petrinet\"/>";
                return true;
            }
            message = "<simul id=\""+QString::number(maxid-1)+"\">";
            message += net; //poslu uzivateli ID simulace
            message += "</simul>";
            qDebug() << "[info] sending new simulation";
            return true;
        } else if (strcmd == "save-this") {
            //uzivatel chce ulozit simulaci na server
            command.remove(QRegExp("^<save-this>"));
            command.remove(QRegExp("</save-this>$"));
            if (!saveSimulation(command)) {
                message = "<err info=\"Cannot save simulation\"/>";
                return true;
            }
            PetriSim *simulation = new PetriSim(); //vytvori se nova simulace

            simulation->setState(command); //nastavi se ji stav

            simulations[maxid++] = simulation; //ulozi se do simulaci

            message = "<simulid id=\""+QString::number(maxid-1)+"\">"; //uzivateli se zasle zpet ID simulace

            qDebug() << "[info] sending simulation id";
            return true;
        } else if (strcmd == "run") {
            //uzivatel chce spustit simulaci
            qDebug() << "[info] user wants to run simulation";
            runSimulation(args["id"],true);
        } else if (strcmd == "step") {
            //uzivatel chce jeden krok simulace
            qDebug() << "[info] user wants to step simulation";
            runSimulation(args["id"],false);
        }
    }
    return true;
}

int PNSimThread::logUser(QString login, QString password)
{
    iomutex->lock(); //pristup k i/o
    QFile users(usersFile);

    if (!users.open(QIODevice::ReadOnly | QIODevice::Text)) {
        iomutex->unlock();
        qCritical("[err] cannot open file with users");
        return 3; //soubor s uzivateli vubec neexistuje
    }

    QTextStream filestream(&users);
    while (!filestream.atEnd()) {
        QString line = filestream.readLine();
        QStringList items = line.split(':');
        if (items.size() != 2) continue;
        if (items[0] == login && items[1] == password) {
            iomutex->unlock();
            return 0; //nasel jsem uzivatele, jmeno-heslo odpovida
        }
        if (items[0] == login) {
            iomutex->unlock();
            return 2; //nasel jsem uzivatele, heslo neodpovida
        }
    }

    iomutex->unlock();
    return 1; //nenasel jsem uzivatele
}

QByteArray PNSimThread::createMessage(QString message)
{
    //pomoci QDataStreamu vytvori zpravu vhodnou pro zapsani
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)0;
    out << message;
    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));
    return block;
}

int PNSimThread::registerUser(QString login, QString password)
{
    //uzivatel se chce zaregistrovat
    if (login == "" || password == "" || login.count(':') != 0 || password.count(':') != 0) {
        return 3; //dvojtecka nesmi byt v hesle
    }
    iomutex->lock();
    QFile users(usersFile);

    if (!users.open(QIODevice::ReadWrite | QIODevice::Text)) {
        iomutex->unlock();
        qCritical("[err] cannot open file with users");
        return 2; //nepodarilo s otevrit soubor
    }

    QTextStream filestream(&users);
    while (!filestream.atEnd()) {
        QString line = filestream.readLine();
        QStringList items = line.split(':');
        if (items.size() != 2) continue;
        if (items[0] == login) { //uzivatel je uz zaregistrovan
            iomutex->unlock();
            return 1;
        }
    }
    filestream << login << ":" << password << endl; //zapise zaznam o uzivateli
    iomutex->unlock();
    return 0;
}

QString PNSimThread::getSimulations()
{
    iomutex->lock(); //bude se provadet i/o operace
    if (!QDir(simDirectory).exists()) {
        QDir().mkdir(simDirectory); //neexistuje slozka se simulacemi
    }
    QDir dir(simDirectory);

    dir.setFilter(QDir::Files | QDir::Readable);

    QString result;
    QXmlStreamWriter xml(&result); //vracet se bude XML
    xml.writeStartDocument();
    xml.writeStartElement("simul-list");

    QFileInfoList files = dir.entryInfoList(); //ziskam seznam souboru

    foreach(QFileInfo info, files) {
        QFile soubor(info.absoluteFilePath()); //ziskam absolutni cestu
        if (!soubor.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "[err] cannot open file";
            continue; //nemam pristup k souboru
        }
        QXmlStreamReader inxml(&soubor);
        inxml.readNext();
        inxml.readNext();
        if (inxml.atEnd() || inxml.hasError()) {
            continue; //asi to neni soubor se simulaci
        }
        //ziskam informace o simulaci
        xml.writeEmptyElement("simul-item");
        xml.writeAttribute("author",inxml.attributes().value("author").toString());
        xml.writeAttribute("name",inxml.attributes().value("name").toString());
        xml.writeAttribute("version",inxml.attributes().value("version").toString());
        xml.writeAttribute("info",inxml.attributes().value("info").toString());
    }
    xml.writeEndElement();
    xml.writeEndDocument();
    iomutex->unlock();
    return result; //vysledek poslu uzivateli
}

bool PNSimThread::getCommand(QString xml, QString &result, StrToStrMap &args)
{
    //proparsuju XML s prikazem
    QXmlStreamReader input(xml);

    if (input.readNext() != QXmlStreamReader::StartDocument) {
        return false;
    }
    input.readNext();
    if (input.atEnd() || input.hasError()) {
        qCritical() << "[err] bad XML request";
        return false;
    }
    result = input.name().toString(); //ziskam prikaz
    foreach (QXmlStreamAttribute attrib, input.attributes()) { //ziskam atributy a jejich hodnoty
        args[attrib.name().toString()] = attrib.value().toString();
    }
    return true;
}

QString PNSimThread::loadSim(QString name, QString version)
{
    //nacteni simulace
    iomutex->lock(); //bude se provadet i/o operace
    if (!QDir(simDirectory).exists()) {
        iomutex->unlock();
        return "false"; //simulace neexistuje
    }
    QDir dir(simDirectory);
    dir.setFilter(QDir::Files | QDir::Readable | QDir::Writable);

    QFileInfoList files = dir.entryInfoList(); //ziskam soubory se simulacemi

    foreach(QFileInfo info, files) {
        QFile file(info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical() << "[err] cannot open file";
            continue;
        }
        QXmlStreamReader inxml(&file);
        inxml.readNext();
        inxml.readNext();
        if (inxml.atEnd() || inxml.hasError()) { //asi neni soubor se simulacemi
            continue;
        }
        if (inxml.attributes().value("name") == name && inxml.attributes().value("version") == version) {
            //nasel jsem pozadovanou simulaci
            file.close();

            if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                iomutex->unlock();
                qCritical("[err] cannot open file.");
                return "false";
            }

            QString str;
            QTextStream stream(&file);

            while (!stream.atEnd()) { //nactu cely soubor
                str += stream.readLine();
            }

            PetriSim *simulation = new PetriSim(); //vytvorim novou simulaci

            simulation->setState(str); //nastavim jeji stav na nactene XML

            simulations[maxid++] = simulation; //ulozim do otevrenych simulaci

            iomutex->unlock();
            logRun(name,version,userName); //zaloguju nacteni simulace
            return simulation->getState(); //vratim XML se simulaci
        }
    }

    iomutex->unlock();
    return "false";
}

PNSimThread::~PNSimThread()
{
    foreach (PetriSim *simulation, simulations) {
        delete simulation; //smazu veskere otevrene simulace
    }
}

bool PNSimThread::saveSimulation(QString xml)
{
    iomutex->lock(); //i/o operace

    QDir dir(simDirectory);
    dir.setFilter(QDir::Files | QDir::Readable | QDir::Writable);
    int maxversion = 1; //zde bude ulozena nejvyssi verze nalezene simulace
    QXmlStreamReader simXml(xml);
    simXml.readNext();
    simXml.readNext();
    if(simXml.atEnd() || simXml.hasError()) {
        qCritical() << "[err] bad simulation file";
        iomutex->unlock();
        return false;
    }

    //ziskam jmeno a verzi ukladane simulace
    QString name = simXml.attributes().value("name").toString();

    QFileInfoList files = dir.entryInfoList(); //ziskam soubory se simulacemi

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
        //toto je stejna simulace, musim navysit verzi
        if (inxml.attributes().value("name") == name) {
            int thisversion = inxml.attributes().value("version").toString().toInt();
            if (thisversion >= maxversion) maxversion = thisversion+1;
        }
    }

    SimState state;
    state.setState(xml);
    state.version = maxversion; //nastavim verzi na o jedna vyssi nez je nejvyssi
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
    stream << state.getState(); //zapisi XML do souboru

    qDebug() << "[info] saved";
    iomutex->unlock();
    logRun(name,QString::number(maxversion),userName); //zaloguji, ze uzivatel nacetl simulaci
    return true;
}

void PNSimThread::runSimulation(QString id, bool run_or_step)
{
    if (!simulations.contains(id.toInt())) { //ID neodpovida otevrene simulaci
        qCritical() << "[err] simulation ID doesn't exist";
        return;
    }
    //vytvorim nove vlakno pro beh simulace
    RunSimThread *thread = new RunSimThread(id,simulations[id.toInt()],simmutex,outid,run_or_step);
    connect(thread,SIGNAL(finished()),this,SLOT(handleSimuled()),Qt::DirectConnection);
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    thread->start();
}

void PNSimThread::handleSimuled()
{
    //tento slot se spusti, jakmile se odsimuluje jedna simulace - skonci jeji thread
    qDebug() << "[info] simulated: " << (*outid);
    simmutex->unlock(); //muzou koncit i dalsi simulace
    idsToSend.push_back((*outid)); //simulaci s timto ID mam poslat zpet
    if (sockmutex.tryLock()) { //pokud se nezamkne, server zrovna konunikuje s uzivatelem
        commSock->blockSignals(true); //na chvili blokuji signaly
        foreach (QString id, idsToSend) { //poslu vsechny dokoncene signaly
            QString message = "<simul id=\""+id+"\">";
            message += simulations[(*outid).toInt()]->getState();
            message += "</simul>";
            qDebug() << "[info] sending simulation";
            commSock->write(createMessage(message));
        }
        idsToSend.clear();
        sockmutex.unlock(); //open se muze komunikovat standardni cestou
        commSock->blockSignals(false);
    }
}

void PNSimThread::logRun(QString name, QString version, QString user)
{
    QFile log(logFile);
    //otevru soubor s logem
    if (!log.open(QIODevice::Append | QIODevice::Text)) {
        if (!log.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
    }
    QString outstr = QDate::currentDate().toString("yyyy-MM-dd") + " ";
    outstr += QTime::currentTime().toString("HH:mm:ss");
    outstr += ": "+user+" loaded "+name+" v"+version+"\n";
    QTextStream ts(&log);
    ts << outstr; //zapisu zaznam
}
