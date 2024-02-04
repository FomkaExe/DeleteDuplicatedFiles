#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duplicatefsmodel.h"

#include <QFileSystemModel>
#include <QFileDialog>
#include <QTreeView>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , model(nullptr) {
    ui->setupUi(this);
    this->setWindowTitle("Delete duplicated files");

    QStringList comboBoxItemsList = { "All", "Images", "Documents",
                                      "Music", "Videos" };
    ui->filesFormatComboBox->addItems(comboBoxItemsList);

    connect(ui->openButton, SIGNAL(clicked()),
            this, SLOT(openFolderButtonSlot()));
    connect(ui->searchButton, SIGNAL(clicked()),
            this, SLOT(searchDuplicatesButtonSlot()));
    connect(ui->deleteButton, SIGNAL(clicked()),
            this, SLOT(deleteDuplicatesButtonSlot()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()),
            this, SLOT(actionAboutQtSlot()));
    connect(ui->filesFormatComboBox, SIGNAL(activated(int)),
            this, SLOT(fileFormatComboBoxActivated(int)));
}

MainWindow::~MainWindow() {
    delete ui;
    delete model;
}

void MainWindow::openFolderButtonSlot() {
    m_path = QFileDialog::getExistingDirectory(this,
                                                     tr("Open directory"),
                                                    "/home",
                                                     QFileDialog::ShowDirsOnly);
    if (!m_path.isEmpty()) {
        if (model) {
            delete model;
        }
        model = new DuplicateFSModel(m_path);
        ui->treeView->setModel(model);
        QHeaderView* header = ui->treeView->header();
        header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    }
}

void MainWindow::searchDuplicatesButtonSlot() {
    if (!model) {
        QMessageBox msgbox;
        msgbox.setWindowTitle("Error");
        msgbox.setText("Please select a folder to search for duplicates in");
        msgbox.exec();
        return;
    }
    model->findDuplicates();
}

void MainWindow::deleteDuplicatesButtonSlot() {
    if (!model) {
        QMessageBox msgbox;
        msgbox.setWindowTitle("Error");
        msgbox.setText("Please set filter and search "
                       "for duplicates in a selected folder");
        msgbox.exec();
        return;
    }
    int deletedFilesAmount = model->deleteDuplicates();
    QMessageBox msgbox;
    msgbox.setText(QString("Deleted %1 files").arg(deletedFilesAmount));
    msgbox.exec();
}

void MainWindow::actionAboutQtSlot() {
    QMessageBox::aboutQt(this);
}

void MainWindow::fileFormatComboBoxActivated(int index) {
    if (!model) {
        QMessageBox msgbox;
        msgbox.setWindowTitle("Error");
        msgbox.setText("Please select a folder to search for duplicates in");
        msgbox.exec();
        return;
    }
    delete model;
    model = new DuplicateFSModel(m_path,
                                 index);
    ui->treeView->setModel(model);
}
