#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cd = new ConnectDialog(this);

    QObject::connect(ui->actionConnect_to_server, SIGNAL(activated()),
                     this, SLOT(showConnectDialog()) );
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cd;
}

void MainWindow::showConnectDialog()
{
    cd->show();
}
