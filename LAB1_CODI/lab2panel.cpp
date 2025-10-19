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

    layout->addWidget(title);
    layout->addWidget(buttonsGroup);
    layout->addStretch();

    // ===== Подключаем кнопки к чтению CSV =====
    connect(inputWord, &QPushButton::clicked, this, [=](){
        emit logMessage("Показано: Входное слово");
        plotCsv("p2_random");
    });
    connect(decoded, &QPushButton::clicked, this, [=](){
        emit logMessage("Показано: Декодированное слово");
        plotCsv("p2_decoded");
    });
    connect(codeSeq, &QPushButton::clicked, this, [=](){
        emit logMessage("Показано: Кодовая последовательность");
        plotCsv("p2_encoded");
    });
    connect(compare, &QPushButton::clicked, this, [=](){
        emit logMessage("Показано: Сравнение последовательностей");
        plotCsv("p2_compare");
    });
    connect(three, &QPushButton::clicked, this, [=](){
        emit logMessage("Показано: Три на одном");
        // Три графика на одном
        QVector<double> r = readCsv("results/p2_three_random.csv");
        QVector<double> e = readCsv("results/p2_three_encoded.csv");
        QVector<double> d = readCsv("results/p2_three_decoded.csv");
        if(r.isEmpty() || e.isEmpty() || d.isEmpty()) return;

        QCustomPlot *plotThree = new QCustomPlot;
        plotThree->addGraph(); plotThree->graph(0)->setData(QVector<double>::fromStdVector(std::vector<double>(r.size())), r); plotThree->graph(0)->setPen(QPen(Qt::blue));
        plotThree->addGraph(); plotThree->graph(1)->setData(QVector<double>::fromStdVector(std::vector<double>(e.size())), e); plotThree->graph(1)->setPen(QPen(Qt::red));
        plotThree->addGraph(); plotThree->graph(2)->setData(QVector<double>::fromStdVector(std::vector<double>(d.size())), d); plotThree->graph(2)->setPen(QPen(Qt::green));
        plotThree->xAxis->setRange(0, r.size());
        plotThree->yAxis->rescale();
        plotThree->replot();
        plotThree->resize(800,600);
        plotThree->show();
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

    QCustomPlot *plot = new QCustomPlot;
    plot->addGraph(); // Для точек
    QVector<double> x(data.size());
    for (int i=0;i<data.size();++i) x[i]=i+1;

    // Рисуем точки
    plot->graph(0)->setData(x, data);
    plot->graph(0)->setLineStyle(QCPGraph::lsNone); // отключаем линии
    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6)); // точки

    // Для вертикальных линий (как у stem)
    QCPGraph *stemLines = plot->addGraph();
    QVector<double> zeros(data.size(), 0);
    stemLines->setData(x, data);
    stemLines->setLineStyle(QCPGraph::lsImpulse); // вертикальные линии
    stemLines->setPen(QPen(Qt::blue));

    plot->xAxis->setLabel("Index");
    plot->yAxis->setLabel("Value");
    plot->xAxis->setRange(0,data.size());
    double min = *std::min_element(data.begin(), data.end());
    double max = *std::max_element(data.begin(), data.end());
    plot->yAxis->setRange(min-1,max+1);
    plot->resize(800,600);
    plot->replot();
    plot->show();
}

