#ifndef FSITEM_H
#define FSITEM_H

#include <QList>

class QVariant;

class FSItem {
public:
    explicit FSItem(const QList<QVariant> &data, const QString &path, FSItem *parentItem = nullptr);
    ~FSItem();

    void appendChild(FSItem *child);

    FSItem *getChild(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int getItemRow() const;
    FSItem *getItemParent();

    void generateMD5();
    bool isDir();

private:
    QList<FSItem *> m_childItems;
    QList<QVariant> m_itemData;
    QString m_path;
    bool m_isDir;
    FSItem *m_parentItem;
};

#endif // FSITEM_H
