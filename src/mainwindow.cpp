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
#include <QDesktopServices>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_model(nullptr),
    m_contextMenu(new QMenu()),
    m_contextOpen(new QAction("Open")),
    m_contextDelete(new QAction("Delete")),
    m_fileMenu(new QMenu("File")),
    m_filterMenu(new QMenu("Filter")),
    m_aboutMenu(new QMenu("About")),
    m_actionOpen(new QAction("Open")),
    m_actionDeleteDuplicates(new QAction("Delete Duplicates")),
    m_actionExit(new QAction("Exit")),
    m_actionAboutQt(new QAction("About Qt")),
    m_filterALL(new QAction("All")),
    m_filterIMAGES(new QAction("Images")),
    m_filterDOCUMENTS(new QAction("Documents")),
    m_filterMUSIC(new QAction("Music")),
    m_filterVIDEOS(new QAction("Videos")) {
    /* mainwindow init */
    m_ui->setupUi(this);
    this->setWindowTitle("Delete duplicated files");

    m_ui->labelDir->setText("");

    /* Combo Box init */
    QStringList comboBoxItemsList = { "All", "Images", "Documents",
                                     "Music", "Videos" };
    m_ui->filesFormatComboBox->addItems(comboBoxItemsList);

    /* Context menu init */
    m_contextMenu->addAction(m_contextOpen);
    m_contextMenu->addAction(m_contextDelete);
    connect(m_contextOpen, SIGNAL(triggered()),
            this, SLOT(actionContextOpenTriggered()));
    connect(m_contextDelete, SIGNAL(triggered()),
            this, SLOT(actionContextDeleteTriggered()));

    /* Menu bar init */
    m_ui->menubar->addMenu(m_fileMenu);
    m_fileMenu->addAction(m_actionOpen);
    m_fileMenu->addMenu(m_filterMenu);
    m_filterMenu->addAction(m_filterALL);
    m_filterMenu->addAction(m_filterIMAGES);
    m_filterMenu->addAction(m_filterDOCUMENTS);
    m_filterMenu->addAction(m_filterMUSIC);
    m_filterMenu->addAction(m_filterVIDEOS);
    m_fileMenu->addAction(m_actionDeleteDuplicates);
    m_fileMenu->addAction(m_actionExit);
    m_ui->menubar->addMenu(m_aboutMenu);
    m_aboutMenu->addAction(m_actionAboutQt);


    connect(m_actionOpen, &QAction::triggered,
            this, &MainWindow::openFolderButtonSlot);
    connect(m_ui->openButton, &QPushButton::clicked,
            this, &MainWindow::openFolderButtonSlot);

    connect(m_filterALL, &QAction::triggered,
            this, [this]() {refreshModel(TypeFilter::ALL);});
    connect(m_filterIMAGES, &QAction::triggered,
            this, [this]() {refreshModel(TypeFilter::IMAGES);});
    connect(m_filterDOCUMENTS, &QAction::triggered,
            this, [this]() {refreshModel(TypeFilter::DOCUMENTS);});
    connect(m_filterMUSIC, &QAction::triggered,
            this, [this]() {refreshModel(TypeFilter::MUSIC);});
    connect(m_filterVIDEOS, &QAction::triggered,
            this, [this]() {refreshModel(TypeFilter::VIDEOS);});
    connect(m_ui->filesFormatComboBox, &QComboBox::activated,
            this, [=](int sig) {refreshModel(static_cast<TypeFilter>(sig));});

    connect(m_actionDeleteDuplicates, &QAction::triggered,
            this, &MainWindow::deleteDuplicatesButtonSlot);
    connect(m_ui->deleteButton, &QPushButton::clicked,
            this, &MainWindow::deleteDuplicatesButtonSlot);
    connect(m_actionExit, &QAction::triggered,
            this, []() {QCoreApplication::quit();});
    connect(m_actionAboutQt, &QAction::triggered,
            this, &MainWindow::actionAboutQtSlot);
}

MainWindow::~MainWindow() {
    delete m_filterVIDEOS;
    delete m_filterMUSIC;
    delete m_filterDOCUMENTS;
    delete m_filterIMAGES;
    delete m_filterALL;
    delete m_actionAboutQt;
    delete m_actionExit;
    delete m_actionDeleteDuplicates;
    delete m_actionOpen;
    delete m_aboutMenu;
    delete m_filterMenu;
    delete m_fileMenu;
    delete m_contextDelete;
    delete m_contextOpen;
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
        connect(m_ui->treeView, &QWidget::customContextMenuRequested,
                this, &MainWindow::onContextMenuRequest);

        QHeaderView* header = m_ui->treeView->header();
        header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        header->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        m_ui->labelDir->setText(m_path);
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

void MainWindow::refreshModel(TypeFilter filter) {
    if (!m_model) {
        return;
    }
    delete m_model;
    m_model = new DuplicateFSModel(m_path,
                                   filter);
    m_ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->treeView, &QWidget::customContextMenuRequested,
            this, &MainWindow::onContextMenuRequest);
    m_ui->treeView->setModel(m_model);

    if (m_ui->filesFormatComboBox->currentIndex() != static_cast<int>(filter)) {
        m_ui->filesFormatComboBox->setCurrentIndex(static_cast<int>(filter));
    }
}

void MainWindow::onContextMenuRequest(const QPoint& point) {
    QModelIndex index = m_ui->treeView->indexAt(point);
    if (index.isValid()) {
        m_contextMenu->exec(m_ui->treeView->viewport()->mapToGlobal(point));
    }
}

void MainWindow::actionContextOpenTriggered() {
    QModelIndex index = m_ui->treeView->currentIndex();
    QString path = m_model->path(index);
    QUrl fileUrl = QUrl::fromLocalFile(path);

    if (!QDesktopServices::openUrl(fileUrl)) {
        this->openUrl(path);
    }
}

void MainWindow::actionContextDeleteTriggered() {
    QModelIndex index = m_ui->treeView->currentIndex();
    QString path = m_model->path(index);
    QFile file(path);
    if (!file.moveToTrash()) {
        qDebug() << "QFile::moveToTrash() failed";
        qDebug() << "Trying QFile::remove()...";
        if (!file.remove()) {
            qDebug() << "QFile::remove() failed";
            return;
        }
    }
    refreshModel(m_model->getFilter());
}

void MainWindow::openUrl(const QString& url) {
    QString command;
#ifdef Q_OS_WIN
    command = "start " + url;
#elif defined(Q_OS_MACOS)
    command = "open " + url;
#elif defined(Q_OS_LINUX)
    command = "xdg-open " + url;
#else
    qDebug() << "Unsupported operating system";
    return;
#endif

    QProcess::startDetached(command);
}
