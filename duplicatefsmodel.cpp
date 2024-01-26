#include "duplicatefsmodel.h"

#include <QFile>
#include <QDebug>

DuplicateFSModel::DuplicateFSModel(const QString& root_path, QObject *parent)
    : QAbstractItemModel{parent} {
    setupModelData(root_path);
}

DuplicateFSModel::~DuplicateFSModel() {
    delete m_root;
}

QModelIndex DuplicateFSModel::index(int row, int column, const QModelIndex &parent) const {

    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    FSItem *parentItem;

    if (!parent.isValid()) {
        parentItem = m_root;
    } else {
        parentItem = static_cast<FSItem*>(parent.internalPointer());
    }

    FSItem *childItem = parentItem->getChild(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex DuplicateFSModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    FSItem *childItem = static_cast<FSItem*>(index.internalPointer());
    FSItem *parentItem = childItem->getItemParent();

    if (parentItem == m_root)
        return QModelIndex();

    return createIndex(parentItem->getItemRow(), 0, parentItem);
}

int DuplicateFSModel::rowCount(const QModelIndex &parent) const {
    FSItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_root;
    else
        parentItem = static_cast<FSItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int DuplicateFSModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<FSItem*>(parent.internalPointer())->columnCount();
    return m_root->columnCount();
}

QVariant DuplicateFSModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    FSItem *item = static_cast<FSItem*>(index.internalPointer());

    return item->data(index.column());
}

QVariant DuplicateFSModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return QString("Name");
            break;
        case 1:
            return QString("Size");
            break;
        case 2:
            return QString("MD5");
            break;
        case 3:
            return QString("Created");
            break;
        }
    }
    return QVariant();
}

QString DuplicateFSModel::getSize(quint64 size) {
    QString newSize;
    if (size > 1000000000) {
        newSize.append(QString::number(size/1000000000));
        size %= 1000000000;
        newSize.append(",");
        newSize.append(QString::number(size/100000000));
        newSize.append(" GB");
        return newSize;
    } else if (size > 1000000) {
        newSize.append(QString::number(size/1000000));
        newSize.append(",");
        size %= 1000000;
        newSize.append(QString::number(size/100000));
        newSize.append(" MB");
    } else if (size > 1000) {
        newSize.append(QString::number(size/1000));
        newSize.append(",");
        size %= 1000;
        newSize.append(QString::number(size/100));
        newSize.append(" KB");
    } else {
        newSize.append(QString::number(size));
        newSize.append(" B");
    }
    return newSize;
}

void DuplicateFSModel::setupItemData(FSItem *parent, const QString &path)
{
    QDir root(path);
    QFileInfoList entryList = root.entryInfoList();
    for (int i = 0; i < entryList.size(); ++i) {
        QFileInfo current = entryList.at(i);
        if (current.fileName() == "." ||
            current.fileName() == "..") {
            continue;
        }
        bool isDir = current.isDir();
        QList<QVariant> childList = {current.baseName()};
        if (!isDir) {
            childList.append(getSize(current.size()));
        } else {
            childList.append("");
        }
        FSItem *child = new FSItem(childList, parent);
        parent->appendChild(child);
        if (current.isDir()) {
            setupItemData(child, current.filePath());
        }
    }
}

void DuplicateFSModel::setupModelData(const QString &root_path)
{
    QDir root(root_path);
    m_root = new FSItem(QList<QVariant>({root.dirName(), "Size"}));

    setupItemData(m_root, root_path);
}
