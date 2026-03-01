#pragma once
#include <QDialog>
#include "qcustomplot.h"
#include "ManualPlotDialogCompare.h"

class PlotWindowCompare : public QDialog
{
    Q_OBJECT
public:
    explicit PlotWindowCompare(const PlotData& plot1,
                               const QString& name1,
                               const PlotData& plot2,
                               const QString& name2,
                               int totalBits,
                               QWidget* parent=nullptr);

private slots:
    void savePlot();

private:
    QCustomPlot* customPlot;
    int N_dec;
};
