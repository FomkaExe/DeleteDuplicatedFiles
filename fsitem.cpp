#include "fsitem.h"

#include <QVariant>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QDebug>

FSItem::FSItem(const QList<QVariant> &data, const QString &path, FSItem *parentItem)
    : m_itemData(data), m_path(path), m_isDir(false), m_parentItem(parentItem) {
    QFileInfo info(path);
    m_isDir = info.isDir();
}

FSItem::~FSItem() {
    qDeleteAll(m_childItems);
}

void FSItem::appendChild(FSItem *child) {
    m_childItems.append(child);
}

FSItem *FSItem::getChild(int row) {
    if (row < 0 || row >= m_childItems.size()) {
        return nullptr;
    }
    return m_childItems.at(row);
}

int FSItem::childCount() const {
    return m_childItems.count();
}

int FSItem::columnCount() const {
    return m_itemData.count();
}

QVariant FSItem::data(int column) const {
    if (column < 0 || column >= m_itemData.size()) {
        return QVariant();
    }
    return m_itemData.at(column);
}

int FSItem::getItemRow() const {
    if (m_parentItem) {
        return m_parentItem->m_childItems.indexOf(const_cast<FSItem *>(this));
    }
    return 0;
}

FSItem *FSItem::getItemParent() {
    return m_parentItem;
}

void FSItem::generateMD5() {
    QFile file(m_path);
    QCryptographicHash MD5hash(QCryptographicHash::Md5);
    QByteArray data;
    QByteArray hash;
    file.open(QIODevice::ReadOnly);
    if (file.isOpen()) {
        data = file.readAll();
        hash = MD5hash.hash(data, QCryptographicHash::Md5);
        m_itemData.append(hash);
    }
}

bool FSItem::isDir() {
    return m_isDir;
}

