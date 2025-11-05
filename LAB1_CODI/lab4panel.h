#ifndef LAB4PANEL_H
#define LAB4PANEL_H

#include <QWidget>
#include <QCustomPlot.h>
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

class Lab4Panel : public QWidget
{
    Q_OBJECT

public:
    explicit Lab4Panel(QWidget *parent = nullptr);
    OctaveParams_ getParams() const;

signals:
    void logMessage(const QString &message);
    void runRequestedLab4();

private slots:
    void savePlot(QCustomPlot *plot, const QString &format);
    QString getFileFilter(const QString &format);
    QString getFileExtension(const QString &format);

private:
    QVector<double> readCsv(const QString &filePath);
    void plotCsv(const QString &fileName, const QString &title);
    QDoubleSpinBox *pSpin;
    ManualPlotDialog *manualPlotDialog = nullptr;
};

#endif // LAB4PANEL_H
