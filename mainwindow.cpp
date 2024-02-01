#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duplicatefsmodel.h"

#include <QFileSystemModel>
#include <QFileDialog>
#include <QTreeView>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(nullptr) {
    ui->setupUi(this);
    this->setWindowTitle("Delete duplicated files");

    QStringList comboBoxItemsList = { "All", "Images", "Documents", "Music",
                                      "Videos", "Archives", "Executables" };
    ui->filesFormatComboBox->addItems(comboBoxItemsList);

    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(openFolderButtonSlot()));
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(searchDuplicatesButtonSlot()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteDuplicatesButtonSlot()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(actionAboutQtSlot()));
}

MainWindow::~MainWindow() {
    delete ui;
    delete model;
}

void MainWindow::openFolderButtonSlot() {
    QString path = QFileDialog::getExistingDirectory(this, tr("Open directory"),
                                                    "/home", QFileDialog::ShowDirsOnly);
    if (!path.isEmpty()) {
        if (model) {
            delete model;
        }
        model = new DuplicateFSModel(path, this);
        ui->treeView->setModel(model);
        ui->treeView->setColumnWidth(0, ui->treeView->width()/3);
        ui->treeView->setColumnWidth(1, ui->treeView->width()/6);
    }
}

void MainWindow::searchDuplicatesButtonSlot() {
    if (!model) {
        QMessageBox msgbox;
        msgbox.setText("Please select folder to search duplicates in");
        msgbox.exec();
        return;
    }
    model->findDuplicates();
}

void MainWindow::deleteDuplicatesButtonSlot() {
    if (!model) {
        return;
    }
    int deletedFilesAmount = model->deleteDuplicates();
    qDebug() << deletedFilesAmount;
    QMessageBox msgbox;
    msgbox.setText(QString("Deleted %1 files").arg(deletedFilesAmount));
    msgbox.exec();
    qDebug() << "WTF";
}

void MainWindow::actionAboutQtSlot() {
    QMessageBox::aboutQt(this);
}

