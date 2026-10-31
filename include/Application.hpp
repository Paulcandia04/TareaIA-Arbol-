#pragma once

#include "Tableau.hpp"
#include <QWidget>
#include <QObject> // Necesario para Q_OBJECT

class QPlainTextEdit;
class QTableWidget;
class QTreeWidget;
class QTreeWidgetItem;
class TreeRenderer;

class Application : public QWidget {
    Q_OBJECT // <--- CRUCIAL: Sin esto, el botón "Analizar" no hará nada

public:
    explicit Application(QWidget* parent = nullptr);

private slots:
    void analyze(); // Ahora es un slot para poder conectarlo al botón

private:
    QPlainTextEdit* input;
    QPlainTextEdit* formulas;
    QPlainTextEdit* results;
    QTableWidget* atoms;
    QTreeWidget* tree;
    TreeRenderer* treeView;

    void addNode(
        const TableauNode& node,
        QTreeWidgetItem* parent,
        int& counter
    );
};