#pragma once

#include "Tableau.hpp"

#include <QGraphicsView>
#include <QPointF>

class QGraphicsScene;
class QShowEvent;
class QWheelEvent;

class TreeRenderer : public QGraphicsView {
public:
    explicit TreeRenderer(QWidget* parent = nullptr);

    void setTree(const TableauNode& root);
    void clearTree();
    void fitTree();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    QGraphicsScene* graphScene;
    bool pendingFit = false;

    QPointF drawNode(
        const TableauNode& node,
        const TableauNode* parent,
        qreal top,
        qreal& cursor
    );
};