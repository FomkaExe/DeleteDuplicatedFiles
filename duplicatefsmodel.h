#ifndef DUPLICATEFILESYSTEMMODEL_H
#define DUPLICATEFILESYSTEMMODEL_H
#include "fsitem.h"
#include <QFileSystemModel>
#include <QObject>

enum class TypeFilter {
    ALL,
    IMAGES,
    DOCUMENTS,
    MUSIC,
    VIDEOS
};

class DuplicateFSModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit DuplicateFSModel(const QString& root_path,
                              int filter = 0,
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
    QString getSize(quint64 size);
    void generateChildMD5(FSItem *parent);
    uint64_t getItemCount();
    void findDuplicates();
    int deleteDuplicates(FSItem *root = nullptr);
    void setFilter(int filter);

private:
    void setupItemData(FSItem* parent, const QString& path);
    void setupModelData(const QString& root_path);
    void makeItemsList(QList<FSItem *> &list, FSItem *root);
    FSItem* m_root;
    uint64_t m_item_counter;
    TypeFilter m_filter;
};

#endif // DUPLICATEFILESYSTEMMODEL_H
