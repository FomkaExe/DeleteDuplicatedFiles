#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QFileSystemModel;
class QTreeView;
class DuplicateFSModel;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openFolderButtonSlot();
    void deleteDuplicatesButtonSlot();
    void actionAboutQtSlot();
    void refreshModel(int index);
    void onContextMenuRequest(const QPoint &point);
    void actionContextOpenTriggered();
    void actionContextDeleteTriggered();
    void openUrl(const QString& url);

private:
    Ui::MainWindow *m_ui;
    DuplicateFSModel *m_model;
    QString m_path;

    QMenu *m_contextMenu;
    QAction *m_contextOpen;
    QAction *m_contextDelete;

    QMenu *m_fileMenu;
    QMenu *m_filterMenu;
    QMenu *m_aboutMenu;

    QAction *m_actionOpen;
    QAction *m_actionDeleteDuplicates;
    QAction *m_actionExit;
    QAction *m_actionAboutQt;

    QAction *m_filterALL;
    QAction *m_filterIMAGES;
    QAction *m_filterDOCUMENTS;
    QAction *m_filterMUSIC;
    QAction *m_filterVIDEOS;
};
#endif // MAINWINDOW_H
