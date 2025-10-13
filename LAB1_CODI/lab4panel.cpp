#include "lab4panel.h"
#include "themestyles.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QPushButton>

Lab4Panel::Lab4Panel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    QLabel *title = new QLabel("Кодек с искажающим двоичным симметричным каналом (ДСК)");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px; font-weight:bold;");

    QFormLayout *form = new QFormLayout;
    QDoubleSpinBox *pSpin = new QDoubleSpinBox;
    pSpin->setRange(0.0, 1.0);
    pSpin->setSingleStep(0.01);
    pSpin->setValue(0.1);
    pSpin->setMaximumWidth(90);

    form->addRow("Вероятность канальной ошибки (p):", pSpin);

    QGroupBox *buttonsGroup = new QGroupBox("Зависимости");
    buttonsGroup->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    buttonsGroup->setMaximumWidth(380);
    QVBoxLayout *gl = new QVBoxLayout(buttonsGroup);

    QGroupBox *buttonsHandGroup = new QGroupBox("Построить вручную");
    buttonsHandGroup->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    buttonsHandGroup->setMaximumWidth(300);
    QVBoxLayout *glh = new QVBoxLayout(buttonsHandGroup);

    auto *infoSeq = new QPushButton("Информационная последовательность");
    auto *decOut = new QPushButton("Последовательность на выходе декодера");
    auto *cmp = new QPushButton("Сравнение информационной и\nдекодированной последовательности");



    auto *ber_dk = new QPushButton("Зависимость битовой ошибки\n на выходе декодера");


    for (auto *b : {infoSeq, decOut, cmp, ber_dk}) b->setStyleSheet(ThemeStyles::lightButtonStyle());
    gl->addWidget(infoSeq);
    gl->addWidget(decOut);
    gl->addWidget(cmp);

    glh->addWidget(ber_dk);

    connect(infoSeq, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Информационная последовательность"); });
    connect(decOut, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Последовательность на выходе декодера"); });
    connect(cmp, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Сравнение информационной и декодированной последовательности"); });

    connect(ber_dk, &QPushButton::clicked, this, [=](){ emit logMessage("Выбрано: Зависимость битовой ошибки на выходе декодера");


        ManualPlotDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            emit logMessage("График успешно построен");
        }
    });


    layout->addWidget(title);
    layout->addLayout(form);
    layout->addWidget(buttonsGroup);
    layout->addWidget(buttonsHandGroup);
    layout->addStretch();

}
