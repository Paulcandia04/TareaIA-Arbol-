#include "Application.hpp"
#include "Parser.hpp"
#include "TreeRenderer.hpp"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QHeaderView>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStringList>
#include <QTableWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <exception>

namespace {

QString toQt(const std::string& text) {
    return QString::fromUtf8(text.c_str());
}

} 

Application::Application(QWidget* parent)
    : QWidget(parent) {
    setWindowTitle("TareaIA - Arbol semantico");
    resize(1100, 800);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(
        new QLabel(
            QString::fromUtf8(
                u8"Base de conocimiento: escribe una regla por línea"
            ),
            this
        )
    );

    input = new QPlainTextEdit(this);
    input->setMinimumHeight(140);

    input->setPlainText(
        QString::fromUtf8(
            u8"Si es de día, entonces voy a trabajar\n"
            u8"Si llueve, entonces no voy a trabajar\n"
            u8"Es de día"
        )
    );

    layout->addWidget(input);

    auto* button = new QPushButton("Analizar", this);
    layout->addWidget(button);

    auto* tabs = new QTabWidget(this);
    treeView = new TreeRenderer(tabs);

    atoms = new QTableWidget(0, 2, tabs);
    atoms->setHorizontalHeaderLabels(
        QStringList{"Letra", "Significado"}
    );
    atoms->horizontalHeader()->setStretchLastSection(true);
    atoms->setEditTriggers(QAbstractItemView::NoEditTriggers);

    formulas = new QPlainTextEdit(tabs);
    formulas->setReadOnly(true);

    tree = new QTreeWidget(tabs);
    tree->setHeaderLabels(
        QStringList{"Desarrollo", "Estado"}
    );
    tree->header()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents
    );
    tree->setAlternatingRowColors(true);

    results = new QPlainTextEdit(tabs);
    results->setReadOnly(true);

    tabs->addTab(atoms, QString::fromUtf8(u8"Átomos"));
    tabs->addTab(formulas, QString::fromUtf8(u8"Fórmulas"));
    tabs->addTab(treeView, QString::fromUtf8(u8"Árbol gráfico"));
    tabs->addTab(tree, "Detalle de ramas");
    tabs->addTab(results, "Resultados");

    layout->addWidget(tabs, 1);

    connect(
        button,
        &QPushButton::clicked,
        this,
        [this, tabs]() {
            analyze();
            tabs->setCurrentWidget(results);
        }
    );
}

void Application::analyze() {
    atoms->setRowCount(0);
    formulas->clear();
    tree->clear();
    treeView->clearTree();
    results->clear();

    try {
        AtomRegistry registry;
        Parser parser(registry);
        Tableau tableau;
        FormulaList knowledgeBase;
        QStringList ruleDescriptions;

        const QStringList lines = input->toPlainText().split('\n');

        for (qsizetype i = 0; i < lines.size(); ++i) {
            const QString line = lines[i].trimmed();

            if (line.isEmpty()) {
                continue;
            }

            Formula::Ptr formula;

            try {
                formula = parser.parse(
                    line.toUtf8().toStdString()
                );
            } catch (const std::exception& error) {
                results->setPlainText(
                    QString::fromUtf8(u8"Error en la línea %1:\n%2")
                        .arg(static_cast<int>(i + 1))
                        .arg(toQt(error.what()))
                );
                return;
            }

            knowledgeBase.push_back(formula);

            ruleDescriptions << QString("R%1: %2\n    %3")
                .arg(static_cast<int>(knowledgeBase.size()))
                .arg(line)
                .arg(toQt(formula->toString()));
        }

        if (knowledgeBase.empty()) {
            results->setPlainText("Escribe al menos una regla.");
            return;
        }

        auto result = tableau.build(knowledgeBase);

        formulas->setPlainText(ruleDescriptions.join("\n\n"));

        for (const auto& atom : registry.entries()) {
            const int row = atoms->rowCount();

            atoms->insertRow(row);

            atoms->setItem(
                row, 0,
                new QTableWidgetItem(toQt(atom.symbol))
            );

            atoms->setItem(
                row, 1,
                new QTableWidgetItem(toQt(atom.meaning))
            );
        }

        int counter = 0;
        addNode(*result.root, nullptr, counter);
        tree->expandAll();
        treeView->setTree(*result.root);

        QStringList output;

        output << (
            result.isSatisfiable()
                ? "Base de conocimiento SATISFACIBLE."
                : "Base de conocimiento INSATISFACIBLE."
        );

        output << QString("Ramas cerradas: %1")
            .arg(static_cast<qulonglong>(result.closedBranches));

        output << QString("Ramas abiertas: %1")
            .arg(static_cast<qulonglong>(result.models.size()));

        for (std::size_t i = 0; i < result.models.size(); ++i) {
            const auto& model = result.models[i];

            output << "";
            output << QString("Rama abierta %1:")
                .arg(static_cast<qulonglong>(i + 1));

            for (const auto& atom : registry.entries()) {
                const auto assignment = model.find(atom.symbol);

                QString value;

                if (assignment == model.end()) {
                    value = "libre (V o F)";
                } else {
                    value = assignment->second ? "V" : "F";
                }

                output << QString("  %1 = %2")
                    .arg(toQt(atom.symbol))
                    .arg(value);
            }
        }

        results->setPlainText(output.join("\n"));
    } catch (const std::exception& error) {
        results->setPlainText(
            QString("Error: %1").arg(toQt(error.what()))
        );
    }
}

void Application::addNode(
    const TableauNode& node,
    QTreeWidgetItem* parent,
    int& counter
) {
    auto* item = new QTreeWidgetItem();

    item->setText(
        0,
        QString("Nodo %1").arg(++counter)
    );

    if (parent) {
        parent->addChild(item);
    } else {
        tree->addTopLevelItem(item);
    }

    if (node.closed) {
        item->setText(
            1,
            QString::fromUtf8(u8"✕ Cerrada: ")
                + toQt(node.contradiction)
        );
        item->setForeground(1, QBrush(QColor("#c62828")));
    } else if (node.children.empty()) {
        item->setText(1, "Abierta");
        item->setForeground(1, QBrush(QColor("#2e7d32")));
    } else {
        item->setText(1, QString::fromUtf8(u8"Expansión"));
    }

    for (const auto& entry : node.entries) {
        auto* formulaItem = new QTreeWidgetItem(item);

        formulaItem->setText(0, toQt(entry.formula->toString()));

        if (isLiteral(entry.formula)) {
            formulaItem->setText(1, "Literal");
        } else if (entry.applied) {
            formulaItem->setText(
                1, QString::fromUtf8(u8"✓ Aplicada")
            );
        } else {
            formulaItem->setText(1, "Pendiente");
        }
    }

    for (const auto& child : node.children) {
        addNode(*child, item, counter);
    }
}