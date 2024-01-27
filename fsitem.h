#ifndef FSITEM_H
#define FSITEM_H

#include <QList>

class QVariant;

class FSItem {
public:
    explicit FSItem(const QList<QVariant> &data, const QString &path, FSItem *parentItem = nullptr);
    ~FSItem();

    void appendChild(FSItem *child);

    FSItem *getChild(int row) const;
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int getItemRow() const;
    FSItem *getItemParent() const;
    void generateMD5();
    bool isDir() const;
    QByteArray getHash() const;
    QString path() const;
    bool duplicate() const;
    void setDuplicate(bool duplicate);

private:
    QList<FSItem *> m_childItems;
    QList<QVariant> m_itemData;
    QString m_path;
    QByteArray m_hash;
    bool m_duplicate;
    bool m_isDir;
    FSItem *m_parentItem;
};

#endif // FSITEM_H
