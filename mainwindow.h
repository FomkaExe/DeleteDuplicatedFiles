﻿#ifndef MAINWINDOW_H
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
    void searchDuplicatesButtonSlot();
    void deleteDuplicatesButtonSlot();
    void actionAboutQtSlot();

private:
    Ui::MainWindow *ui;
    DuplicateFSModel *model;
};
#endif // MAINWINDOW_H
