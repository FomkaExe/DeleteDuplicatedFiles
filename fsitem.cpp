#include "fsitem.h"

#include <QVariant>
#include <QCryptographicHash>
#include <QFileInfo>

FSItem::FSItem(const QList<QVariant> &data,
               const QString &path,
               FSItem *parentItem)
    : m_itemData(data)
    , m_path(path)
    , m_hash(nullptr)
    , m_duplicate(false)
    , m_isDir(false)
    , m_parentItem(parentItem)
    , m_fileType(FileType::ALL) {
    QFileInfo info(path);
    m_isDir = info.isDir();
    if (info.isDir()) {
        m_fileType = FileType::FOLDER;
    }
    if (info.suffix() == "png" ||
        info.suffix() == "jpg" ||
        info.suffix() == "jpeg" ||
        info.suffix() == "gif") {
        m_fileType = FileType::IMAGE;
    }
    if (info.suffix() == "txt" ||
        info.suffix() == "doc" ||
        info.suffix() == "docx" ||
        info.suffix() == "html" ||
        info.suffix() == "htm" ||
        info.suffix() == "xls" ||
        info.suffix() == "xlsx" ||
        info.suffix() == "pdf" ||
        info.suffix() == "ppt" ||
        info.suffix() == "pptx") {
        m_fileType = FileType::DOCUMENT;
    }
    if (info.suffix() == "mp3" ||
        info.suffix() == "wav" ||
        info.suffix() == "flac" ||
        info.suffix() == "midi") {
        m_fileType = FileType::MUSIC;
    }
    if (info.suffix() == "mp4" ||
        info.suffix() == "avi" ||
        info.suffix() == "mov" ||
        info.suffix() == "flv") {
        m_fileType = FileType::VIDEO;
    }
}

FSItem::~FSItem() {
    qDeleteAll(m_childItems);
}

void FSItem::appendChild(FSItem *child) {
    m_childItems.append(child);
}

void FSItem::deleteChild(int index) {
    m_childItems.removeAt(index);
}

FSItem *FSItem::getChild(int row) const{
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

FSItem *FSItem::getItemParent() const {
    return m_parentItem;
}

void FSItem::generateMD5() {
    QFile file(m_path);
    QCryptographicHash MD5hash(QCryptographicHash::Md5);
    QByteArray data;
    file.open(QIODevice::ReadOnly);
    if (file.isOpen()) {
        data = file.readAll();
        m_hash = MD5hash.hash(data, QCryptographicHash::Md5);
    }
}

bool FSItem::isDir() const {
    return m_isDir;
}

QByteArray FSItem::getHash() const {
    return m_hash;
}

QString FSItem::path() const {
    return m_path;
}

bool FSItem::duplicate() const {
    return m_duplicate;
}

void FSItem::setDuplicate(bool duplicate) {
    m_duplicate = duplicate;
}

FileType FSItem::getFilter() {
    return m_fileType;
}

