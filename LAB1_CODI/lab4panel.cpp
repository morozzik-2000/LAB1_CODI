#include "lab4panel.h"
#include "themestyles.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <algorithm>
#include <QHBoxLayout>
#include <QMenu>
#include <QFileDialog>
#include "manualplotdialog.h"

Lab4Panel::Lab4Panel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    QLabel *title = new QLabel("Кодек с искажающим двоичным симметричным каналом (ДСК)");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px; font-weight:bold;");

    QFormLayout *form = new QFormLayout;
    pSpin = new QDoubleSpinBox;
    pSpin->setRange(0.001, 1.0);
    pSpin->setSingleStep(0.001);
    pSpin->setValue(0.001);
    pSpin->setMaximumWidth(90);
    pSpin->setDecimals(3); // Показывать 3 знака после запятой
    pSpin->setStepType(QDoubleSpinBox::AdaptiveDecimalStepType); // Адаптивный шаг

    form->addRow("Вероятность канальной ошибки (pₖ):", pSpin);

    QGroupBox *buttonsGroup = new QGroupBox("Реализации");
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
    auto *ber_dk = new QPushButton("Зависимость BER\nна выходе декодера от\nвероятности ошибки в ДСК");

    for (auto *b : {infoSeq, decOut, cmp, ber_dk}) b->setStyleSheet(ThemeStyles::lightButtonStyle());

    QPushButton *runButton = new QPushButton("🚀 Запустить моделирование");
    runButton->setStyleSheet(ThemeStyles::OctaveButtonStyle());
    runButton->setFixedWidth(280);
    runButton->setCursor(Qt::PointingHandCursor);

    connect(runButton, &QPushButton::clicked, this, [=]() {
        emit runRequestedLab4();
    });


    gl->addWidget(infoSeq);
    gl->addWidget(decOut);
    gl->addWidget(cmp);
    glh->addWidget(ber_dk);

    connect(infoSeq, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Информационная последовательность");
        plotCsv("p4_encoded", "Информационная последовательность");
    });

    connect(decOut, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Последовательность на выходе декодера");
        plotCsv("p4_decoded", "Последовательность на выходе декодера");
    });

    connect(cmp, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Сравнение информационной и декодированной последовательности");
        plotCsv("p4_difference", "Сравнение информационной и декодированной последовательности");
    });

    // В MainWindow.cpp
    connect(ber_dk, &QPushButton::clicked, this, [=]() {
        emit logMessage("📈 Выбрано: Зависимость битовой ошибки на выходе декодера");

        // Если окно уже создано — просто показываем его снова
        if (!manualPlotDialog) {
            manualPlotDialog = new ManualPlotDialog(
                "Введите значения вероятности канальной ошибки (p<sub>k</sub>) и количество ошибок на выходе декодера:",
                "BER на выходе декодера",
                "Зависимость",
                this
                );

            manualPlotDialog->setAttribute(Qt::WA_DeleteOnClose, false); // не удалять при закрытии
            connect(manualPlotDialog, &QObject::destroyed, [this]() { manualPlotDialog = nullptr; });
        }

        manualPlotDialog->show();
        manualPlotDialog->raise();
        manualPlotDialog->activateWindow();
    });


    layout->addWidget(title);
    layout->addLayout(form);
    // layout->addWidget(buttonsGroup);
    // layout->addWidget(buttonsHandGroup);
    QHBoxLayout *groupsLayout = new QHBoxLayout;
    groupsLayout->addWidget(buttonsGroup);
    groupsLayout->addWidget(buttonsHandGroup);
    groupsLayout->setSpacing(16); // расстояние между блоками
    groupsLayout->setAlignment(Qt::AlignLeft); // чтобы они не растягивались на всю ширину

    buttonsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    buttonsHandGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    layout->addLayout(groupsLayout);
    layout->addWidget(runButton, 0, Qt::AlignLeft);
    layout->addStretch();
}

// ======== CSV чтение ========
QVector<double> Lab4Panel::readCsv(const QString &filePath)
{
    QVector<double> data;
    QFile file(QDir::currentPath() + "/results/" + filePath + ".csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return data;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        for (const QString &numStr : line.split(',')) {
            bool ok;
            double val = numStr.toDouble(&ok);
            if (ok) data.append(val);
        }
    }
    file.close();
    return data;
}

// ======== Построение графика ========
void Lab4Panel::plotCsv(const QString &fileName, const QString &title)
{
    QVector<double> data = readCsv(fileName);
    if (data.isEmpty()) return;

    QWidget *mainWindow = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWindow);
    QCustomPlot *plot = new QCustomPlot;
    mainWindow->setWindowTitle(title);

    // Заголовок
    QCPTextElement *titleElement = new QCPTextElement(plot, title, QFont("sans", 12, QFont::Bold));
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, titleElement);

    // Данные
    QVector<double> x(data.size());
    for (int i = 0; i < data.size(); ++i) x[i] = i + 1;

    plot->addGraph();
    plot->graph(0)->setData(x, data);
    plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
    plot->graph(0)->setPen(QPen(Qt::blue));

    plot->addGraph();
    plot->graph(1)->setData(x, data);
    plot->graph(1)->setLineStyle(QCPGraph::lsImpulse);
    plot->graph(1)->setPen(QPen(Qt::blue));

    plot->xAxis->setLabel("Индекс");
    plot->yAxis->setLabel("Значение");
    plot->xAxis->setRange(0, data.size() + 1);
    double min = *std::min_element(data.begin(), data.end());
    double max = *std::max_element(data.begin(), data.end());
    plot->yAxis->setRange(min - 1, max + 1);

    // Разрешаем масштабирование и перетаскивание по оси OX
    plot->setInteraction(QCP::iRangeZoom, true);
    plot->setInteraction(QCP::iRangeDrag, true);

    // Настраиваем, чтобы масштабировалась только ось X
    plot->axisRect()->setRangeZoom(Qt::Horizontal);
    plot->axisRect()->setRangeDrag(Qt::Horizontal);


    // Сохранение
    QPushButton *saveButton = new QPushButton("Сохранить");
    saveButton->setFixedSize(100, 30);
    saveButton->setStyleSheet(ThemeStyles::lightButtonStyle());

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);

    QMenu *saveMenu = new QMenu(saveButton);
    saveMenu->addAction("PNG", [=]() { savePlot(plot, "PNG"); });
    saveMenu->addAction("JPEG", [=]() { savePlot(plot, "JPEG"); });
    saveMenu->addAction("PDF", [=]() { savePlot(plot, "PDF"); });

    connect(saveButton, &QPushButton::clicked, this, [=]() {
        saveMenu->exec(saveButton->mapToGlobal(QPoint(0, saveButton->height())));
    });

    mainLayout->addWidget(plot);
    mainLayout->addLayout(buttonLayout);

    plot->replot();
    mainWindow->resize(800, 600);
    mainWindow->show();
}

// ======== Сохранение ========
void Lab4Panel::savePlot(QCustomPlot *plot, const QString &format)
{
    QWidget *parent = plot->parentWidget();
    if (!parent) parent = plot;

    QString fileName = QFileDialog::getSaveFileName(
        parent, "Сохранить график как",
        QDir::homePath() + "/" + parent->windowTitle(),
        getFileFilter(format));

    if (fileName.isEmpty()) return;

    bool success = false;
    int w = plot->width(), h = plot->height();
    if (format == "PNG") success = plot->savePng(fileName, w, h);
    else if (format == "JPEG") success = plot->saveJpg(fileName, w, h);
    else if (format == "PDF") success = plot->savePdf(fileName, w, h);

    if (success)
        emit logMessage(QString("График сохранен как: %1").arg(fileName));
    else
        emit logMessage("Ошибка при сохранении графика");
}

QString Lab4Panel::getFileFilter(const QString &format)
{
    if (format == "PNG") return "PNG Files (*.png)";
    if (format == "JPEG") return "JPEG Files (*.jpg *.jpeg)";
    if (format == "PDF") return "PDF Files (*.pdf)";
    return "All Files (*)";
}

QString Lab4Panel::getFileExtension(const QString &format)
{
    if (format == "PNG") return ".png";
    if (format == "JPEG") return ".jpg";
    if (format == "PDF") return ".pdf";
    return "";
}

OctaveParams_ Lab4Panel::getParams() const
{
    OctaveParams_ p;
    p.n = 127;
    p.k = 64;
    p.t = 10;
    p.numWords = 100;
    p.channelErrorProbability = pSpin->value();
    return p;
}
