#include "MainWindow.h"
#include "ThemeStyles.h"


#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QApplication>
#include <QDebug>
#include "octaveparamspart2.h"
#include "octaverunnerpart3.h"


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Коды исправляющие ошибки");
    resize(1200, 800);

    // main layout
    auto *mainLayout = new QHBoxLayout(this);

    // left panel
    mainLayout->addWidget(createLeftPanel(), 1);

    // right: top area + log
    QWidget *rightPanel = new QWidget;
    auto *rightLayout = new QVBoxLayout(rightPanel);

    // top area will contain stacked panels
    QWidget *topArea = createTopArea();


    rightLayout->addWidget(topArea, 1);

    // log panel
    auto *logGroup = new QGroupBox("📝 Окно сообщений");
    logGroup->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    auto *logLayout = new QVBoxLayout(logGroup);

    logTextEdit = new QTextEdit;
    logTextEdit->setReadOnly(true);
    logTextEdit->setStyleSheet(ThemeStyles::logTextEdit());

    QPushButton *clearLog = new QPushButton("🗑 Очистить окно сообщений");
    clearLog->setStyleSheet(ThemeStyles::lightButtonStyle());
    qDebug() << "Style" << ThemeStyles::lightButtonStyle();
    connect(clearLog, &QPushButton::clicked, logTextEdit, &QTextEdit::clear);

    logLayout->addWidget(logTextEdit);
    logLayout->addWidget(clearLog);

    rightLayout->addWidget(logGroup, 40);

    mainLayout->addWidget(rightPanel, 4);

    QFont font = qApp->font();
    font.setPointSize(11);
    qApp->setFont(font);

}

QWidget* MainWindow::createLeftPanel()
{
    auto *leftGroupBox = new QGroupBox("📡 Управление");

    leftGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    auto *mainLeftLayout = new QVBoxLayout(leftGroupBox);
    mainLeftLayout->setSpacing(10);

    // first group: params
    QGroupBox *firstGroupBox = new QGroupBox("Параметры кода");
    firstGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    auto *firstLayout = new QVBoxLayout(firstGroupBox);
    btnLab1 = new QPushButton("Задать параметры кода");
    btnLab1->setStyleSheet(ThemeStyles::lightButtonStyle());
    firstLayout->addWidget(btnLab1);

    // modelling group
    QGroupBox *secondGroupBox = new QGroupBox("Моделирование");
    secondGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    auto *secondLayout = new QVBoxLayout(secondGroupBox);

    btnLab2 = new QPushButton("Модель кодека с\n неискажающим каналом");
    btnLab3 = new QPushButton("Модель канала ДСК");
    btnLab4 = new QPushButton("Модель кодека с\n искажающим ДСК");
    btnLab2->setStyleSheet(ThemeStyles::lightButtonStyle());
    btnLab3->setStyleSheet(ThemeStyles::lightButtonStyle());
    btnLab4->setStyleSheet(ThemeStyles::lightButtonStyle());

    secondLayout->addWidget(btnLab2);
    secondLayout->addWidget(btnLab3);
    secondLayout->addWidget(btnLab4);

    // tools
    QGroupBox *thirdGroupBox = new QGroupBox("Инструменты");
    thirdGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    auto *thirdLayout = new QVBoxLayout(thirdGroupBox);
    QPushButton *saveLog = new QPushButton("Сохранить историю окна сообщений");
    saveLog->setStyleSheet(ThemeStyles::lightButtonStyle());
    // QPushButton *toggleTheme = new QPushButton("Сменить тему");
    // toggleTheme->setStyleSheet(ThemeStyles::lightButtonStyle());
    thirdLayout->addWidget(saveLog);
    // thirdLayout->addWidget(toggleTheme);

    mainLeftLayout->addWidget(firstGroupBox);
    mainLeftLayout->addWidget(secondGroupBox);
    mainLeftLayout->addWidget(thirdGroupBox);
    mainLeftLayout->addStretch();

    // connect left buttons to slots
    connect(btnLab1, &QPushButton::clicked, this, &MainWindow::onLab1Clicked);
    connect(btnLab2, &QPushButton::clicked, this, &MainWindow::onLab2Clicked);
    connect(btnLab3, &QPushButton::clicked, this, &MainWindow::onLab3Clicked);
    connect(btnLab4, &QPushButton::clicked, this, &MainWindow::onLab4Clicked);
    connect(saveLog, &QPushButton::clicked, this, &MainWindow::saveLogToFile);


    return leftGroupBox;
}

QWidget* MainWindow::createTopArea()
{
    // Групбокс для TopArea
    topGroupBox = new QGroupBox("⚙ Рабочая область");
    topGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());


    auto *layout = new QVBoxLayout(topGroupBox);

    // // Кнопка Run
    // runOctaveButton = new QPushButton("Запустить моделирование");
    // runOctaveButton->setMaximumWidth(240);
    // runOctaveButton->setStyleSheet(ThemeStyles::OctaveButtonStyle());
    // runOctaveButton->hide();


    // QStackedWidget с панелями
    stack = new QStackedWidget;

    p1 = new Lab1Panel;
    p2 = new Lab2Panel;
    p3 = new Lab3Panel;
    p4 = new Lab4Panel;

    connect(p1, &Lab1Panel::logMessage, this, &MainWindow::appendLog);
    connect(p2, &Lab2Panel::logMessage, this, &MainWindow::appendLog);
    connect(p3, &Lab3Panel::logMessage, this, &MainWindow::appendLog);
    connect(p4, &Lab4Panel::logMessage, this, &MainWindow::appendLog);


    connect(p2, &Lab2Panel::runRequestedLab2, this, &MainWindow::startModeling);
    connect(p3, &Lab3Panel::runRequestedLab3, this, &MainWindow::startModeling);
    connect(p4, &Lab4Panel::runRequestedLab4, this, &MainWindow::startModeling);

    stack->addWidget(p1);
    stack->addWidget(p2);
    stack->addWidget(p3);
    stack->addWidget(p4);
    stack->setCurrentIndex(0);

    connect(stack, &QStackedWidget::currentChanged, this, &MainWindow::onStackIndexChanged);
    // connect(runOctaveButton, &QPushButton::clicked, this, [=]() {
    //     int current = stack->currentIndex();

    //     // Берём параметры из первой панели (Lab1Panel)
    //     Lab1Panel *p1 = qobject_cast<Lab1Panel*>(stack->widget(0));
    //     if (!p1) {
    //         appendLog("⚠ Не удалось получить параметры из ЛР1!");
    //         return;
    //     }

    //     OctaveParams_ baseParams = p1->getParams();

    //     appendLog("▶ Запуск моделирования с параметрами:");
    //     appendLog(QString("n=%1, k=%2, t=%3, numWords=%4, p=%5")
    //                   .arg(baseParams.n)
    //                   .arg(baseParams.k)
    //                   .arg(baseParams.t)
    //                   .arg(baseParams.numWords)
    //                   .arg(baseParams.channelErrorProbability));

    //     // === Определяем, какую часть запускать ===
    //     if (current == 1) { // --- ЛР2 ---
    //         appendLog("🔹 Запуск: Модель кодека с неискажающим каналом (ЛР2)");

    //         auto *runner = new OctaveRunner(this);
    //         connect(runner, &OctaveRunner::logMessage, this, &MainWindow::appendLog);
    //         connect(runner, &OctaveRunner::finished, this, [=]() { appendLog("✅ Часть 1 завершена."); });

    //         runner->runOctave(baseParams);
    //     }

    //     else if (current == 2) { // --- ЛР3 ---
    //         appendLog("🔹 Запуск: Модель двоичного симметричного канала (ЛР3)");

    //         // Вероятность ошибки из Lab3Panel
    //         Lab3Panel *p3 = qobject_cast<Lab3Panel*>(stack->widget(2));
    //         OctaveParams_ params;
    //         params.n = baseParams.n;
    //         params.k = baseParams.k;
    //         params.t = baseParams.t;
    //         params.numWords = baseParams.numWords;
    //         params.channelErrorProbability = p3->getParams().channelErrorProbability;

    //         appendLog(QString("📊 Параметры ЛР3: n=%1, k=%2, t=%3, numWords=%4, p=%5")
    //                       .arg(params.n).arg(params.k).arg(params.t)
    //                       .arg(params.numWords).arg(params.channelErrorProbability));

    //         auto *runner = new OctaveRunnerPart2(this);
    //         connect(runner, &OctaveRunnerPart2::logMessage, this, &MainWindow::appendLog);
    //         connect(runner, &OctaveRunnerPart2::finished, this, [=]() { appendLog("✅ Часть 2 завершена."); });

    //         runner->runOctave(params);
    //     }


    //     else if (current == 3) { // --- ЛР4 ---
    //         appendLog("🔹 Запуск: Кодек с искажающим ДСК (ЛР4)");

    //         Lab4Panel *p4 = qobject_cast<Lab4Panel*>(stack->widget(3));
    //         OctaveParams_ params;
    //         params.n = baseParams.n;
    //         params.k = baseParams.k;
    //         params.t = baseParams.t;
    //         params.numWords = baseParams.numWords;
    //         params.channelErrorProbability = p4->getParams().channelErrorProbability;

    //         appendLog(QString("📊 Параметры ЛР3: n=%1, k=%2, t=%3, numWords=%4, p=%5")
    //                       .arg(params.n).arg(params.k).arg(params.t)
    //                       .arg(params.numWords).arg(params.channelErrorProbability));

    //         auto *runner = new OctaveRunnerPart3(this);
    //         connect(runner, &OctaveRunnerPart3::logMessage, this, &MainWindow::appendLog);
    //         connect(runner, &OctaveRunnerPart3::finished, this, [=]() { appendLog("✅ Часть 3 завершена."); });

    //         runner->runOctave(params);
    //     }

    //     else {
    //         appendLog("ℹ Выберите панель моделирования (ЛР2, ЛР3 или ЛР4) перед запуском.");
    //     }
    // });


    layout->addWidget(stack);
    // layout->addStretch();
    // layout->addWidget(runOctaveButton, 0, Qt::AlignTop);

    return topGroupBox;
}


void MainWindow::onLab1Clicked() { stack->setCurrentIndex(0); }
void MainWindow::onLab2Clicked() { stack->setCurrentIndex(1); }
void MainWindow::onLab3Clicked() { stack->setCurrentIndex(2); }
void MainWindow::onLab4Clicked() { stack->setCurrentIndex(3); }

void MainWindow::onStackIndexChanged(int idx)
{
    QString name;
    // bool showRun = false;
    switch (idx) {
    case 0:
        name = "Задать параметры кода";
        // showRun = false;
        topGroupBox->setMaximumHeight(500);
        break;
    case 1:
        name = "Модель кодека с неискажающим каналом";
        // showRun = true;
        topGroupBox->setMaximumHeight(440);
        break;
    case 2:
        name = "Модель канала ДСК";
        // showRun = true;
        topGroupBox->setMaximumHeight(350);
        break;
    case 3:
        name = "Модель кодека с искажающим ДСК";
        // showRun = true;
        topGroupBox->setMaximumHeight(370);
        break;
    default:
        name = "Unknown";
        topGroupBox->setMaximumHeight(400);
        // showRun = false;
    }

    appendHeader(name);

   /* runOctaveButton->setVisible(showRun);*/ // контролируем видимость здесь
}

void MainWindow::appendHeader(const QString &title)
{
    // Добавляем в историю только ТЕКСТ
    logHistory << title;

    // А в окно — форматированную версию
    logTextEdit->append(QString("<div style='text-align:center;'><b>%1</b></div>").arg(title));
    logTextEdit->moveCursor(QTextCursor::End);
}


void MainWindow::appendLog(const QString &msg)
{
    // Игнорируем предупреждение про UTF-8
    if (msg.contains("warning: Invalid UTF-8 byte sequences have been replaced."))
        return;

    // Проверяем специфическую ошибку BCH
    if (msg.contains("bchenco: can not find valid generator polynomial for parameters", Qt::CaseInsensitive)) {
        logTextEdit->append("❌ Неверные параметры кода");
        return;
    }

    // 3️⃣ Обработка QQQ и TTT
    if (msg.contains("QQQ:")) {
        // Извлекаем число после QQQ:
        QRegularExpression re("QQQ:\\s*(\\d+)");
        QRegularExpressionMatch match = re.match(msg);
        if (match.hasMatch()) {
            QString num = match.captured(1);
            logTextEdit->append("⚠ Количество ошибок в канале: " + num);
        }
        return;
    }
    if (msg.trimmed() == "0") {
        logTextEdit->append("⚠ Количество ошибок в канале: 0");
        return;
    }
    // Все остальные сообщения выводим как есть
    logHistory << msg;
    logTextEdit->append(msg);
    logTextEdit->moveCursor(QTextCursor::End);

}


void MainWindow::startModeling()
{
    int current = stack->currentIndex();

    // Берём базовые параметры из первой панели
    Lab1Panel *p1 = qobject_cast<Lab1Panel*>(stack->widget(0));
    if (!p1) {
        appendLog("⚠ Не удалось получить параметры из ЛР1!");
        return;
    }

    OctaveParams_ baseParams = p1->getParams();


    if (current == 1) {
        appendLog("▶️ Запуск моделирования с параметрами:");
        appendLog(QString("𝐧 = %1, 𝐤 = %2, 𝐭 = %3, количество слов = %4")
                      .arg(baseParams.n)
                      .arg(baseParams.k)
                      .arg(baseParams.t)
                      .arg(baseParams.numWords));
        appendLog("🔹 Пункт №1: Неискажающий канал");
        auto *runner = new OctaveRunner(this);
        connect(runner, &OctaveRunner::logMessage, this, &MainWindow::appendLog);
        connect(runner, &OctaveRunner::finished, this, [=]() { appendLog("✅ Моделирование завершено."); });
        runner->runOctave(baseParams);
    }
    else if (current == 2) {
        appendLog("🔹 Пункт №2: Двоичный симметричный канал");
        Lab3Panel *p3 = qobject_cast<Lab3Panel*>(stack->widget(2));
        OctaveParams_ params = baseParams;
        params.channelErrorProbability = p3->getParams().channelErrorProbability;

        appendLog("▶️ Запуск моделирования с параметрами:");
        appendLog(QString("𝐧 = %1, 𝐤 = %2, 𝐭 = %3, количество слов = %4, 𝐩 = %5")
                      .arg(baseParams.n)
                      .arg(baseParams.k)
                      .arg(baseParams.t)
                      .arg(baseParams.numWords)
                      .arg(params.channelErrorProbability));

        auto *runner = new OctaveRunnerPart2(this);
        connect(runner, &OctaveRunnerPart2::logMessage, this, &MainWindow::appendLog);
        connect(runner, &OctaveRunnerPart2::finished, this, [=]() { appendLog("✅ Моделирование завершено."); });
        runner->runOctave(params);
    }
    else if (current == 3) {
        appendLog("🔹 Пункт №3: Кодек с искажающим ДСК");
        Lab4Panel *p4 = qobject_cast<Lab4Panel*>(stack->widget(3));
        OctaveParams_ params = baseParams;
        params.channelErrorProbability = p4->getParams().channelErrorProbability;

        appendLog("▶️ Запуск моделирования с параметрами:");
        appendLog(QString("𝐧 = %1, 𝐤 = %2, 𝐭 = %3, количество слов = %4, 𝐩 = %5")
                      .arg(baseParams.n)
                      .arg(baseParams.k)
                      .arg(baseParams.t)
                      .arg(baseParams.numWords)
                      .arg(params.channelErrorProbability));

        auto *runner = new OctaveRunnerPart3(this);
        connect(runner, &OctaveRunnerPart3::logMessage, this, &MainWindow::appendLog);
        connect(runner, &OctaveRunnerPart3::finished, this, [=]() { appendLog("✅ Моделирование завершено."); });
        runner->runOctave(params);
    }
    else {
        appendLog("ℹ Выберите панель моделирования (ЛР2, ЛР3, ЛР4).");
    }
}

void MainWindow::saveLogToFile()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Сохранить лог",
        "log.txt",
        "Текстовые файлы (*.txt)"
        );

    if (filename.isEmpty())
        return;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out.setCodec("UTF-8");

    for (const QString &line : logHistory) {
        out << line << "\n";
    }

    file.close();

    appendLog("💾 Лог сохранён в файл: " + filename);
}
