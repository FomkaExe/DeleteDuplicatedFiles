#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duplicatefsmodel.h"

#include <QFileSystemModel>
#include <QFileDialog>
#include <QTreeView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    model = new QFileSystemModel;
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open directory"),
                                                    "/home", QFileDialog::ShowDirsOnly);
    model = new DuplicateFSModel(dir, this);
    ui->treeView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

