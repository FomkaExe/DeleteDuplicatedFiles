#include "duplicatefsmodel.h"

#include <QFile>
#include <QDateTime>
#include <algorithm>

DuplicateFSModel::DuplicateFSModel(const QString& root_path,
                                   int filter,
                                   QObject *parent) :
    QAbstractItemModel{parent},
    m_item_counter(0) {
    setFilter(filter);
    setupModelData(root_path);
    searchDuplicates();
}

DuplicateFSModel::~DuplicateFSModel() {
    delete m_root;
}

QModelIndex DuplicateFSModel::index(int row,
                                    int column,
                                    const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    FSItem *parentItem;

    if (!parent.isValid()) {
        parentItem = m_root;
    } else {
        parentItem = static_cast<FSItem*>(parent.internalPointer());
    }

    FSItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex DuplicateFSModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return QModelIndex();

    FSItem *childItem = static_cast<FSItem*>(index.internalPointer());
    FSItem *parentItem = childItem->parent();

    if (parentItem == m_root)
        return QModelIndex();

    return createIndex(parentItem->itemRow(), 0, parentItem);
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

    if (role != Qt::DisplayRole &&
        role != Qt::BackgroundRole &&
        role != Qt::DecorationRole)
        return QVariant();

    FSItem *item = static_cast<FSItem*>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        return item->data(index.column());
    }

    if (role == Qt::BackgroundRole) {
        if (item->isDuplicate()) {
            if (item->isDir()) {
                return QColor(Qt::GlobalColor::yellow);
            }
            return QColor(Qt::GlobalColor::red);
        } else {
            return QVariant();
        }
    }

    if (role == Qt::DecorationRole && index.column() == 0) {
        switch (item->getFilter()) {
        case FileType::FOLDER:
            return QIcon(":/icons/folder");
            break;
        case FileType::IMAGE:
            return QIcon(":/icons/image");
            break;
        case FileType::DOCUMENT:
            return QIcon(":/icons/document");
            break;
        case FileType::MUSIC:
            return QIcon(":/icons/music");
            break;
        case FileType::VIDEO:
            return QIcon(":/icons/video");
            break;
        case FileType::ALL:
            return QIcon(":/icons/file");
            break;
        }
    }

    return QVariant();
}

QVariant DuplicateFSModel::headerData(int section,
                                      Qt::Orientation orientation,
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

Qt::ItemFlags DuplicateFSModel::flags(const QModelIndex &index) const {
    auto flags = QAbstractItemModel::flags(index);
    return flags;
}

void DuplicateFSModel::searchDuplicates() {
    emit layoutAboutToBeChanged();

    QList<FSItem *> list;
    makeItemsList(list, m_root);
    std::sort(list.begin(), list.end(), [](FSItem *a, FSItem *b)
                                        {
                                            return a->hash() > b->hash();
                                        });
    for (int i = 0; i < list.size() - 1; ++i) {
        if (list[i]->hash() == list[i+1]->hash()) {
            list[i+1]->setDuplicate(true);
            FSItem* parent = list[i+1]->parent();
            while (parent) {
                parent->setDuplicate(true);
                parent = parent->parent();
            }
        }
    }
    emit layoutChanged();
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
        switch (m_filter) {
        case TypeFilter::IMAGES:
            if (!current.fileName().endsWith(".png") &&
                !current.fileName().endsWith(".jpg") &&
                !current.fileName().endsWith(".jpeg") &&
                !current.fileName().endsWith(".gif") &&
                !isDir) {
                continue;
            }
            break;
        case TypeFilter::DOCUMENTS:
            if (!current.fileName().endsWith(".txt") &&
                !current.fileName().endsWith(".doc") &&
                !current.fileName().endsWith(".docx") &&
                !current.fileName().endsWith(".html") &&
                !current.fileName().endsWith(".htm") &&
                !current.fileName().endsWith(".xls") &&
                !current.fileName().endsWith(".xlsx") &&
                !current.fileName().endsWith(".pdf") &&
                !current.fileName().endsWith(".ppt") &&
                !current.fileName().endsWith(".pptx") &&
                !isDir) {
                continue;
            }
            break;
        case TypeFilter::MUSIC:
            if (!current.fileName().endsWith(".mp3") &&
                !current.fileName().endsWith(".wav") &&
                !current.fileName().endsWith(".flac") &&
                !current.fileName().endsWith(".midi") &&
                !isDir) {
                continue;
            }
            break;
        case TypeFilter::VIDEOS:
            if (!current.fileName().endsWith(".mp4") &&
                !current.fileName().endsWith(".avi") &&
                !current.fileName().endsWith(".mov") &&
                !current.fileName().endsWith(".flv") &&
                !isDir) {
                continue;
            }
            break;
        case TypeFilter::ALL:
            break;
        }
        QList<QVariant> childList = {current.baseName()};
        if (!isDir) {
            childList.append(size(current.size()));
        } else {
            childList.append("");
        }
        childList.append(current.birthTime().toString());
        if (isDir) {
            childList.append("");
        }
        FSItem *child = new FSItem(childList,
                                   current.absoluteFilePath(),
                                   parent);
        parent->appendChild(child);
        ++m_item_counter;
        if (current.isDir()) {
            setupItemData(child, current.filePath());
        }
    }
}

void DuplicateFSModel::setupModelData(const QString &root_path) {
    QDir root(root_path);
    m_root = new FSItem(QList<QVariant>({root.dirName(), "Size", "Date"}),
                        root_path);

    setupItemData(m_root, root_path);
}

void DuplicateFSModel::makeItemsList(QList<FSItem *> &list, FSItem *root) {
    for (int i = 0; i < root->childCount(); ++i) {
        FSItem *item = root->child(i);
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
        FSItem *item = root->child(i);
        if (!item->isDir()) {
            if (item->isDuplicate()) {
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

int DuplicateFSModel::getFilter() {
    return static_cast<int>(m_filter);
}

void DuplicateFSModel::setFilter(int filter) {
    switch (filter) {
    case 0:
        m_filter = TypeFilter::ALL;
        break;
    case 1:
        m_filter = TypeFilter::IMAGES;
        break;
    case 2:
        m_filter = TypeFilter::DOCUMENTS;
        break;
    case 3:
        m_filter = TypeFilter::MUSIC;
        break;
    case 4:
        m_filter = TypeFilter::VIDEOS;
        break;
    }
}

bool DuplicateFSModel::isImage(const QModelIndex& index) {
    FSItem *item = static_cast<FSItem*>(index.internalPointer());
    if (item->getFilter() == FileType::IMAGE) {
        return true;
    }
    return false;
}

QString DuplicateFSModel::path(const QModelIndex &index) {
    FSItem *item = static_cast<FSItem*>(index.internalPointer());
    return item->path();
}

QString DuplicateFSModel::size(quint64 size) {
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

uint64_t DuplicateFSModel::count() {
    return m_item_counter;
}
