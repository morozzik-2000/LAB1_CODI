#include "mainwindow.h"
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QFormLayout>
#include <QApplication>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Коды исправляющие ошибки");
    resize(1200, 800);

    // ВАЖНО: Инициализация панелей!
    lab1Panel = nullptr;
    lab2Panel = nullptr;
    lab3Panel = nullptr;
    lab4Panel = nullptr;

    // Основной горизонтальный layout
    mainLayout = new QHBoxLayout(this);

    // Установка шрифта для всего приложения
    QFont font = qApp->font();
    font.setPointSize(11);
    qApp->setFont(font);

    // Добавляем левую и правую панели
    mainLayout->addWidget(createLeftPanel(), 1);

    // Создаем правую панель с двумя групбоксами
    QWidget *rightPanel = new QWidget;
    rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->addWidget(createTopPanel(), 2);
    rightLayout->addWidget(createLogPanel(), 1);

    mainLayout->addWidget(rightPanel, 4);

    setupConnections();
}

QGroupBox* MainWindow::createLeftPanel()
{
    leftGroupBox = new QGroupBox("📡 Управление");
    QVBoxLayout *mainLeftLayout = new QVBoxLayout(leftGroupBox);
    mainLeftLayout->setSpacing(10); // Расстояние между групбоксами

    // Первый групбокс для первой кнопки
    QGroupBox *firstGroupBox = new QGroupBox("Параметры кода");
    firstGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());

    QVBoxLayout *firstLayout = new QVBoxLayout(firstGroupBox);
    btnLab1 = new QPushButton("Задать параметры кода");
    btnLab1->setStyleSheet(ThemeStyles::lightButtonStyle());
    firstLayout->addWidget(btnLab1);

    leftGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    // Второй групбокс для остальных трех кнопок
    QGroupBox *secondGroupBox = new QGroupBox("Моделирование");

    secondGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    QVBoxLayout *secondLayout = new QVBoxLayout(secondGroupBox);
    btnLab2 = new QPushButton("Модель кодека с\n неискажающим каналом");
    btnLab2->setStyleSheet(ThemeStyles::lightButtonStyle());
    btnLab3 = new QPushButton("Модель канала ДСК");
    btnLab3->setStyleSheet(ThemeStyles::lightButtonStyle());
    btnLab4 = new QPushButton("Модель кодека с\n искажающим ДСК");
    btnLab4->setStyleSheet(ThemeStyles::lightButtonStyle());

    secondLayout->addWidget(btnLab2);
    secondLayout->addWidget(btnLab3);
    secondLayout->addWidget(btnLab4);
    // secondLayout->addStretch();

    // Третий групбокс сервисный
    QGroupBox *thirdGroupBox = new QGroupBox("Инструменты");

    saveLog = new QPushButton("Сохранить историю окна сообщений");
    saveLog->setStyleSheet(ThemeStyles::lightButtonStyle());
    toggleTheme = new QPushButton("Сменить тему");
    toggleTheme->setStyleSheet(ThemeStyles::lightButtonStyle());

    thirdGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    QVBoxLayout *thirdLayout = new QVBoxLayout(thirdGroupBox);
    thirdLayout->addWidget(saveLog);
    thirdLayout->addWidget(toggleTheme);

    // Добавляем оба групбокса в основной layout
    mainLeftLayout->addWidget(firstGroupBox);
    mainLeftLayout->addWidget(secondGroupBox);
    mainLeftLayout->addWidget(thirdGroupBox);
    mainLeftLayout->addStretch();

    return leftGroupBox;
}

QGroupBox* MainWindow::createTopPanel()
{
    topGroupBox = new QGroupBox("⚙ Рабочая область");
    topGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    topAreaLayout = new QVBoxLayout(topGroupBox);

    runOctaveButton = new QPushButton("Запустить моделирование");
    runOctaveButton->setMaximumWidth(240);
    runOctaveButton->setStyleSheet(ThemeStyles::OctaveButtonStyle());
    runOctaveButton->hide();
    topAreaLayout->addWidget(runOctaveButton);       // потом кнопка в самом конце

    return topGroupBox;
}

void MainWindow::hideAllPanels()
{
    if (lab1Panel) lab1Panel->hide();
    if (lab2Panel) lab2Panel->hide();
    if (lab3Panel) lab3Panel->hide();
    if (lab4Panel) lab4Panel->hide();
}


QGroupBox* MainWindow::createLogPanel()
{
    logGroupBox = new QGroupBox("📝 Окно сообщений");
    QVBoxLayout *layout = new QVBoxLayout(logGroupBox);

    logGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    logTextEdit = new QTextEdit;
    logTextEdit->setReadOnly(true);

    logTextEdit->setStyleSheet(ThemeStyles::logTextEdit());

    QPushButton *clearLog = new QPushButton("🗑 Очистить окно сообщений");
    clearLog->setStyleSheet(ThemeStyles::lightButtonStyle());
    layout->addWidget(logTextEdit);
    layout->addWidget(clearLog);


    return logGroupBox;
}

void MainWindow::setupConnections()
{
    connect(btnLab1, &QPushButton::clicked, this, &MainWindow::onLab1Clicked);
    connect(btnLab2, &QPushButton::clicked, this, &MainWindow::onLab2Clicked);
    connect(btnLab3, &QPushButton::clicked, this, &MainWindow::onLab3Clicked);
    connect(btnLab4, &QPushButton::clicked, this, &MainWindow::onLab4Clicked);
}

void MainWindow::clearTopArea()
{
    QLayoutItem* item;
    while ((item = topAreaLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}


void MainWindow::onLab1Clicked()
{
    hideAllPanels();
    logTextEdit->append("=== Задать параметры кода ===");
    runOctaveButton->hide();  // спрятать кнопку в Лабе 1

    // создаём панель только один раз
    if (!lab1Panel) {
        lab1Panel = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(lab1Panel);

        QLabel *title = new QLabel("Параметры кода");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size:18px; font-weight:bold;");


        QWidget *contentWidget = new QWidget;
        QFormLayout *formLayout = new QFormLayout(contentWidget);

        spinCodeLength = new QSpinBox;
        spinCodeLength->setRange(1, 1000);
        spinCodeLength->setValue(7);
        // spinCodeLength->setStyleSheet(ThemeStyles::lightSpinBoxStyle());
        formLayout->addRow("Длина кодового слова (n):", spinCodeLength);

        spinInfoLength = new QSpinBox;
        spinInfoLength->setRange(1, 1000);
        spinInfoLength->setValue(4);
        formLayout->addRow("Длина информационного слова (k):", spinInfoLength);

        spinСorrectingAbility = new QSpinBox;
        spinСorrectingAbility->setRange(1, 1000);
        spinСorrectingAbility->setValue(1);
        formLayout->addRow("Исправляющая способность (t):", spinСorrectingAbility);

        spinInWordsfoLength = new QSpinBox;
        spinInWordsfoLength->setRange(1, 1000);
        spinInWordsfoLength->setValue(16);
        formLayout->addRow("Количество слов:", spinInWordsfoLength);\

        QPushButton *btnApply = new QPushButton("Применить параметры");
        btnApply->setStyleSheet(ThemeStyles::lightButtonStyle());
        layout->addWidget(title);
        layout->addWidget(contentWidget);
        layout->addWidget(btnApply);
        layout->addStretch();

        topAreaLayout->addWidget(lab1Panel);
    }

    lab1Panel->show();
}

void MainWindow::onLab2Clicked()
{
    hideAllPanels();
    logTextEdit->append("=== Модель кодека с неискажающим каналом ===");

    if (!lab2Panel)
    {
        // Создаем панель ЛР2 только один раз
        lab2Panel = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(lab2Panel);
        layout->setSpacing(15); // Увеличиваем расстояние между виджетами
        QLabel *title = new QLabel("Неискажающий канал");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size:18px; font-weight:bold;");

        // Группа для кнопок-секций с горизонтальным layout
        QGroupBox *buttonsGroup = new QGroupBox("Зависимости");
        QVBoxLayout *groupLayout = new QVBoxLayout(buttonsGroup);
        buttonsGroup->setMaximumWidth(410);

        // Кнопки
        QPushButton *inputWord = new QPushButton("Входное слово");
        QPushButton *decodedWord = new QPushButton("Декодированное слово");
        QPushButton *codeSequence = new QPushButton("Кодовая последовательность (реализация)");
        QPushButton *comparison = new QPushButton("Сравнение входной и\nдекодированной последовательностей");
        QPushButton *threeOnOne = new QPushButton("Три на одном");

        inputWord->setStyleSheet(ThemeStyles::lightButtonStyle());
        decodedWord->setStyleSheet(ThemeStyles::lightButtonStyle());
        codeSequence->setStyleSheet(ThemeStyles::lightButtonStyle());
        comparison->setStyleSheet(ThemeStyles::lightButtonStyle());
        threeOnOne->setStyleSheet(ThemeStyles::lightButtonStyle());

        groupLayout->addWidget(inputWord);
        groupLayout->addWidget(decodedWord);
        groupLayout->addWidget(codeSequence);
        groupLayout->addWidget(comparison);
        groupLayout->addWidget(threeOnOne);

        // Добавление в layout
        layout->addWidget(title);
        layout->addWidget(buttonsGroup);

        runOctaveButton->show();
        layout->addStretch();

        // Добавить панель в верхнюю область
        topAreaLayout->insertWidget(0, lab2Panel);
    }

    lab2Panel->show();
    runOctaveButton->show();  // <-- переносим сюда!
    topAreaLayout->addStretch();
}



void MainWindow::onLab3Clicked()
{
    hideAllPanels();
    logTextEdit->append("=== Модель канала ДСК (двоичный симметричный канал) ===");

    if (!lab3Panel)
    {
        lab3Panel = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(lab3Panel);
        // Увеличиваем отступы между элементами в основном layout
        layout->setSpacing(15); // Увеличиваем расстояние между виджетами

        // Заголовок
        QLabel *title = new QLabel("Двоичный симметричный канал (ДСК)");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size:18px; font-weight:bold;");

        // Параметр: вероятность ошибки канала
        QLabel *labelProbabilityChannelError = new QLabel("Вероятность канальной ошибки (p):");
        QDoubleSpinBox *spinProbabilityChannelError = new QDoubleSpinBox;
        spinProbabilityChannelError->setRange(0.0, 1.0);
        spinProbabilityChannelError->setSingleStep(0.01);
        spinProbabilityChannelError->setValue(0.1);
        spinProbabilityChannelError->setMaximumWidth(80);

        // Формовый layout
        QFormLayout *formLayout = new QFormLayout;
        formLayout->setVerticalSpacing(8);

        formLayout->addRow(labelProbabilityChannelError, spinProbabilityChannelError);

        // Группа для кнопок-секций с горизонтальным layout
        QGroupBox *buttonsGroup = new QGroupBox("Зависимости");
        QVBoxLayout *groupLayout = new QVBoxLayout(buttonsGroup);

        // Кнопки-секции
        QPushButton *sequenceAtTheEncoderOutput = new QPushButton("Выход кодера");
        QPushButton *sequenceAtTheInputOfTheDecoder = new QPushButton("Вход декодера");
        QPushButton *errorVector = new QPushButton("Вектор ошибок");

        sequenceAtTheEncoderOutput->setStyleSheet(ThemeStyles::lightButtonStyle());
        sequenceAtTheInputOfTheDecoder->setStyleSheet(ThemeStyles::lightButtonStyle());
        errorVector->setStyleSheet(ThemeStyles::lightButtonStyle());

        buttonsGroup->setMaximumWidth(280);

        // Добавляем кнопки в группу
        groupLayout->addWidget(sequenceAtTheEncoderOutput);
        groupLayout->addWidget(sequenceAtTheInputOfTheDecoder);
        groupLayout->addWidget(errorVector);

        // Добавление на панель
        layout->addWidget(title);
        layout->addLayout(formLayout);
        // layout->addSpacing(15);
        layout->addWidget(buttonsGroup);
        // layout->addWidget(runOctaveButton);
        runOctaveButton->show();
        layout->addStretch();

        // Добавить панель в верхнюю рабочую область
        topAreaLayout->insertWidget(0, lab3Panel);
    }

    lab3Panel->show();
    runOctaveButton->show();  // <-- переносим сюда!
    topAreaLayout->addStretch();
}


void MainWindow::onLab4Clicked()
{
    hideAllPanels();
    logTextEdit->append("=== Модель кодека с искажающим ДСК ===");

    if (!lab4Panel)
    {
        lab4Panel = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(lab4Panel);
        layout->setSpacing(15); // Увеличиваем расстояние между виджетами
        // Заголовок
        QLabel *title = new QLabel("Кодек с искажающим двоичным симметричным каналом (ДСК)");
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size:18px; font-weight:bold;");

        // Вероятность ошибки
        QLabel *labelProbabilityChannelError = new QLabel("Вероятность канальной ошибки (p):");
        QDoubleSpinBox *spinProbabilityChannelError = new QDoubleSpinBox;
        spinProbabilityChannelError->setRange(0.0, 1.0);
        spinProbabilityChannelError->setSingleStep(0.01);
        spinProbabilityChannelError->setValue(0.1);
        spinProbabilityChannelError->setMaximumWidth(80);

        // Формовый блок
        QFormLayout *formLayout = new QFormLayout;
        formLayout->setVerticalSpacing(12);
        formLayout->addRow(labelProbabilityChannelError, spinProbabilityChannelError);

        // Группа для кнопок-секций с горизонтальным layout
        QGroupBox *buttonsGroup = new QGroupBox("Зависимости");
        QVBoxLayout *groupLayout = new QVBoxLayout(buttonsGroup);
        buttonsGroup->setMaximumWidth(380);
        // Кнопки
        QPushButton *informationSequence = new QPushButton("Информационная последовательность");
        QPushButton *decoderOutputSequence = new QPushButton("Последовательность на выходе декодера");
        QPushButton *comparison = new QPushButton("Сравнение информационной и\nдекодированной последовательности");

        informationSequence->setStyleSheet(ThemeStyles::lightButtonStyle());
        decoderOutputSequence->setStyleSheet(ThemeStyles::lightButtonStyle());
        comparison->setStyleSheet(ThemeStyles::lightButtonStyle());

        groupLayout->addWidget(informationSequence);
        groupLayout->addWidget(decoderOutputSequence);
        groupLayout->addWidget(comparison);

        // Сборка панели
        layout->addWidget(title);
        layout->addLayout(formLayout);
        layout->addWidget(buttonsGroup);
        runOctaveButton->show();
        layout->addStretch();

        // Добавляем панель в верхнюю область
        topAreaLayout->insertWidget(0, lab4Panel);
    }

    lab4Panel->show();
    runOctaveButton->show();  // <-- переносим сюда!
    topAreaLayout->addStretch();

}


void MainWindow::onLabButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString labName = btn->text();
    logTextEdit->append("Выбрана: " + labName);
    qDebug() << "Button clicked:" << labName;
}


