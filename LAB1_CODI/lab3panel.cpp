#include "Lab3Panel.h"
#include "themestyles.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QPushButton>

Lab3Panel::Lab3Panel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    QLabel *title = new QLabel("Двоичный симметричный канал (ДСК)");
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
    buttonsGroup->setMaximumWidth(300);
    QVBoxLayout *gl = new QVBoxLayout(buttonsGroup);

//    QGroupBox *buttonsHandGroup = new QGroupBox("Построить вручную");
//    buttonsHandGroup->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
//    buttonsHandGroup->setMaximumWidth(300);
//    QVBoxLayout *glh = new QVBoxLayout(buttonsHandGroup);

    auto *outEnc = new QPushButton("Выход кодера");
    auto *inDec = new QPushButton("Вход декодера");
    auto *errVec = new QPushButton("Вектор ошибок");

//    auto *ber_dk = new QPushButton("Зависимость битовой ошибки\n на выходе декодера");



    for (auto *b : {outEnc, inDec, errVec}) b->setStyleSheet(ThemeStyles::lightButtonStyle());
    gl->addWidget(outEnc);
    gl->addWidget(inDec);
    gl->addWidget(errVec);


//    glh->addWidget(ber_dk);

    connect(outEnc, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Выход кодера"); });
    connect(inDec, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Вход декодера"); });
    connect(errVec, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Вектор ошибок"); });

//    connect(ber_dk, &QPushButton::clicked, this, [=](){ emit logMessage("Выбрано: Зависимость битовой ошибки на выходе декодера"); });

    layout->addWidget(title);
    layout->addLayout(form);
    layout->addWidget(buttonsGroup);
//    layout->addWidget(buttonsHandGroup);
    layout->addStretch();

}
