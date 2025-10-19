#include "Lab1Panel.h"
#include "CodecDiagramWidget.h"
#include "themestyles.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QSpinBox>
#include "Lab1Panel.h"
#include "CodecDiagramWidget.h"
#include "themestyles.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QPushButton>

Lab1Panel::Lab1Panel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(10);

    // Добавляем заголовок для схемы
    QLabel *diagramTitle = new QLabel("Схема системы передачи данных");
    diagramTitle->setAlignment(Qt::AlignCenter);
    diagramTitle->setStyleSheet("font-size:16px; font-weight:bold; margin:10px;");
    layout->addWidget(diagramTitle);

    // Добавляем виджет с диаграммой
    CodecDiagramWidget *diagram = new CodecDiagramWidget;
    diagram->setStyleSheet("background-color: white; border: 1px solid #ccc; border-radius: 5px;");
    layout->addWidget(diagram);


    QLabel *title = new QLabel("Параметры кода");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px; font-weight:bold;");

    QWidget *content = new QWidget;
    auto *form = new QFormLayout(content);

    spinCodeLength = new QSpinBox;
    spinCodeLength->setRange(1, 1000);
    spinCodeLength->setValue(7);

    spinInfoLength = new QSpinBox;
    spinInfoLength->setRange(1, 1000);
    spinInfoLength->setValue(4);

    spinCorrecting = new QSpinBox;
    spinCorrecting->setRange(0, 1000);
    spinCorrecting->setValue(1);

    spinWords = new QSpinBox;
    spinWords->setRange(1, 1000000);
    spinWords->setValue(16);

    form->addRow("Длина кодового слова (n):", spinCodeLength);
    form->addRow("Длина информационного слова (k):", spinInfoLength);
    form->addRow("Исправляющая способность (t):", spinCorrecting);
    form->addRow("Количество слов:", spinWords);

    QPushButton *apply = new QPushButton("Применить параметры");
    apply->setStyleSheet(ThemeStyles::lightButtonStyle());

    connect(apply, &QPushButton::clicked, [=]() {
        QString s = QString("Параметры применены: n=%1 k=%2 t=%3 words=%4")
                        .arg(spinCodeLength->value())
                        .arg(spinInfoLength->value())
                        .arg(spinCorrecting->value())
                        .arg(spinWords->value());
        emit logMessage(s);

    });

    layout->addWidget(title);
    layout->addWidget(content);
    layout->addWidget(apply, 0, Qt::AlignLeft);
    layout->addStretch();
}

OctaveParams Lab1Panel::getParams() const
{
    OctaveParams p;
    p.n = spinCodeLength->value();
    p.k = spinInfoLength->value();
    p.t = spinCorrecting->value();
    p.numWords = spinWords->value();
    p.errorProb = 0.01; // можно сделать отдельный QDoubleSpinBox, если нужно
    p.part = 1; // по умолчанию, можно менять в зависимости от выбранной панели
    return p;
}
