#pragma once
#include <QDialog>
#include <QPushButton>
#include "ManualPlotDialogCompare.h"

class ComparisonWindow : public QDialog
{
    Q_OBJECT
public:
    explicit ComparisonWindow(QWidget* parent=nullptr);

private slots:
    void enterPlot1();
    void enterPlot2();
    void buildPlot();

private:
    QPushButton* btnPlot1;
    QPushButton* btnPlot2;
    QPushButton* btnBuild;

    PlotData plot1, plot2;
    QString name1, name2;
    bool hasPlot1 = false, hasPlot2 = false;
    ManualPlotDialogCompare* manualPlotDialog1 = nullptr;
    ManualPlotDialogCompare* manualPlotDialog2 = nullptr;
};
