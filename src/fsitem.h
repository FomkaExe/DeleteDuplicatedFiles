#ifndef FSITEM_H
#define FSITEM_H

#include <QList>

class QVariant;

enum class FileType {
    ALL,
    IMAGE,
    DOCUMENT,
    MUSIC,
    VIDEO,
    FOLDER
};

class FSItem {
public:
    explicit FSItem(const QList<QVariant> &data,
                    const QString &path,
                    FSItem *parentItem = nullptr);
    ~FSItem();

    void appendChild(FSItem *child);
    void deleteChild(int index);

    FSItem *child(int row) const;
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int itemRow() const;
    FSItem *parent() const;
    void generateMD5();
    bool isDir() const;
    QByteArray hash() const;
    QString path() const;
    bool isDuplicate() const;
    void setDuplicate(bool duplicate);
    FileType getFilter();

private:
    QList<FSItem *> m_childItems;
    QList<QVariant> m_itemData;
    QString m_path;
    QByteArray m_hash;
    bool m_duplicate;
    bool m_isDir;
    FSItem *m_parentItem;
    FileType m_fileType;
};

#endif // FSITEM_H
