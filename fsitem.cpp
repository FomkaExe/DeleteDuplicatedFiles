#include "fsitem.h"

#include <QVariant>

FSItem::FSItem(const QList<QVariant> &data, FSItem *parentItem)
    : m_itemData(data), m_parentItem(parentItem) {}

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

