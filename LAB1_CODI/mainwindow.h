#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "OctaveRunner.h"
#include "OctaveParams.h"
#include "Lab1Panel.h"
#include "Lab2Panel.h"
#include "Lab3Panel.h"
#include "Lab4Panel.h"
#include "PlotDataForCompare.h"

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWidget* createLeftPanel();
    QWidget* createTopArea();

    QStackedWidget *stack = nullptr;
    QTextEdit *logTextEdit = nullptr;
    QPushButton *runOctaveButton = nullptr;
    OctaveRunner * octRunner= nullptr;
    // Left panel buttons
    QPushButton *btnLab1 = nullptr;
    QPushButton *btnLab2 = nullptr;
    QPushButton *btnLab3 = nullptr;
    QPushButton *btnLab4 = nullptr;

    QGroupBox *topGroupBox = nullptr;

    Lab1Panel *p1 = nullptr;
    Lab2Panel *p2 = nullptr;
    Lab3Panel *p3 = nullptr;
    Lab4Panel *p4 = nullptr;

    QStringList logHistory;

    void appendHeader(const QString &msg);

    QPushButton *btnLab5 = nullptr;

    PlotData inputData;
    PlotData outputData;

    bool hasInput=false;
    bool hasOutput=false;


private slots:
    void onPlotReady(const PlotData &data);
    void showComparisonPlot();

private slots:
    void onLab1Clicked();
    void onLab2Clicked();
    void onLab3Clicked();
    void onLab4Clicked();
    void onStackIndexChanged(int idx);
    void appendLog(const QString &msg);
    void startModeling();
    void saveLogToFile();

};
