#include "TreeRenderer.hpp"
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QPen>
#include <QShowEvent>
#include <QStringList>
#include <QWheelEvent>

#include <vector>

namespace {

QString toQt(const std::string& text) {
    return QString::fromUtf8(text.c_str());
}

} 

TreeRenderer::TreeRenderer(QWidget* parent)
    : QGraphicsView(parent),
      graphScene(new QGraphicsScene(this)) {
    setScene(graphScene);

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);

    setBackgroundBrush(QBrush(QColor("#111827")));
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setToolTip(
        QString::fromUtf8(
            u8"Rueda: zoom · Arrastrar: mover el árbol"
        )
    );
}

void TreeRenderer::clearTree() {
    graphScene->clear();
    graphScene->setSceneRect(QRectF());
    resetTransform();
    pendingFit = false;
}

void TreeRenderer::setTree(const TableauNode& root) {
    clearTree();

    qreal cursor = 0;
    drawNode(root, nullptr, 0, cursor);

    graphScene->setSceneRect(
        graphScene->itemsBoundingRect().adjusted(-30, -30, 30, 30)
    );

    pendingFit = true;
    fitTree();
}

void TreeRenderer::fitTree() {
    if (graphScene->items().isEmpty()) {
        return;
    }

    if (!isVisible()) {
        pendingFit = true;
        return;
    }

    resetTransform();
    fitInView(graphScene->sceneRect(), Qt::KeepAspectRatio);

    if (transform().m11() > 1.0) {
        resetTransform();
    }

    centerOn(graphScene->sceneRect().center());
    pendingFit = false;
}

void TreeRenderer::showEvent(QShowEvent* event) {
    QGraphicsView::showEvent(event);

    if (pendingFit) {
        fitTree();
    }
}

void TreeRenderer::wheelEvent(QWheelEvent* event) {
    if (graphScene->items().isEmpty()) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    const int delta = event->angleDelta().y();

    if (delta == 0) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    const qreal factor = delta > 0 ? 1.15 : 1.0 / 1.15;
    const qreal nextScale = transform().m11() * factor;

    if (nextScale >= 0.02 && nextScale <= 4.0) {
        scale(factor, factor);
    }

    event->accept();
}

QPointF TreeRenderer::drawNode(
    const TableauNode& node,
    const TableauNode* parent,
    qreal top,
    qreal& cursor
) {
    constexpr qreal width = 300;
    constexpr qreal padding = 12;
    constexpr qreal columnSpacing = 350;
    constexpr qreal verticalSpacing = 65;

    QStringList lines;

    if (!parent) {
        lines << "BASE DE CONOCIMIENTO";
    }

    for (std::size_t i = node.inheritedCount;
         i < node.entries.size();
         ++i) {
        const auto& entry = node.entries[i];

        QString text = toQt(entry.formula->toString());

        if (entry.applied) {
            text += QString::fromUtf8(u8"  ✓");
        }

        lines << text;
    }

    if (parent) {
        for (std::size_t i = 0;
             i < node.inheritedCount;
             ++i) {
            if (node.entries[i].applied &&
                !parent->entries[i].applied) {
                lines << QString::fromUtf8(u8"✓ Se aplica: ")
                    + toQt(node.entries[i].formula->toString());
            }
        }
    }

    QColor border("#64748b");
    QColor background("#1e293b");

    if (node.closed) {
        lines << QString::fromUtf8(u8"✕ RAMA CERRADA");
        lines << toQt(node.contradiction);

        border = QColor("#f87171");
        background = QColor("#451a1a");
    } else if (node.children.empty()) {
        lines << QString::fromUtf8(u8"✓ RAMA ABIERTA");

        border = QColor("#4ade80");
        background = QColor("#143322");
    }

    auto* textItem = graphScene->addText(
        lines.join("\n"),
        QFont("Segoe UI", 11)
    );

    textItem->setDefaultTextColor(QColor("#f8fafc"));
    textItem->setTextWidth(width - 2 * padding);
    textItem->setZValue(1);

    const qreal height =
        textItem->boundingRect().height() + 2 * padding;

    std::vector<QPointF> childPositions;

    for (const auto& child : node.children) {
        childPositions.push_back(
            drawNode(
                *child,
                &node,
                top + height + verticalSpacing,
                cursor
            )
        );
    }

    qreal center;

    if (childPositions.empty()) {
        center = cursor + width / 2;
        cursor += columnSpacing;
    } else {
        center = (
            childPositions.front().x() +
            childPositions.back().x()
        ) / 2;
    }

    const QRectF rectangle(
        center - width / 2,
        top,
        width,
        height
    );

    auto* box = graphScene->addRect(
        rectangle,
        QPen(border, 2),
        QBrush(background)
    );

    box->setZValue(0);

    textItem->setPos(
        rectangle.left() + padding,
        rectangle.top() + padding
    );

    for (const auto& childPosition : childPositions) {
        auto* connection = graphScene->addLine(
            QLineF(
                QPointF(center, rectangle.bottom()),
                childPosition
            ),
            QPen(QColor("#94a3b8"), 2)
        );

        connection->setZValue(-1);
    }

    return QPointF(center, top);
}