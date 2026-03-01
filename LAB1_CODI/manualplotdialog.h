#ifndef MANUALPLOTDIALOG_H
#define MANUALPLOTDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QTextStream>
#include <QPrinter>
#include <QPainter>
#include "lab1panel.h"
#include "PlotDataForCompare.h"

class QCustomPlot;

class ManualPlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManualPlotDialog(
        const QString &instructionText,
        const QString &yAxisLabel,
        const QString &plotTitle,
        QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;



private slots:
    void addPoint();
    void removePoint();
    void clearPoints();
    void plotGraph();
    void savePointsToTxt();
    void savePlot(QCustomPlot *customPlot);


private:
    void setupUI();
    void updateTable();

    QTableWidget *m_table;
    QVector<double> m_pkValues;
    QVector<double> m_errorValues;

    QString m_instructionText;
    QString m_yAxisLabel;
    QString m_plotTitle;

    // Параметры кода
    int n = 127;
    int k = 64;
    int t = 10;
    int numWords = 1000;
    int N_dec_new = k * numWords;
};

#endif // MANUALPLOTDIALOG_H
