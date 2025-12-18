#include "SidebarWidget.h"
#include <QPainter>
#include <QIcon>

SidebarWidget::SidebarWidget(QWidget *parent) : QListWidget(parent) {
    setItemDelegate(new SidebarDelegate(this));
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet("QListWidget { background-color: #2b2b2b; color: #ffffff; outline: 0; }"
                  "QListWidget::item { padding: 10px; }"
                  "QListWidget::item:selected { background-color: #3d3d3d; border-left: 4px solid #3498db; }");
    
    connect(this, &QListWidget::currentRowChanged, this, &SidebarWidget::itemClicked);
}

void SidebarWidget::addDestination(const QString& name, const QString& iconPath) {
    QListWidgetItem* item = new QListWidgetItem(this);
    item->setText(name);
    // In a real app we'd load the icon, for now just text or generic
    // item->setIcon(QIcon(iconPath)); 
    item->setData(Qt::UserRole, iconPath); // Store icon name/path
    addItem(item);
}

void SidebarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);
    
    // Custom painting can be added here if needed, e.g. drawing the icon manually
    // For now the stylesheet handles most of the look
}

QSize SidebarDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QSize s = QStyledItemDelegate::sizeHint(option, index);
    s.setHeight(50); // Fixed height for sidebar items
    return s;
}
