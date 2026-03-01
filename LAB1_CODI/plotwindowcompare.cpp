#include "PlotWindowCompare.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileDialog>

PlotWindowCompare::PlotWindowCompare(const PlotData& plot1,
                                     const QString& name1,
                                     const PlotData& plot2,
                                     const QString& name2,
                                     int totalBits,
                                     QWidget* parent)
    : QDialog(parent), N_dec(totalBits)
{
    setWindowTitle("Сравнение BER");
    resize(1000,700);

    QVBoxLayout* layout = new QVBoxLayout(this);
    customPlot = new QCustomPlot;
    layout->addWidget(customPlot,1);

    // Вычисляем BER
    QVector<double> ber1, ber2;
    for(double e : plot1.errors) ber1.append(e / N_dec);
    for(double e : plot2.errors) ber2.append(e / N_dec);

    // График 1
    customPlot->addGraph();
    customPlot->graph(0)->setData(plot1.pk, ber1);
    customPlot->graph(0)->setPen(QPen(Qt::blue,2));
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,6));
    customPlot->graph(0)->setName(name1);

    // График 2
    customPlot->addGraph();
    customPlot->graph(1)->setData(plot2.pk, ber2);
    customPlot->graph(1)->setPen(QPen(Qt::red,2));
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare,6));
    customPlot->graph(1)->setName(name2);

    customPlot->legend->setVisible(true);
    customPlot->xAxis->setLabel("Вероятность канальной ошибки pₖ");
    customPlot->yAxis->setLabel("BER на выходе/входе декодера");
    customPlot->rescaleAxes();
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->replot();

    // Кнопки
    QDialogButtonBox* box = new QDialogButtonBox();
    QPushButton* saveBtn = new QPushButton("💾 Сохранить");
    QPushButton* closeBtn = new QPushButton("Закрыть");
    box->addButton(saveBtn,QDialogButtonBox::ActionRole);
    box->addButton(closeBtn,QDialogButtonBox::RejectRole);
    layout->addWidget(box);

    connect(saveBtn, &QPushButton::clicked, this, &PlotWindowCompare::savePlot);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::close);
}

void PlotWindowCompare::savePlot()
{
    QString file = QFileDialog::getSaveFileName(this,
                                                "Сохранить график",
                                                "BER_Comparison",
                                                "PNG (*.png);;JPEG (*.jpg);;PDF (*.pdf)");
    if(file.isEmpty()) return;

    if(file.endsWith(".png")) customPlot->savePng(file,1200,800);
    else if(file.endsWith(".jpg")) customPlot->saveJpg(file,1200,800);
    else if(file.endsWith(".pdf")) customPlot->savePdf(file);
}
