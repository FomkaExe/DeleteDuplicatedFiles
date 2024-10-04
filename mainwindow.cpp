#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duplicatefsmodel.h"

#include <QFileSystemModel>
#include <QFileDialog>
#include <QTreeView>
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_model(nullptr),
    m_contextMenu(new QMenu()),
    m_actionOpen(new QAction("Open")) {
    m_ui->setupUi(this);
    this->setWindowTitle("Delete duplicated files");

    m_contextMenu->addAction(m_actionOpen);

    QStringList comboBoxItemsList = { "All", "Images", "Documents",
                                      "Music", "Videos" };
    m_ui->filesFormatComboBox->addItems(comboBoxItemsList);

    connect(m_actionOpen, SIGNAL(triggered()),
            this, SLOT(menuActionOpen()));

    connect(m_ui->openButton, SIGNAL(clicked()),
            this, SLOT(openFolderButtonSlot()));

    connect(m_ui->deleteButton, SIGNAL(clicked()),
            this, SLOT(deleteDuplicatesButtonSlot()));

    connect(m_ui->actionAbout_Qt, SIGNAL(triggered()),
            this, SLOT(actionAboutQtSlot()));

    connect(m_ui->filesFormatComboBox, SIGNAL(activated(int)),
            this, SLOT(fileFormatComboBoxActivated(int)));
}

MainWindow::~MainWindow() {
    delete m_actionOpen;
    delete m_contextMenu;
    delete m_model;
    delete m_ui;
}

void MainWindow::openFolderButtonSlot() {
    m_path = QFileDialog::getExistingDirectory(this,
                                               tr("Open directory"),
                                               "/home",
                                               QFileDialog::ShowDirsOnly);
    if (!m_path.isEmpty()) {
        if (m_model) {
            delete m_model;
        }
        setEnabled(false);
        QApplication::setOverrideCursor(Qt::WaitCursor);

        m_model = new DuplicateFSModel(m_path);

        m_ui->treeView->setModel(m_model);
        m_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
                this, SLOT(onContextMenuRequest(const QPoint &)));

        QHeaderView* header = m_ui->treeView->header();
        header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    }
    QApplication::restoreOverrideCursor();
    setEnabled(true);
}

void MainWindow::deleteDuplicatesButtonSlot() {
    if (!m_model) {
        return;
    }
    auto choice =
            QMessageBox::warning(this,
                                 "Confirm",
                                 "Are you sure you want to delete duplicate files?",
                                 QMessageBox::Ok | QMessageBox::Cancel);
    if (choice == QMessageBox::Ok) {
        int deletedFilesAmount = m_model->deleteDuplicates();
        QMessageBox::information(this,
                                 "Success",
                                 QString("Deleted %1 files").arg(deletedFilesAmount));
    }
}

void MainWindow::actionAboutQtSlot() {
    QMessageBox::aboutQt(this);
}

void MainWindow::fileFormatComboBoxActivated(int index) {
    if (!m_model) {
        return;
    }
    delete m_model;
    m_model = new DuplicateFSModel(m_path,
                                   index);
    m_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(onContextMenuRequest(const QPoint &)));

    m_ui->treeView->setModel(m_model);

}

void MainWindow::onContextMenuRequest(const QPoint& point) {
    QModelIndex index = m_ui->treeView->indexAt(point);
    if (index.isValid() && m_model->isImage(index)) {
        m_contextMenu->exec(m_ui->treeView->viewport()->mapToGlobal(point));
    }
}

void MainWindow::menuActionOpen() {
    QModelIndex index = m_ui->treeView->currentIndex();
    QString path = m_model->path(index);
    QImage img(path);
    if (img.size().width() > size().width()) {
        img = img.scaledToWidth(size().width() - 50);
    }
    if (img.size().height() > size().height()) {
        img = img.scaledToHeight(size().height() - 50);
    }

    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->addPixmap(QPixmap::fromImage(img));
    QGraphicsView *view = new QGraphicsView(scene);
    view->setWindowTitle(path);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->resize(img.size().width() + 5, img.size().height() + 5);
    view->show();
}
