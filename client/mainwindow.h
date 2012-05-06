/**
 * @file mainwindow.h
 * @author  Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 * @author  Jan Sedlak   <xsedla85@stud.fit.vutbr.cz>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Trida pro hlavni okno, ktere slouzi jako klientsky editor a
 * prohlizec siti.
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectdialog.h"
#include "editdialog.h"
#include "pngui.h"
#include <vector>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "aboutdialog.h"
#include "../server/simstate.h"
#include "../server/pnplace.h"
#include "../server/pntrans.h"
#include <QFileDialog>
#include "propertiesdialog.h"
#include "loadsimdialog.h"
#include "helpdialog.h"

namespace Ui {
class MainWindow;
}

/**
 *
 * Trida zastitujici zobrazeni a praci s hlavnim oknem
 *
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    /**
      * Konstruktor vytvarejici MainWindow
      * @param predek pro uklid
      */
    explicit MainWindow(QWidget *parent = 0);

    /**
      * Destruktor MainWindow
      */
    ~MainWindow();

    /**
      * Ziska ukazatel na edit dialog
      * @return ukazatel na editDialog
      */
    editDialog * getEditor();

    /**
      * Ziska aktualni simulaci
      * @return ukazatel na aktualni simulaci
      */
    SimState * getCurrentSim();

    /**
      * Ziska aktualni index
      * @return aktualni index
      */
    int getCurrentIndex();

    /**
      * Nastavi jmeno simulaci (do tab widgetu)
      * @param name jmeno simulace
      */
    void setSimName(QString name);

    /**
      * Nastavi ID simulace
      * @param id id simulace
      */
    void setID(QString id);

    /**
      * Ziksa ID simulace
      * @return ID simulace
      */
    QString getID();

    /**
      * Zobrazi ID simulace na index
      * @param _id ID simulace
      * @return index
      */
    int findID(QString _id);

    /**
      * Znovu nacte simulaci a prepne na jeji tab
      * @param _id ID simulace
      * @param newSimstate XML stav simulace
      */
    void simReload(QString _id, QString newSimState);

    /**
      * Nastavi online/offline status simulace a barvu
      * @param status text statusu
      * @param barava statusu
      */
    void setStatusLabel(QString status, QString color);

private:
    Ui::MainWindow *ui;     ///< user interface

    ConnectDialog * cd;     ///< connect dialog pointer
    aboutDialog * ad;       ///< about dialog pointer
    propertiesDialog * pd;  ///< properties dialog pointer
    loadSimDialog * ld;     ///< loadsim dialog pointer
    helpDialog * hd;        ///< help dialog pointer

    QLabel * statusLabel;   ///< status label (online/offline status)

    /**
      * Nacte simulaci z XML retezce, ulozi do aktualni
      * @param simString XML stav simulace
      */
    void __loadSimString(QString simString);

    /**
      * Nacte simulaci z XML ..., neotevira novy tab
      * @param simString XML stav simulace
      */
    void __loadSimStringNoNewTab(QString simString);

    /**
      * Prida misto do aktualni sceny
      * @param simPlace ukazatel na misto do SimState
      * @return ukazatel na misto
      */
    pnItem * __addItem(PNPlace * simPlace);

    /**
      * Prida prechod do aktialni sceny
      * @param simTrans ukazatel na prechod do SimState
      * @return ukazatel na prechod
      */
    pnItem * __addItemRect(PNTrans * simTrans);

    std::vector<QWidget *> tabVect;             ///< vektor tabu
    std::vector<QGraphicsView *> viewVect;      ///< vektor zobrazovacu scen
    std::vector<QGraphicsScene *> canvasVect;   ///< vektor scen


    std::vector<SimState *> simVect;            ///< vektor simulaci
    std::vector<QString> idVect;                ///< vektor ID simulaci

    editDialog * editor;                        ///< ukazatel na editor prvku

    /**
      * Nacte simulaci ze souboru
      * @param fileName jmeno souboru
      */
    void __loadSim(QString fileName);

    /**
      * Ziskani aktualniho zobrazovace scen
      * @return uakaztel na aktualni zobrazovac scen
      */
    QGraphicsView * currentTabView();

private slots:
    /**
      * Slot volany pred zavrenim okna (uklizeni statusbaru)
      */
    void preClose();

public slots:
    /**
      * Slot aktualizujici informaci o aktualnosti site
      */
    void actAct(int i);

    /**
      * Slot zobrazujici connect dialog
      */
    void showConnectDialog();

    /**
      * Slot zobrazujici about dialog
      */
    void showAboutDialog();

    /**
      * Slot otevirajici novy tab
      */
    void newTab();

    /**
      * Slot pridavajici misto do sceny
      * @return ukazatel na misto (nepouzivan, jen pro uplnost)
      */
    pnItem * addItem();

    /**
      * Slot pridavajici prechod do sceny
      * @return ukazatel na misto (nepouzivan, jen pro uplnost)
      */
    pnItem * addItemRect();

    /**
      * Slot kontrolujici nastaveni mazani (nastavuje globalni promennou)
      */
    void checkErase();

    /**
      * Slot nacitajici simulaci
      */
    void loadSim();

    /**
      * Slot ukladajici simulaci
      */
    void saveSim();

    /**
      * Slot nacitajici lokalno simulaci
      */
    void loadLocalSim();

    /**
      * Slot ukladajici simulaci lokalne
      */
    void saveLocalSim();

    /**
      * Slot pro oddaleni sceny
      */
    void zoomOut();

    /**
      * Slot pro priblizeni sceny
      */
    void zoomIn();

    /**
      * Slot pro editaci vlasnosti site
      */
    void editProperties();

    /**
      * Slot pro naceni vykomunikovane site od serveru
      */
    void simOk();

    /**
      * Slot spoustejici simulaci na serveru (pres communikator)
      */
    void runSim();

    /**
      * Slot krokujici simulaci na serveru (pres komunikator)
      */
    void stepSim();
};

extern MainWindow * mw; /// ukazatel na hlavni okno

#endif // MAINWINDOW_H
