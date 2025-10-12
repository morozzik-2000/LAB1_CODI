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
    firstGroupBox->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            border: 2px solid #aaa;
            border-radius: 15px;
            margin-top: 10px;
            padding-top: 15px;
            background-color: #f8f8f8;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            background-color: #f8f8f8;
        }
    )");

    QVBoxLayout *firstLayout = new QVBoxLayout(firstGroupBox);
    btnLab1 = new QPushButton("Задать параметры кода");
    firstLayout->addWidget(btnLab1);
    // firstLayout->addStretch();

    leftGroupBox->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            border: 2px solid #aaa;
            border-radius: 15px;
            margin-top: 10px;
            padding-top: 15px;
            background-color: #f8f8f8;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            background-color: #f8f8f8;
        }
    )");

    // Второй групбокс для остальных трех кнопок
    QGroupBox *secondGroupBox = new QGroupBox("Моделирование");
    secondGroupBox->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            border: 2px solid #aaa;
            border-radius: 15px;
            margin-top: 10px;
            padding-top: 15px;
            background-color: #f8f8f8;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            background-color: #f8f8f8;
        }
    )");

    QVBoxLayout *secondLayout = new QVBoxLayout(secondGroupBox);
    btnLab2 = new QPushButton("Модель кодека с\n неискажающим каналом");
    btnLab3 = new QPushButton("Модель канала ДСК");
    btnLab4 = new QPushButton("Модель кодека с\n искажающим ДСК");

    secondLayout->addWidget(btnLab2);
    secondLayout->addWidget(btnLab3);
    secondLayout->addWidget(btnLab4);
    // secondLayout->addStretch();

    // Третий групбокс сервисный
    QGroupBox *thirdGroupBox = new QGroupBox("Инструменты");

    saveLog = new QPushButton("Сохранить историю окна сообщений");
    toggleTheme = new QPushButton("Сменить тему");

    thirdGroupBox->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            border: 2px solid #aaa;
            border-radius: 15px;
            margin-top: 10px;
            padding-top: 15px;
            background-color: #f8f8f8;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            background-color: #f8f8f8;
        }
    )");

    QVBoxLayout *thirdLayout = new QVBoxLayout(thirdGroupBox);
    thirdLayout->addWidget(saveLog);
    thirdLayout->addWidget(toggleTheme);
    // secondLayout->addStretch();



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
    topGroupBox->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            border: 2px solid #aaa;
            border-radius: 15px;
            margin-top: 10px;
            padding-top: 15px;
            background-color: #f8f8f8;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            background-color: #f8f8f8;
        }
    )");

    topAreaLayout = new QVBoxLayout(topGroupBox);
    // labelCodeLength = new QLabel("💡 Длина кодового слова (n): ");
    // labelInfoLength = new QLabel("💡 Длина кодового слова (n): ");
    // labelСorrectingAbility = new QLabel("💡 Длина кодового слова (n): ");
    // labelWordsfoLength = new QLabel("💡 Длина кодового слова (n): ");
    // topAreaLayout->addWidget(labelCodeLength);
    // topAreaLayout->addWidget(labelInfoLength);
    // topAreaLayout->addWidget(labelСorrectingAbility);
    // topAreaLayout->addWidget(labelWordsfoLength);



    return topGroupBox;
}

QGroupBox* MainWindow::createLogPanel()
{
    logGroupBox = new QGroupBox("📝 Окно сообщений");
    QVBoxLayout *layout = new QVBoxLayout(logGroupBox);

    logGroupBox->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            font-size: 14px;
            border: 2px solid #aaa;
            border-radius: 15px;
            margin-top: 10px;
            padding-top: 15px;
            background-color: #f8f8f8;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            background-color: #f8f8f8;
        }
    )");

    logTextEdit = new QTextEdit;
    logTextEdit->setReadOnly(true);

    logTextEdit->setStyleSheet(R"(
        QTextEdit {
            background-color: #ffffff;
            font-size: 14px;
            border: 1px solid #ccc;
            border-radius: 5px;
            padding: 5px;
        }
    )");

    QPushButton *clearLog = new QPushButton("🗑 Очистить окно сообщений");
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
    clearTopArea();
    logTextEdit->append("=== Задать параметры кода ===");

    QLabel *title = new QLabel("Параметры кода:");
    title->setAlignment(Qt::AlignCenter);

    QWidget *contentWidget = new QWidget;
    QFormLayout *formLayout = new QFormLayout(contentWidget);
    formLayout->setVerticalSpacing(15);
    formLayout->setHorizontalSpacing(10);

    QLabel *labelCodeLength = new QLabel("Длина кодового слова (n):");
    spinCodeLength = new QSpinBox;
    spinCodeLength->setRange(1, 1000);
    spinCodeLength->setValue(7);
    formLayout->addRow(labelCodeLength, spinCodeLength);

    QLabel *labelInfoLength = new QLabel("Длина информационного слова (k):");
    spinInfoLength = new QSpinBox;
    spinInfoLength->setRange(1, 1000);
    spinInfoLength->setValue(4);
    formLayout->addRow(labelInfoLength, spinInfoLength);

    QLabel *labelСorrectingAbility = new QLabel("Исправляющая способность кода (t):");
    spinСorrectingAbility = new QSpinBox;
    spinСorrectingAbility->setRange(1, 1000);
    spinСorrectingAbility->setValue(1);
    formLayout->addRow(labelСorrectingAbility, spinСorrectingAbility);

    QLabel *labelInWordsfoLength = new QLabel("Количество информационных слов:");
    spinInWordsfoLength = new QSpinBox;
    spinInWordsfoLength->setRange(1, 1000);
    spinInWordsfoLength->setValue(16);
    formLayout->addRow(labelInWordsfoLength, spinInWordsfoLength);

    QPushButton *btnApply = new QPushButton("Применить параметры");
    QPushButton *btnReset = new QPushButton("Сбросить");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(btnApply);
    buttonLayout->addWidget(btnReset);
    buttonLayout->addStretch();

    // setupLabels(); // Создание лейблов
    // labelsVisibleFalse(false);

    topAreaLayout->addWidget(title);
    topAreaLayout->addSpacing(20);
    topAreaLayout->addWidget(contentWidget);
    topAreaLayout->addSpacing(20);
    topAreaLayout->addLayout(buttonLayout);
    topAreaLayout->addStretch();

    connect(btnApply, &QPushButton::clicked, this, [this](){
        qDebug() << "Применены параметры:";
        qDebug() << "Длина кодового слова:" << spinCodeLength->value();
        qDebug() << "Длина информационного слова:" << spinInfoLength->value();
        qDebug() << "Исправляющая способность:" << spinСorrectingAbility->value();
        qDebug() << "Количество информационных слов:" << spinInWordsfoLength->value();
    });

    connect(btnReset, &QPushButton::clicked, this, [this](){
        spinCodeLength->setValue(7);
        spinInfoLength->setValue(4);
        spinСorrectingAbility->setValue(1);
        spinInWordsfoLength->setValue(16);
        qDebug() << "Параметры сброшены к значениям по умолчанию";
    });
}

void MainWindow::onLab2Clicked()
{
    clearTopArea();
    // labelsVisibleFalse(true);
    logTextEdit->append("=== Модель кодека с неискажающим каналом ===");

    QLabel *title = new QLabel("Неискажающий канал:");
    title->setAlignment(Qt::AlignCenter);

    QPushButton *runButton = new QPushButton("Run Octave");
    runButton->setStyleSheet("font-size:16px; padding:8px; background:#27ae60; color:white; border-radius:6px;");
    QLabel *statusLabel = new QLabel("Ready");
    statusLabel->setStyleSheet("background:#ecf0f1; padding:4px;");

    QWidget *contentWidget = new QWidget;
    QFormLayout *formLayout = new QFormLayout(contentWidget);
    formLayout->setVerticalSpacing(15);
    formLayout->setHorizontalSpacing(10);
    formLayout->addRow(runButton, statusLabel);

    QPushButton *inputWord = new QPushButton("Входное слово");
    QPushButton *decodedWord = new QPushButton("Декодированное слово");
    QPushButton *codeSequence = new QPushButton("Кодовая последовательность(реализация)");
    QPushButton *compareInputWordDecodedWord = new QPushButton("Результат сравнения\nвходной последовательности и декодированной");
    QPushButton *threeOnOne = new QPushButton("Три на одном");

    topAreaLayout->addWidget(title);
    topAreaLayout->addWidget(contentWidget);
    topAreaLayout->addSpacing(10);
    topAreaLayout->addWidget(inputWord);
    topAreaLayout->addWidget(decodedWord);
    topAreaLayout->addWidget(codeSequence);
    topAreaLayout->addWidget(compareInputWordDecodedWord);
    topAreaLayout->addWidget(threeOnOne);
    topAreaLayout->addStretch();
}

void MainWindow::onLab3Clicked()
{
    clearTopArea();
    // labelsVisibleFalse(true);
    logTextEdit->append("=== Модель канала ДСК ===");

    QLabel *title = new QLabel("Двоичный симметричный канал:");
    title->setAlignment(Qt::AlignCenter);

    QPushButton *runButton = new QPushButton("Run Octave");
    runButton->setStyleSheet("font-size:16px; padding:8px; background:#27ae60; color:white; border-radius:6px;");
    QLabel *statusLabel = new QLabel("Ready");
    statusLabel->setStyleSheet("background:#ecf0f1; padding:4px;");

    QWidget *contentWidget = new QWidget;
    QFormLayout *formLayout = new QFormLayout(contentWidget);
    formLayout->setVerticalSpacing(15);
    formLayout->setHorizontalSpacing(10);

    QLabel *labelProbabilityChannelError = new QLabel("Вероятность канальной ошибки:");
    QDoubleSpinBox *spinProbabilityChannelError = new QDoubleSpinBox;
    spinProbabilityChannelError->setRange(0.0, 1.0);
    spinProbabilityChannelError->setValue(0.1);
    spinProbabilityChannelError->setSingleStep(0.01);
    formLayout->addRow(labelProbabilityChannelError, spinProbabilityChannelError);
    formLayout->addRow(runButton, statusLabel);

    QPushButton *sequenceAtTheEncoderOutput = new QPushButton("Последовательность на выходе кодера");
    QPushButton *sequenceAtTheInputOfTheDecoder = new QPushButton("Последовательность на входе декодера");
    QPushButton *errorVector = new QPushButton("Вектор ошибок");

    topAreaLayout->addWidget(title);
    topAreaLayout->addWidget(contentWidget);
    topAreaLayout->addSpacing(10);
    topAreaLayout->addWidget(sequenceAtTheEncoderOutput);
    topAreaLayout->addWidget(sequenceAtTheInputOfTheDecoder);
    topAreaLayout->addWidget(errorVector);
    topAreaLayout->addStretch();
}

void MainWindow::onLab4Clicked()
{
    clearTopArea();
    // labelsVisibleFalse(true);
    logTextEdit->append("=== Модель кодека с искажающим ДСК ===");

    QLabel *title = new QLabel("Кодек с искажающим каналом:");
    title->setAlignment(Qt::AlignCenter);

    QWidget *contentWidget = new QWidget;
    QFormLayout *formLayout = new QFormLayout(contentWidget);
    formLayout->setVerticalSpacing(15);
    formLayout->setHorizontalSpacing(10);

    QLabel *labelProbabilityChannelError = new QLabel("Вероятность канальной ошибки:");
    QDoubleSpinBox *spinProbabilityChannelError = new QDoubleSpinBox;
    spinProbabilityChannelError->setRange(0.0, 1.0);
    spinProbabilityChannelError->setValue(0.1);
    spinProbabilityChannelError->setSingleStep(0.01);
    formLayout->addRow(labelProbabilityChannelError, spinProbabilityChannelError);

    QPushButton *runButton = new QPushButton("Run Octave");
    runButton->setStyleSheet("font-size:16px; padding:8px; background:#27ae60; color:white; border-radius:6px;");
    QLabel *statusLabel = new QLabel("Ready");
    statusLabel->setStyleSheet("background:#ecf0f1; padding:4px;");
    formLayout->addRow(runButton, statusLabel);

    QPushButton *informationSequence = new QPushButton("Информационная последовательность");
    QPushButton *decoderOutputSequence = new QPushButton("Последовательность на выходе декодера");
    QPushButton *compareInformationAndDecoderOutput = new QPushButton("Сравнение информационной и\nдекодированной последовательности");

    topAreaLayout->addWidget(title);
    topAreaLayout->addWidget(contentWidget);
    topAreaLayout->addSpacing(10);
    topAreaLayout->addWidget(informationSequence);
    topAreaLayout->addWidget(decoderOutputSequence);
    topAreaLayout->addWidget(compareInformationAndDecoderOutput);
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

// void MainWindow::setupLabels() {



//     // Соединяем изменение спинбокса с обновлением лейбла
//     connect(spinCodeLength, QOverload<int>::of(&QSpinBox::valueChanged),
//             this, [this](int value) {
//                 labelCodeLength->setText(QString("💡 Длина кодового слова (n): %1").arg(value));
//             });

//     // Соединяем изменение спинбокса с обновлением лейбла
//     connect(spinInfoLength, QOverload<int>::of(&QSpinBox::valueChanged),
//             this, [this](int value) {
//                 labelInfoLength->setText(QString("💡 Длина кодового слова (n): %1").arg(value));
//             });


//     // Соединяем изменение спинбокса с обновлением лейбла
//     connect(spinСorrectingAbility, QOverload<int>::of(&QSpinBox::valueChanged),
//             this, [this](int value) {
//                 labelСorrectingAbility->setText(QString("💡 Длина кодового слова (n): %1").arg(value));
//             });


//     // Соединяем изменение спинбокса с обновлением лейбла
//     connect(spinInWordsfoLength, QOverload<int>::of(&QSpinBox::valueChanged),
//             this, [this](int value) {
//                 labelWordsfoLength->setText(QString("💡 Длина кодового слова (n): %1").arg(value));
//             });

// }

// void MainWindow::labelsVisibleFalse(bool labelslive) {

//     if (!labelslive) {
//         labelCodeLength->setVisible(false);
//         labelInfoLength->setVisible(false);
//         labelСorrectingAbility->setVisible(false);
//         labelWordsfoLength->setVisible(false);
//     } else {
//         labelCodeLength->setVisible(true);
//         labelInfoLength->setVisible(true);
//         labelСorrectingAbility->setVisible(true);
//         labelWordsfoLength->setVisible(true);
//     }

// }
