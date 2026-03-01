#include "ManualPlotDialogCompare.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLabel>
#include <QMessageBox>

ManualPlotDialogCompare::ManualPlotDialogCompare(const QString& title,
                                                 const QString& yAxisLabel,
                                                 const QString& plotTitle,
                                                 QWidget* parent)
    : QDialog(parent), m_title(title), m_yAxisLabel(yAxisLabel), m_plotTitle(plotTitle)
{
    setWindowTitle("Ввод данных для " + m_title);
    setMinimumSize(600, 400);
    setAttribute(Qt::WA_DeleteOnClose, false);
    setWindowFlags(windowFlags() | Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* label = new QLabel("Введите точки:");
    layout->addWidget(label);

    table = new QTableWidget(0,2,this);
    table->setHorizontalHeaderLabels({"p_k","Ошибки"});
    table->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(table);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* addBtn = new QPushButton("Добавить точку");
    QPushButton* removeBtn = new QPushButton("Удалить выбранную");
    QPushButton* clearBtn = new QPushButton("Очистить все");

    buttonsLayout->addWidget(addBtn);
    buttonsLayout->addWidget(removeBtn);
    buttonsLayout->addWidget(clearBtn);
    buttonsLayout->addStretch();
    layout->addLayout(buttonsLayout);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    layout->addWidget(buttonBox);

    connect(addBtn, &QPushButton::clicked, this, &ManualPlotDialogCompare::addPoint);
    connect(removeBtn, &QPushButton::clicked, this, &ManualPlotDialogCompare::removePoint);
    connect(clearBtn, &QPushButton::clicked, this, &ManualPlotDialogCompare::clearPoints);
    connect(buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        updateTable();
        data.pk = m_pkValues;
        data.errors = m_errorValues;
        accept(); // Закрываем окно
    });
}

void ManualPlotDialogCompare::addPoint()
{
    m_pkValues.append(0.0);
    m_errorValues.append(0.0);
    updateTable();
    table->setCurrentCell(table->rowCount()-1,0);
    table->edit(table->currentIndex());
}

void ManualPlotDialogCompare::removePoint()
{
    int row = table->currentRow();
    if(row >= 0 && row < m_pkValues.size()) {
        m_pkValues.removeAt(row);
        m_errorValues.removeAt(row);
        updateTable();
    }
}

void ManualPlotDialogCompare::clearPoints()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Очистка");
    msgBox.setText("Вы уверены, что хотите удалить все точки?");
    msgBox.setIcon(QMessageBox::Question);

    QPushButton* yesButton = msgBox.addButton("Да", QMessageBox::YesRole);
    QPushButton* noButton  = msgBox.addButton("Нет", QMessageBox::NoRole);
    msgBox.exec();

    if(msgBox.clickedButton() == yesButton) {
        m_pkValues.clear();
        m_errorValues.clear();
        updateTable();
    }
}

void ManualPlotDialogCompare::updateTable()
{
    for(int i=0;i<table->rowCount() && i<m_pkValues.size();++i){
        if(table->item(i,0)) m_pkValues[i] = table->item(i,0)->text().toDouble();
        if(table->item(i,1)) m_errorValues[i] = table->item(i,1)->text().toDouble();
    }

    table->setRowCount(m_pkValues.size());
    for(int i=0;i<m_pkValues.size();++i){
        if(!table->item(i,0)) table->setItem(i,0,new QTableWidgetItem(QString::number(m_pkValues[i],'f',4)));
        else table->item(i,0)->setText(QString::number(m_pkValues[i],'f',4));

        if(!table->item(i,1)) table->setItem(i,1,new QTableWidgetItem(QString::number(m_errorValues[i])));
        else table->item(i,1)->setText(QString::number(m_errorValues[i]));
    }
}

void ManualPlotDialogCompare::closeEvent(QCloseEvent* event)
{
    this->hide();
    event->ignore(); // окно скрываем, главное не трогаем
}
