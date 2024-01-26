#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duplicatefsmodel.h"

#include <QFileSystemModel>
#include <QFileDialog>
#include <QTreeView>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(nullptr) {

    ui->setupUi(this);
    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(openFolder()));
}

MainWindow::~MainWindow() {
    delete ui;
    delete model;
}

void MainWindow::openFolder() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Open directory"),
                                                    "/home", QFileDialog::ShowDirsOnly);
    if (!path.isEmpty()) {
        if (model) {
            delete model;
        }
        model = new DuplicateFSModel(path, this);
        ui->treeView->setModel(model);
    }
}

