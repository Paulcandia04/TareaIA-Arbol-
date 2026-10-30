#pragma once

#include "Tableau.hpp"
#include <QWidget>

class QPlainTextEdit;
class QTableWidget;
class QTreeWidget;
class QTreeWidgetItem;
class TreeRenderer;

class Application : public QWidget {
public:
    explicit Application(QWidget* parent = nullptr);

private:
    QPlainTextEdit* input;
    QPlainTextEdit* formulas;
    QPlainTextEdit* results;
    QTableWidget* atoms;
    QTreeWidget* tree;
    TreeRenderer* treeView;

    void analyze();

    void addNode(
        const TableauNode& node,
        QTreeWidgetItem* parent,
        int& counter
    );
};