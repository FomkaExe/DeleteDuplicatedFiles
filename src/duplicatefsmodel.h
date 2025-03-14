#ifndef DUPLICATEFILESYSTEMMODEL_H
#define DUPLICATEFILESYSTEMMODEL_H

#include "fsitem.h"
#include "FileType.h"

#include <QFileSystemModel>
#include <QObject>

class DuplicateFSModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit DuplicateFSModel(const QString& root_path,
                              TypeFilter filter = TypeFilter::ALL,
                              QObject *parent = nullptr);
    ~DuplicateFSModel();
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& index) const;
    int rowCount(const QModelIndex& index) const;
    int columnCount(const QModelIndex& index) const;
    QVariant data(const QModelIndex& index, int column) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;

public:
    void searchDuplicates();
    int deleteDuplicates(FSItem *root = nullptr);
    TypeFilter getFilter();
    void setFilter(TypeFilter filter);
    bool isImage(const QModelIndex &index);
    QString path(const QModelIndex &index);

private:
    void setupItemData(FSItem* parent, const QString& path);
    void setupModelData(const QString& root_path);
    void makeItemsList(QList<FSItem *> &list, FSItem *root);
    QString size(quint64 size);
    uint64_t count();


private:
    FSItem* m_root;
    uint64_t m_item_counter;
    TypeFilter m_filter;
};

#endif // DUPLICATEFILESYSTEMMODEL_H
