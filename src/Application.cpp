#include "Application.hpp"
#include "Parser.hpp"
#include "TreeRenderer.hpp"
#include "AtomRegistry.hpp"
#include "Tableau.hpp"
#include "Formula.hpp"

#include <QAbstractItemView>
#include <QBrush>
#include <QColor>
#include <QHeaderView>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPixmap>
#include <QFont>
#include <exception>
#include <vector>
#include <string>

namespace {
QString toQt(const std::string& text) {
    return QString::fromUtf8(text.c_str());
}
} 

Application::Application(QWidget* parent)
    : QWidget(parent) {
    setWindowTitle("TareaIA - Arbol semantico");
    resize(1150, 850);

    // --- ESTILOS CSS ---
    setStyleSheet(R"(
        QWidget {
            background: #f3f6fb;
            color: #1e293b;
            font-family: "Segoe UI", "Arial";
            font-size: 10pt;
        }
        QFrame#header {
            background: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 14px;
        }
        QPlainTextEdit, QTreeWidget, QTableWidget {
            background: #ffffff;
            border: 1px solid #dbe4ef;
            border-radius: 5px;
            padding: 4px;
        }
        QPushButton {
            background: #eaf0f7;
            color: #173b63;
            border: 1px solid #d5e0ec;
            border-radius: 5px;
            padding: 8px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #dce9f7;
        }
        QTabWidget::pane {
            border: 1px solid #cbd5e1;
            background: #ffffff;
            border-radius: 5px;
            top: -1px;
        }
        QTabBar::tab {
            background: #f1f5f9;
            color: #334155;
            padding: 8px 16px;
            border: 1px solid #cbd5e1;
            border-bottom: none;
            border-top-left-radius: 5px;
            border-top-right-radius: 5px;
            margin-right: 2px;
        }
        QTabBar::tab:selected {
            background: #ffffff;
            color: #0f172a;
            font-weight: bold;
            border-bottom: 1px solid #ffffff;
        }
    )");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    // --- ENCABEZADO CON LOGO Y DATOS ---
    auto* header = new QFrame(this);
    header->setObjectName("header");
    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(15, 10, 15, 10);

    auto* logoLabel = new QLabel(header);
    QPixmap logoPixmap(":/Resources/CETI_Logo.png");
    // Escalar el logo al tamaño del encabezado
    logoLabel->setPixmap(logoPixmap.scaled(200, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setMinimumWidth(200);
    logoLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    headerLayout->addWidget(logoLabel);
    headerLayout->addStretch(); // Empuja los datos a la derecha

    auto* studentInfo = new QVBoxLayout();
    studentInfo->setSpacing(2);
    auto* studentName = new QLabel("Paul Oswaldo Candia Fullen", header);
    studentName->setStyleSheet("font-size: 14pt; font-weight: 700; color: #12385e;");
    studentName->setAlignment(Qt::AlignRight);
    
    auto* registration = new QLabel("Registro: 24110244", header);
    registration->setStyleSheet("color: #64748b;");
    registration->setAlignment(Qt::AlignRight);
    
    auto* group = new QLabel("Grupo: 6E", header);
    group->setStyleSheet("color: #64748b;");
    group->setAlignment(Qt::AlignRight);
    
    studentInfo->addWidget(studentName);
    studentInfo->addWidget(registration);
    studentInfo->addWidget(group);
    headerLayout->addLayout(studentInfo);
    
    mainLayout->addWidget(header);

    // --- CUERPO PRINCIPAL (El diseño original que te gusta) ---
    mainLayout->addWidget(new QLabel("Base de conocimiento: escribe una regla por línea", this));
    
    input = new QPlainTextEdit(this);
    input->setMinimumHeight(120);
    input->setPlainText(
        QString::fromUtf8(
            u8"Si es de día, entonces voy a trabajar\n"
            u8"Si llueve, entonces no voy a trabajar\n"
            u8"Es de día"
        )
    );
    mainLayout->addWidget(input);

    auto* button = new QPushButton("Analizar", this);
    mainLayout->addWidget(button);

    auto* tabs = new QTabWidget(this);
    
    atoms = new QTableWidget(0, 2, tabs);
    atoms->setHorizontalHeaderLabels(QStringList{"Letra", "Significado"});
    atoms->horizontalHeader()->setStretchLastSection(true);
    atoms->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    formulas = new QPlainTextEdit(tabs);
    formulas->setReadOnly(true);
    
    treeView = new TreeRenderer(tabs);
    
    tree = new QTreeWidget(tabs);
    tree->setHeaderLabels(QStringList{"Desarrollo", "Estado"});
    tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tree->setAlternatingRowColors(true);
    
    results = new QPlainTextEdit(tabs);
    results->setReadOnly(true);

    // Añadir pestañas en el mismo orden que el original
    tabs->addTab(atoms, QString::fromUtf8(u8"Átomos"));
    tabs->addTab(formulas, QString::fromUtf8(u8"Fórmulas"));
    tabs->addTab(treeView, QString::fromUtf8(u8"Árbol gráfico"));
    tabs->addTab(tree, "Detalle de ramas");
    tabs->addTab(results, "Resultados");
    
    mainLayout->addWidget(tabs, 1);

    // --- CONEXIÓN DEL BOTÓN ---
    connect(button, &QPushButton::clicked, this, [this, tabs]() {
        analyze();
        tabs->setCurrentWidget(results); // Vuelve a la pestaña de resultados al terminar
    });
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
            if (line.isEmpty()) continue;

            Formula::Ptr formula;
            try {
                formula = parser.parse(line.toUtf8().toStdString());
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
            atoms->setItem(row, 0, new QTableWidgetItem(toQt(atom.symbol)));
            atoms->setItem(row, 1, new QTableWidgetItem(toQt(atom.meaning)));
        }

        int counter = 0;
        addNode(*result.root, nullptr, counter);
        tree->expandAll();
        treeView->setTree(*result.root);

        QStringList output;
        output << (result.isSatisfiable() ? "Base de conocimiento SATISFACIBLE." : "Base de conocimiento INSATISFACIBLE.");
        output << QString("Ramas cerradas: %1").arg(static_cast<qulonglong>(result.closedBranches));
        output << QString("Ramas abiertas: %1").arg(static_cast<qulonglong>(result.models.size()));

        for (std::size_t i = 0; i < result.models.size(); ++i) {
            const auto& model = result.models[i];
            output << "";
            output << QString("Rama abierta %1:").arg(static_cast<qulonglong>(i + 1));

            for (const auto& atom : registry.entries()) {
                const auto assignment = model.find(atom.symbol);
                QString value;
                if (assignment == model.end()) {
                    value = "libre (V o F)";
                } else {
                    value = assignment->second ? "V" : "F";
                }
                output << QString("  %1 = %2").arg(toQt(atom.symbol)).arg(value);
            }
        }
        results->setPlainText(output.join("\n"));

    } catch (const std::exception& error) {
        results->setPlainText(QString("Error: %1").arg(toQt(error.what())));
    }
}

void Application::addNode(const TableauNode& node, QTreeWidgetItem* parent, int& counter) {
    auto* item = new QTreeWidgetItem();
    item->setText(0, QString("Nodo %1").arg(++counter));

    if (parent) {
        parent->addChild(item);
    } else {
        tree->addTopLevelItem(item);
    }

    if (node.closed) {
        item->setText(1, QString::fromUtf8(u8"✕ Cerrada: ") + toQt(node.contradiction));
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

        // Usamos la función isLiteral que viene de Expansion.hpp
        if (isLiteral(entry.formula)) {
            formulaItem->setText(1, "Literal");
        } else if (entry.applied) {
            formulaItem->setText(1, QString::fromUtf8(u8"✓ Aplicada"));
        } else {
            formulaItem->setText(1, "Pendiente");
        }
    }

    for (const auto& child : node.children) {
        addNode(*child, item, counter);
    }
}