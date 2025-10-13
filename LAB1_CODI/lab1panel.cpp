#include "Lab1Panel.h"
#include "themestyles.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>

Lab1Panel::Lab1Panel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(10);

    QLabel *title = new QLabel("Параметры кода");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px; font-weight:bold;");

    QWidget *content = new QWidget;
    auto *form = new QFormLayout(content);

    QSpinBox *spinCodeLength = new QSpinBox;
    spinCodeLength->setRange(1, 1000);
    spinCodeLength->setValue(7);

    QSpinBox *spinInfoLength = new QSpinBox;
    spinInfoLength->setRange(1, 1000);
    spinInfoLength->setValue(4);

    QSpinBox *spinCorrecting = new QSpinBox;
    spinCorrecting->setRange(0, 1000);
    spinCorrecting->setValue(1);

    QSpinBox *spinWords = new QSpinBox;
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
