#include "MainWindow.h"
#include "ThemeStyles.h"

#include "Lab1Panel.h"
#include "Lab2Panel.h"
#include "Lab3Panel.h"
#include "Lab4Panel.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QApplication>
#include <QDebug>

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
    rightLayout->addWidget(topArea, 2);

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

    rightLayout->addWidget(logGroup, 1);

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
    QPushButton *toggleTheme = new QPushButton("Сменить тему");
    toggleTheme->setStyleSheet(ThemeStyles::lightButtonStyle());
    thirdLayout->addWidget(saveLog);
    thirdLayout->addWidget(toggleTheme);

    mainLeftLayout->addWidget(firstGroupBox);
    mainLeftLayout->addWidget(secondGroupBox);
    mainLeftLayout->addWidget(thirdGroupBox);
    mainLeftLayout->addStretch();

    // connect left buttons to slots
    connect(btnLab1, &QPushButton::clicked, this, &MainWindow::onLab1Clicked);
    connect(btnLab2, &QPushButton::clicked, this, &MainWindow::onLab2Clicked);
    connect(btnLab3, &QPushButton::clicked, this, &MainWindow::onLab3Clicked);
    connect(btnLab4, &QPushButton::clicked, this, &MainWindow::onLab4Clicked);

    return leftGroupBox;
}

QWidget* MainWindow::createTopArea()
{
    // Групбокс для TopArea
    QGroupBox *topGroupBox = new QGroupBox("⚙ Рабочая область");
    topGroupBox->setStyleSheet(ThemeStyles::lightGroupBoxStyle());

    auto *layout = new QVBoxLayout(topGroupBox);

    // Кнопка Run
    runOctaveButton = new QPushButton("Запустить моделирование");
    runOctaveButton->setMaximumWidth(240);
    runOctaveButton->setStyleSheet(ThemeStyles::OctaveButtonStyle());
    runOctaveButton->hide();


    // QStackedWidget с панелями
    stack = new QStackedWidget;

    auto *p1 = new Lab1Panel;
    auto *p2 = new Lab2Panel;
    auto *p3 = new Lab3Panel;
    auto *p4 = new Lab4Panel;

    connect(p1, &Lab1Panel::logMessage, this, &MainWindow::appendLog);
    connect(p2, &Lab2Panel::logMessage, this, &MainWindow::appendLog);
    connect(p3, &Lab3Panel::logMessage, this, &MainWindow::appendLog);
    connect(p4, &Lab4Panel::logMessage, this, &MainWindow::appendLog);

    stack->addWidget(p1);
    stack->addWidget(p2);
    stack->addWidget(p3);
    stack->addWidget(p4);
    stack->setCurrentIndex(0);

    connect(stack, &QStackedWidget::currentChanged, this, &MainWindow::onStackIndexChanged);

    octRunner = new OctaveRunner(this);
    connect(runOctaveButton, &QPushButton::clicked, this, [=](){
        Lab1Panel *p1 = qobject_cast<Lab1Panel*>(stack->widget(0));
        OctaveParams params = p1->getParams();
        octRunner->runOctave(params);
    });
    connect(octRunner, &OctaveRunner::logMessage, this, &MainWindow::appendLog);
    connect(octRunner, &OctaveRunner::errorOccurred, this, &MainWindow::appendLog);
    connect(octRunner, &OctaveRunner::finished, this, [=](){ appendLog("Octave finished!"); });


    layout->addWidget(stack);
    layout->addStretch();
    layout->addWidget(runOctaveButton, 0, Qt::AlignTop);

    return topGroupBox;
}


void MainWindow::onLab1Clicked() { stack->setCurrentIndex(0); }
void MainWindow::onLab2Clicked() { stack->setCurrentIndex(1); }
void MainWindow::onLab3Clicked() { stack->setCurrentIndex(2); }
void MainWindow::onLab4Clicked() { stack->setCurrentIndex(3); }

void MainWindow::onStackIndexChanged(int idx)
{
    QString name;
    bool showRun = false;
    switch (idx) {
    case 0:
        name = "Задать параметры кода";
        showRun = false;
        break;
    case 1:
        name = "Модель кодека с неискажающим каналом";
        showRun = true;
        break;
    case 2:
        name = "Модель канала ДСК";
        showRun = true;
        break;
    case 3:
        name = "Модель кодека с искажающим ДСК";
        showRun = true;
        break;
    default:
        name = "Unknown";
        showRun = false;
    }

    appendLog("Выбрана панель: " + name);
    runOctaveButton->setVisible(showRun); // контролируем видимость здесь
}


void MainWindow::appendLog(const QString &msg)
{
    logTextEdit->append(msg);
}
