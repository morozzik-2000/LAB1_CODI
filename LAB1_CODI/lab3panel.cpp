#include "Lab3Panel.h"
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
#include "octaveparamspart2.h"
#include "manualplotdialog.h"

Lab3Panel::Lab3Panel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    QLabel *title = new QLabel("Двоичный симметричный канал (ДСК)");
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
    buttonsGroup->setMaximumWidth(510);
    QVBoxLayout *gl = new QVBoxLayout(buttonsGroup);

    QGroupBox *buttonsHandGroup = new QGroupBox("Построить вручную");
    buttonsHandGroup->setStyleSheet(ThemeStyles::lightGroupBoxStyle());
    buttonsHandGroup->setMaximumWidth(300);
    QVBoxLayout *glh = new QVBoxLayout(buttonsHandGroup);

    auto *outEnc = new QPushButton("Кодовая последовательность на выходе кодера");
    auto *inDec = new QPushButton("Последовательность на входе декодера");
    auto *errVec = new QPushButton("Вектор ошибок");

    auto *ber_dk = new QPushButton("Зависимость BER\nна входе декодера от\nвероятности ошибки в ДСК");

    QPushButton *runButton = new QPushButton("🚀 Запустить моделирование");
    runButton->setStyleSheet(ThemeStyles::OctaveButtonStyle());
    runButton->setFixedWidth(280);
    runButton->setCursor(Qt::PointingHandCursor);

    connect(runButton, &QPushButton::clicked, this, [=]() {
        emit runRequestedLab3();
    });


    for (auto *b : {outEnc, inDec, errVec, ber_dk}) b->setStyleSheet(ThemeStyles::lightButtonStyle());
    gl->addWidget(outEnc);
    gl->addWidget(inDec);
    gl->addWidget(errVec);

    glh->addWidget(ber_dk);

    connect(outEnc, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Кодовая последовательность выходе кодера");
        plotCsv("p3_encoded"); // CSV должен сохраняться в OctaveRunnerPart2
    });
    connect(inDec, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Последовательность входе декодера");
        plotCsv("p3_received");
    });
    connect(errVec, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Вектор ошибок");
        plotCsv("p3_error_vector");
    });

    // В MainWindow.cpp
    connect(ber_dk, &QPushButton::clicked, this, [=]() {
        emit logMessage("📈 Выбрано: Зависимость битовой ошибки на входе декодера");

        // Если окно уже создано — просто показываем его снова
        // Если окно уже создано — просто показываем его снова
        if (!manualPlotDialog) {
            manualPlotDialog = new ManualPlotDialog(
                "Введите значения вероятности канальной ошибки (p<sub>k</sub>) и количество ошибок на входе декодера:",
                "BER на входе декодера",
                "Зависимость",
                nullptr  // Важно! Устанавливаем nullptr вместо this
                );

            // Настройки окна для правильного поведения
            manualPlotDialog->setAttribute(Qt::WA_DeleteOnClose, false);
            manualPlotDialog->setWindowFlags(manualPlotDialog->windowFlags() | Qt::Window); // Убеждаемся, что это независимое окно

            connect(manualPlotDialog, &QObject::destroyed, [this]() { manualPlotDialog = nullptr; });
        }

        // Показываем и активируем окно
        emit requestLab1Params();
        manualPlotDialog->show();
        manualPlotDialog->raise();
        manualPlotDialog->activateWindow();

        // Если окно было свернуто, разворачиваем его
        if (manualPlotDialog->windowState() & Qt::WindowMinimized) {
            manualPlotDialog->setWindowState(manualPlotDialog->windowState() & ~Qt::WindowMinimized);
        }

    });



    layout->addWidget(title);
    layout->addLayout(form);
    QHBoxLayout *groupsLayout = new QHBoxLayout;
    groupsLayout->addWidget(buttonsGroup);
    groupsLayout->addWidget(buttonsHandGroup);
    groupsLayout->setSpacing(16); // расстояние между блоками
    groupsLayout->setAlignment(Qt::AlignLeft); // чтобы они не растягивались на всю ширину

    buttonsGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    buttonsHandGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    layout->addLayout(groupsLayout);
    layout->addWidget(runButton, 0, Qt::AlignLeft);

    // layout->addWidget(buttonsGroup);
    // layout->addWidget(buttonsHandGroup);
    layout->addStretch();
}

QVector<double> Lab3Panel::readCsv(const QString &filePath)
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

void Lab3Panel::plotCsv(const QString &fileName)
{
    QVector<double> data = readCsv(fileName);
    if (data.isEmpty()) return;

    // Создаем основное окно
    QWidget *mainWindow = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWindow);
    mainLayout->setSpacing(5);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // Создаем график
    QCustomPlot *plot = new QCustomPlot;

    // Устанавливаем заголовок окна и графика
    QString windowTitle;
    QString graphTitle;
    if (fileName == "p3_encoded") {
        windowTitle = "Кодовая последовательность на выходе кодера";
        graphTitle = "Кодовая последовательность на выходе кодера";
    } else if (fileName == "p3_received") {
        windowTitle = "Последовательность на входе декодера";
        graphTitle = "Последовательность на входе декодера";
    } else if (fileName == "p3_error_vector") {
        windowTitle = "Вектор ошибок";
        graphTitle = "Вектор ошибок";
    } else {
        windowTitle = "График";
        graphTitle = "График";
    }
    mainWindow->setWindowTitle(windowTitle);

    // Добавляем заголовок прямо на график
    QCPTextElement *titleElement = new QCPTextElement(plot, graphTitle, QFont("sans", 10, QFont::Bold));
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, titleElement);

    // Создаем данные для оси X
    QVector<double> x(data.size());
    for (int i = 0; i < data.size(); ++i) {
        x[i] = i + 1;
    }

    // Сохраняем исходные диапазоны для сброса масштаба
    double xMin = 0;
    double xMax = data.size() + 1;
    double yMin = *std::min_element(data.begin(), data.end()) - 1;
    double yMax = *std::max_element(data.begin(), data.end()) + 1;

    // Добавляем график точек
    plot->addGraph();
    plot->graph(0)->setData(x, data);
    plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    plot->graph(0)->setPen(QPen(Qt::blue));

    // Добавляем вертикальные линии (stem)
    plot->addGraph();
    plot->graph(1)->setData(x, data);
    plot->graph(1)->setLineStyle(QCPGraph::lsImpulse);
    plot->graph(1)->setPen(QPen(Qt::blue));

    // Разрешаем масштабирование и перетаскивание по оси OX
    plot->setInteraction(QCP::iRangeZoom, true);
    plot->setInteraction(QCP::iRangeDrag, true);

    // Настраиваем, чтобы масштабировалась только ось X
    plot->axisRect()->setRangeZoom(Qt::Horizontal);
    plot->axisRect()->setRangeDrag(Qt::Horizontal);

    // Настраиваем оси
    plot->xAxis->setLabel("Индекс");
    plot->yAxis->setLabel("Значение");
    plot->xAxis->setRange(xMin, xMax);
    plot->yAxis->setRange(yMin, yMax);

    // Добавляем обработчик двойного клика для сброса масштаба
    connect(plot, &QCustomPlot::mouseDoubleClick, [=](QMouseEvent *event) {
        Q_UNUSED(event);
        plot->xAxis->setRange(xMin, xMax);
        plot->yAxis->setRange(yMin, yMax);
        plot->replot();
    });

    // Добавляем график в layout с растяжением
    mainLayout->addWidget(plot, 1);

    // Создаем компактный layout для кнопок внизу
    QHBoxLayout *bottomButtonsLayout = new QHBoxLayout();
    bottomButtonsLayout->setContentsMargins(0, 2, 0, 2);
    bottomButtonsLayout->setSpacing(10);

    // Создаем информационную кнопку с восклицательным знаком
    QPushButton *infoButton = new QPushButton("ⓘ");
    infoButton->setFixedSize(28, 28);
    infoButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #0078D7;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 14px;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #005A9E;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #004080;"
        "}"
        );

    // Создаем кнопку сохранения
    QPushButton *saveButton = new QPushButton("Сохранить");
    saveButton->setFixedSize(100, 28);
    saveButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #28a745;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #218838;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1e7e34;"
        "}"
        );

    // Добавляем растяжение слева, чтобы кнопки были справа
    bottomButtonsLayout->addStretch();
    bottomButtonsLayout->addWidget(infoButton);
    bottomButtonsLayout->addWidget(saveButton);

    // Создаем меню для сохранения
    QMenu *saveMenu = new QMenu(saveButton);
    saveMenu->addAction("PNG", [=]() { savePlot(plot, "PNG"); });
    saveMenu->addAction("JPEG", [=]() { savePlot(plot, "JPEG"); });
    saveMenu->addAction("PDF", [=]() { savePlot(plot, "PDF"); });

    connect(saveButton, &QPushButton::clicked, this, [=]() {
        saveMenu->exec(saveButton->mapToGlobal(QPoint(0, saveButton->height())));
    });

    // Создаем информационное сообщение
    QMessageBox *infoMessage = new QMessageBox(mainWindow);
    infoMessage->setWindowTitle("Управление графиком");
    infoMessage->setIcon(QMessageBox::Information);
    infoMessage->setText(
        "<h3>Управление графиком</h3>"
        "<p><b>Масштабирование:</b></p>"
        "<ul>"
        "<li>Колесико мыши - приближение/отдаление по оси X</li>"
        "<li>Можно масштабировать только по горизонтали</li>"
        "</ul>"
        "<p><b>Перемещение:</b></p>"
        "<ul>"
        "<li>Зажмите левую кнопку мыши и перетаскивайте график</li>"
        "<li>Перемещение возможно только по горизонтали</li>"
        "</ul>"
        "<p><b>Сброс масштаба:</b></p>"
        "<ul>"
        "<li>Двойной клик по графику</li>"
        "</ul>"
        "<p><b>Дополнительно:</b></p>"
        "<ul>"
        "<li>Кнопка 'Сохранить' - экспорт графика в PNG, JPEG или PDF</li>"
        "</ul>"
        );

    connect(infoButton, &QPushButton::clicked, [=]() {
        infoMessage->show();
    });

    // Добавляем компактный layout с кнопками
    mainLayout->addLayout(bottomButtonsLayout);

    // Перерисовываем график
    plot->replot();

    mainWindow->resize(800, 600);
    mainWindow->show();
}

// ===== Функция сохранения графика =====
void Lab3Panel::savePlot(QCustomPlot *plot, const QString &format)
{
    QWidget *parent = plot->parentWidget();
    if (!parent) parent = plot;

    QString fileName = QFileDialog::getSaveFileName(parent,
                                                    "Сохранить график как",
                                                    QDir::homePath() + "/" + parent->windowTitle(),
                                                    getFileFilter(format));

    if (!fileName.isEmpty()) {
        bool success = false;
        int width = plot->width();
        int height = plot->height();

        if (format == "PNG") {
            success = plot->savePng(fileName, width, height);
        } else if (format == "JPEG") {
            success = plot->saveJpg(fileName, width, height);
        } else if (format == "PDF") {
            success = plot->savePdf(fileName, width, height);
        }

        if (success) {
            emit logMessage(QString("График сохранен как: %1").arg(fileName));
        } else {
            emit logMessage("Ошибка при сохранении графика");
        }
    }
}

// ===== Функция получения фильтра файлов =====
QString Lab3Panel::getFileFilter(const QString &format)
{
    if (format == "PNG") return "PNG Files (*.png)";
    if (format == "JPEG") return "JPEG Files (*.jpg *.jpeg)";
    if (format == "PDF") return "PDF Files (*.pdf)";
    return "All Files (*)";
}

// ===== Функция получения расширения файла =====
QString Lab3Panel::getFileExtension(const QString &format)
{
    if (format == "PNG") return ".png";
    if (format == "JPEG") return ".jpg";
    if (format == "PDF") return ".pdf";
    return "";
}

OctaveParams_ Lab3Panel::getParams() const
{
    OctaveParams_ params;
    params.n = 127; // Можно задать базовые значения или считать их из Lab1Panel
    params.k = 64;
    params.t = 10;
    params.numWords = 100;
    params.channelErrorProbability = pSpin->value();
    return params;
}
