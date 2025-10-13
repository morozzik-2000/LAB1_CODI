#include "Lab2Panel.h"
#include "themestyles.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

Lab2Panel::Lab2Panel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    QLabel *title = new QLabel("Неискажающий канал");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px; font-weight:bold;");

    QGroupBox *buttonsGroup = new QGroupBox("Зависимости");
    buttonsGroup->setMaximumWidth(420);
    buttonsGroup->setStyleSheet(ThemeStyles::lightGroupBoxStyle());

    QVBoxLayout *gl = new QVBoxLayout(buttonsGroup);
    auto *inputWord = new QPushButton("Входное слово");
    auto *decoded = new QPushButton("Декодированное слово");
    auto *codeSeq = new QPushButton("Кодовая последовательность (реализация)");
    auto *compare = new QPushButton("Сравнение входной и\nдекодированной последовательностей");
    auto *three = new QPushButton("Три на одном");


    for (auto *b : {inputWord, decoded, codeSeq, compare, three})
        b->setStyleSheet(ThemeStyles::lightButtonStyle());

    gl->addWidget(inputWord);
    gl->addWidget(decoded);
    gl->addWidget(codeSeq);
    gl->addWidget(compare);
    gl->addWidget(three);

    // connect demo signals to logging for demo purpose
    connect(inputWord, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Входное слово"); });
    connect(decoded, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Декодированное слово"); });
    connect(codeSeq, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Кодовая последовательность"); });
    connect(compare, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Сравнение последовательностей"); });
    connect(three, &QPushButton::clicked, this, [=](){ emit logMessage("Показано: Три на одном"); });

    layout->addWidget(title);
    layout->addWidget(buttonsGroup);
    layout->addStretch();


}
