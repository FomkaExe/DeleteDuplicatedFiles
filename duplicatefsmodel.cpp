#include "duplicatefsmodel.h"

#include <QFile>
#include <QDebug>
#include <QDateTime>

#include <algorithm>

DuplicateFSModel::DuplicateFSModel(const QString& root_path, QObject *parent)
    : QAbstractItemModel{parent}
    , m_item_counter(0) {
    setupModelData(root_path);
    generateChildMD5(m_root);
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

    if (role != Qt::DisplayRole && role != Qt::BackgroundRole)
        return QVariant();

    FSItem *item = static_cast<FSItem*>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        return item->data(index.column());
    } else if (role == Qt::BackgroundRole) {
        if (item->duplicate()) {
            return QColor(Qt::GlobalColor::red);
        } else {
            return QVariant();
        }
    }

    return QVariant();
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

void DuplicateFSModel::generateChildMD5(FSItem *parent) {
    for (int i = 0; i < parent->childCount(); ++i) {
        FSItem *item = parent->getChild(i);
        if (!item->isDir()) {
            item->generateMD5();
        } else {
            generateChildMD5(item);
        }
    }
}

uint64_t DuplicateFSModel::getItemCount() {
    return m_item_counter;
}

void DuplicateFSModel::findDuplicates() {
    layoutAboutToBeChanged();

    QList<FSItem *> list;
    makeItemsList(list, m_root);
    std::sort(list.begin(), list.end(), [](FSItem *a, FSItem *b)
                                        {
                                            return a->getHash() > b->getHash();
                                        });
    for (int i = 0; i < list.size() - 1; ++i) {
        if (list[i]->getHash() == list[i+1]->getHash()) {
            list[i+1]->setDuplicate(true);
        }
    }
    layoutChanged();
}

void DuplicateFSModel::setupItemData(FSItem *parent, const QString &path) {
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
        childList.append(current.birthTime().toString());
        if (isDir) {
            childList.append("");
        }
        FSItem *child = new FSItem(childList, current.absoluteFilePath(), parent);
        parent->appendChild(child);
        ++m_item_counter;
        if (current.isDir()) {
            setupItemData(child, current.filePath());
        }
    }
}

void DuplicateFSModel::setupModelData(const QString &root_path) {
    QDir root(root_path);
    m_root = new FSItem(QList<QVariant>({root.dirName(), "Size", "Date"}), root_path);

    setupItemData(m_root, root_path);
}

void DuplicateFSModel::makeItemsList(QList<FSItem *> &list, FSItem *root)
{
    for (int i = 0; i < root->childCount(); ++i) {
        FSItem *item = root->getChild(i);
        if (!item->isDir()) {
            list.append(item);
        } else {
            makeItemsList(list, item);
        }
    }
}

int DuplicateFSModel::deleteDuplicates(FSItem *root) {
    if (!root) {
        root = m_root;
    }
    int deleted = 0;
    int childCount = root->childCount();
    for (int i = 0; i < childCount; ++i) {
        FSItem *item = root->getChild(i);
        if (!item->isDir()) {
            if (item->duplicate()) {
                emit layoutAboutToBeChanged();
                deleted++;
                root->deleteChild(i);
                QFile::remove(item->path());
                delete item;
                childCount--;
                i--;
                emit layoutChanged();
            }
        } else {
            deleted += deleteDuplicates(item);
        }
    }
    return deleted;
}
