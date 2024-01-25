#ifndef FSITEM_H
#define FSITEM_H

#include <QList>

class QVariant;

class FSItem {
public:
    explicit FSItem(const QList<QVariant> &data, FSItem *parentItem = nullptr);
    ~FSItem();

    void appendChild(FSItem *child);

    FSItem *getChild(int row);
    int childCount() const;
    int columnCount() const;
    QVariant getColumnData(int column) const;
    int getItemRow() const;
    FSItem *getItemParent();

private:
    QList<FSItem *> m_childItems;
    QList<QVariant> m_itemData;
    FSItem *m_parentItem;
};

#endif // FSITEM_H
