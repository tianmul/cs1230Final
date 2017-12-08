#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "view.h"
#include <QGLFormat>
//#include "SupportCanvas3D.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    // Resize the window because the window is huge since all docks were visible.
    resize(800, 600);

    show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
