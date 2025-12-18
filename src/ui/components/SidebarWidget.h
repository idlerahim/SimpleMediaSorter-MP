#pragma once

#include <QListWidget>
#include <QStyledItemDelegate>

class SidebarWidget : public QListWidget {
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget *parent = nullptr);
    void addDestination(const QString& name, const QString& iconPath);

signals:
    void itemClicked(int index);
};

class SidebarDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
