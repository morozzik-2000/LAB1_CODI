#ifndef LAB2PANEL_H
#define LAB2PANEL_H

#include <QWidget>
#include <QCustomPlot.h>
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

class Lab2Panel : public QWidget
{
    Q_OBJECT

public:
    explicit Lab2Panel(QWidget *parent = nullptr);

signals:
    void logMessage(const QString &message);
    void runRequestedLab2();

private slots:
    void savePlot(QCustomPlot *plot, const QString &format);
    void saveThreePlots(QCustomPlot *plot1, QCustomPlot *plot2, QCustomPlot *plot3, const QString &format);
    QString getFileFilter(const QString &format);
    QString getFileExtension(const QString &format);

private:
    QVector<double> readCsv(const QString &filePath);
    void plotCsv(const QString &fileName);
};

#endif // LAB2PANEL_H
