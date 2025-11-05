#include "Lab2Panel.h"
#include "themestyles.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <algorithm>
#include <QHBoxLayout>
#include <QMenu>
#include <QFileDialog>

Lab2Panel::Lab2Panel(QWidget *parent) : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    QLabel *title = new QLabel("Неискажающий канал");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px; font-weight:bold;");

    QGroupBox *buttonsGroup = new QGroupBox("Реализации");
    buttonsGroup->setMaximumWidth(420);
    buttonsGroup->setStyleSheet(ThemeStyles::lightGroupBoxStyle());

    QVBoxLayout *gl = new QVBoxLayout(buttonsGroup);
    auto *inputWord = new QPushButton("Входное слово");
    auto *decoded = new QPushButton("Декодированное слово");
    auto *codeSeq = new QPushButton("Кодовая последовательность");
    auto *compare = new QPushButton("Сравнение входной и\nдекодированной последовательностей");
    auto *three = new QPushButton("Три на одном");

    for (auto *b : {inputWord, decoded, codeSeq, compare, three})
        b->setStyleSheet(ThemeStyles::lightButtonStyle());

    QPushButton *runButton = new QPushButton("🚀 Запустить моделирование");
    runButton->setStyleSheet(ThemeStyles::OctaveButtonStyle());
    runButton->setFixedWidth(280);
    runButton->setCursor(Qt::PointingHandCursor);



    gl->addWidget(inputWord);
    gl->addWidget(decoded);
    gl->addWidget(codeSeq);
    gl->addWidget(compare);
    gl->addWidget(three);

    layout->addWidget(title);
    layout->addWidget(buttonsGroup);
    layout->addWidget(runButton, 0, Qt::AlignLeft);
    layout->addStretch();

    connect(runButton, &QPushButton::clicked, this, [=]() {
        emit runRequestedLab2();
    });


    // ===== Подключаем кнопки к чтению CSV =====
    connect(inputWord, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Входное слово");
        plotCsv("p2_random");
    });
    connect(decoded, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Декодированное слово");
        plotCsv("p2_decoded");
    });
    connect(codeSeq, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Кодовая последовательность");
        plotCsv("p2_encoded");
    });
    connect(compare, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Сравнение последовательностей");
        plotCsv("p2_compare");
    });
    connect(three, &QPushButton::clicked, this, [=](){
        emit logMessage("🔍Показано: Три на одном");

        // Загружаем данные
        QVector<double> input = readCsv("p2_random");
        QVector<double> encoded = readCsv("p2_encoded");
        QVector<double> decoded = readCsv("p2_decoded");

        if(input.isEmpty() || encoded.isEmpty() || decoded.isEmpty()) {
            emit logMessage("Ошибка: Не удалось загрузить один из файлов данных");
            return;
        }

        // Создаем основное окно с вертикальным layout
        QWidget *mainWindow = new QWidget;
        mainWindow->setWindowTitle("Три последовательности");
        mainWindow->resize(800, 1200);

        QVBoxLayout *mainLayout = new QVBoxLayout(mainWindow);
        mainLayout->setSpacing(10);

        // График 1: Входное слово
        QCustomPlot *plotInput = new QCustomPlot;

        // Добавляем заголовок на график
        QCPTextElement *titleInput = new QCPTextElement(plotInput, "Входное слово", QFont("sans", 12, QFont::Bold));
        plotInput->plotLayout()->insertRow(0);
        plotInput->plotLayout()->addElement(0, 0, titleInput);

        QVector<double> x_input(input.size());
        for (int i = 0; i < input.size(); ++i) x_input[i] = i + 1;

        // Точки
        plotInput->addGraph();
        plotInput->graph(0)->setData(x_input, input);
        plotInput->graph(0)->setLineStyle(QCPGraph::lsNone);
        plotInput->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
        plotInput->graph(0)->setPen(QPen(Qt::blue));

        // Вертикальные линии
        plotInput->addGraph();
        plotInput->graph(1)->setData(x_input, input);
        plotInput->graph(1)->setLineStyle(QCPGraph::lsImpulse);
        plotInput->graph(1)->setPen(QPen(Qt::blue));

        plotInput->xAxis->setLabel("Индекс");
        plotInput->yAxis->setLabel("Значение");
        plotInput->xAxis->setRange(0, input.size() + 1);
        double minInput = *std::min_element(input.begin(), input.end());
        double maxInput = *std::max_element(input.begin(), input.end());
        plotInput->yAxis->setRange(minInput - 1, maxInput + 1);

        // График 2: Кодовая последовательность
        QCustomPlot *plotEncoded = new QCustomPlot;

        // Добавляем заголовок на график
        QCPTextElement *titleEncoded = new QCPTextElement(plotEncoded, "Кодовая последовательность", QFont("sans", 12, QFont::Bold));
        plotEncoded->plotLayout()->insertRow(0);
        plotEncoded->plotLayout()->addElement(0, 0, titleEncoded);

        QVector<double> x_encoded(encoded.size());
        for (int i = 0; i < encoded.size(); ++i) x_encoded[i] = i + 1;

        plotEncoded->addGraph();
        plotEncoded->graph(0)->setData(x_encoded, encoded);
        plotEncoded->graph(0)->setLineStyle(QCPGraph::lsNone);
        plotEncoded->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
        plotEncoded->graph(0)->setPen(QPen(Qt::blue));

        plotEncoded->addGraph();
        plotEncoded->graph(1)->setData(x_encoded, encoded);
        plotEncoded->graph(1)->setLineStyle(QCPGraph::lsImpulse);
        plotEncoded->graph(1)->setPen(QPen(Qt::blue));

        plotEncoded->xAxis->setLabel("Индекс");
        plotEncoded->yAxis->setLabel("Значение");
        plotEncoded->xAxis->setRange(0, encoded.size() + 1);
        double minEncoded = *std::min_element(encoded.begin(), encoded.end());
        double maxEncoded = *std::max_element(encoded.begin(), encoded.end());
        plotEncoded->yAxis->setRange(minEncoded - 1, maxEncoded + 1);

        // График 3: Декодированное слово
        QCustomPlot *plotDecoded = new QCustomPlot;

        // Добавляем заголовок на график
        QCPTextElement *titleDecoded = new QCPTextElement(plotDecoded, "Декодированное слово", QFont("sans", 12, QFont::Bold));
        plotDecoded->plotLayout()->insertRow(0);
        plotDecoded->plotLayout()->addElement(0, 0, titleDecoded);

        QVector<double> x_decoded(decoded.size());
        for (int i = 0; i < decoded.size(); ++i) x_decoded[i] = i + 1;

        plotDecoded->addGraph();
        plotDecoded->graph(0)->setData(x_decoded, decoded);
        plotDecoded->graph(0)->setLineStyle(QCPGraph::lsNone);
        plotDecoded->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
        plotDecoded->graph(0)->setPen(QPen(Qt::blue));

        plotDecoded->addGraph();
        plotDecoded->graph(1)->setData(x_decoded, decoded);
        plotDecoded->graph(1)->setLineStyle(QCPGraph::lsImpulse);
        plotDecoded->graph(1)->setPen(QPen(Qt::blue));

        plotDecoded->xAxis->setLabel("Индекс");
        plotDecoded->yAxis->setLabel("Значение");
        plotDecoded->xAxis->setRange(0, decoded.size() + 1);
        double minDecoded = *std::min_element(decoded.begin(), decoded.end());
        double maxDecoded = *std::max_element(decoded.begin(), decoded.end());
        plotDecoded->yAxis->setRange(minDecoded - 1, maxDecoded + 1);

        // Добавляем все графики в основной layout
        mainLayout->addWidget(plotInput);
        mainLayout->addWidget(plotEncoded);
        mainLayout->addWidget(plotDecoded);

        // Создаем кнопку сохранения для всего окна
        QPushButton *saveButton = new QPushButton("Сохранить");
        saveButton->setFixedSize(150, 30);
        saveButton->setStyleSheet(ThemeStyles::lightButtonStyle());

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();
        buttonLayout->addWidget(saveButton);

        QMenu *saveMenu = new QMenu(saveButton);
        saveMenu->addAction("Сохранить как PNG", [=]() {
            saveThreePlots(plotInput, plotEncoded, plotDecoded, "PNG");
        });
        saveMenu->addAction("Сохранить как JPEG", [=]() {
            saveThreePlots(plotInput, plotEncoded, plotDecoded, "JPEG");
        });
        saveMenu->addAction("Сохранить как PDF", [=]() {
            saveThreePlots(plotInput, plotEncoded, plotDecoded, "PDF");
        });

        connect(saveButton, &QPushButton::clicked, this, [=]() {
            saveMenu->exec(saveButton->mapToGlobal(QPoint(0, saveButton->height())));
        });

        mainLayout->addLayout(buttonLayout);

        // Перерисовываем все графики
        plotInput->replot();
        plotEncoded->replot();
        plotDecoded->replot();

        mainWindow->show();
    });
}

// ===== Чтение CSV =====
QVector<double> Lab2Panel::readCsv(const QString &filePath)
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

// ===== Построение графика =====
void Lab2Panel::plotCsv(const QString &fileName)
{
    QVector<double> data = readCsv(fileName);
    if (data.isEmpty()) return;

    // Создаем основное окно
    QWidget *mainWindow = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWindow);

    // Создаем график
    QCustomPlot *plot = new QCustomPlot;

    // Устанавливаем заголовок окна
    QString windowTitle;
    QString graphTitle;
    if (fileName == "p2_random") {
        windowTitle = "Входное слово";
        graphTitle = "Входное слово";
    } else if (fileName == "p2_encoded") {
        windowTitle = "Кодовая последовательность";
        graphTitle = "Кодовая последовательность";
    } else if (fileName == "p2_decoded") {
        windowTitle = "Декодированное слово";
        graphTitle = "Декодированное слово";
    } else if (fileName == "p2_compare") {
        windowTitle = "Сравнение последовательностей";
        graphTitle = "Сравнение последовательностей";
    } else {
        windowTitle = "График";
        graphTitle = "График";
    }
    mainWindow->setWindowTitle(windowTitle);

    // Добавляем заголовок прямо на график
    QCPTextElement *titleElement = new QCPTextElement(plot, graphTitle, QFont("sans", 12, QFont::Bold));
    plot->plotLayout()->insertRow(0);
    plot->plotLayout()->addElement(0, 0, titleElement);

    // Создаем данные для оси X
    QVector<double> x(data.size());
    for (int i = 0; i < data.size(); ++i) {
        x[i] = i + 1;
    }

    // Добавляем график точек
    plot->addGraph();
    plot->graph(0)->setData(x, data);
    plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
    plot->graph(0)->setPen(QPen(Qt::blue));

    // Добавляем вертикальные линии (stem)
    plot->addGraph();
    plot->graph(1)->setData(x, data);
    plot->graph(1)->setLineStyle(QCPGraph::lsImpulse);
    plot->graph(1)->setPen(QPen(Qt::blue));

    // Настраиваем оси
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


    // Создаем кнопку сохранения в правом нижнем углу
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

    // Добавляем элементы в основной layout
    mainLayout->addWidget(plot);
    mainLayout->addLayout(buttonLayout);

    // Перерисовываем график
    plot->replot();
    mainWindow->resize(800, 600);
    mainWindow->show();
}

// ===== Функция сохранения графика =====
void Lab2Panel::savePlot(QCustomPlot *plot, const QString &format)
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

// ===== Функция сохранения трех графиков =====
void Lab2Panel::saveThreePlots(QCustomPlot *plot1, QCustomPlot *plot2, QCustomPlot *plot3, const QString &format)
{
    QWidget *parent = plot1->parentWidget();
    if (!parent) parent = plot1;

    QString dirPath = QFileDialog::getExistingDirectory(parent,
                                                        "Выберите папку для сохранения графиков",
                                                        QDir::homePath());

    if (!dirPath.isEmpty()) {
        QStringList fileNames = {
            "Входное_слово",
            "Кодовая_последовательность",
            "Декодированное_слово"
        };

        QList<QCustomPlot*> plots = {plot1, plot2, plot3};
        bool allSaved = true;

        for (int i = 0; i < plots.size(); ++i) {
            QString fileName = dirPath + "/" + fileNames[i] + getFileExtension(format);
            bool success = false;

            if (format == "PNG") {
                success = plots[i]->savePng(fileName, plots[i]->width(), plots[i]->height());
            } else if (format == "JPEG") {
                success = plots[i]->saveJpg(fileName, plots[i]->width(), plots[i]->height());
            } else if (format == "PDF") {
                success = plots[i]->savePdf(fileName, plots[i]->width(), plots[i]->height());
            }

            if (!success) {
                allSaved = false;
            }
        }

        if (allSaved) {
            emit logMessage(QString("Все графики сохранены в папку: %1").arg(dirPath));
        } else {
            emit logMessage("Ошибка при сохранении некоторых графиков");
        }
    }
}

// ===== Функция получения фильтра файлов =====
QString Lab2Panel::getFileFilter(const QString &format)
{
    if (format == "PNG") return "PNG Files (*.png)";
    if (format == "JPEG") return "JPEG Files (*.jpg *.jpeg)";
    if (format == "PDF") return "PDF Files (*.pdf)";
    return "All Files (*)";
}

// ===== Функция получения расширения файла =====
QString Lab2Panel::getFileExtension(const QString &format)
{
    if (format == "PNG") return ".png";
    if (format == "JPEG") return ".jpg";
    if (format == "PDF") return ".pdf";
    return "";
}
