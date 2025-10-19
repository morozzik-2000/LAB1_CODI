#ifndef LAB2PANEL_H
#define LAB2PANEL_H

#include <QWidget>
#include "qcustomplot.h"

class QPushButton;

class Lab2Panel : public QWidget
{
    Q_OBJECT
public:
    explicit Lab2Panel(QWidget *parent = nullptr);

signals:
    void logMessage(const QString &msg);

private:
    QCustomPlot *plot;

    void plotCsv(const QString &fileName);
    QVector<double> readCsv(const QString &filePath);
};

#endif // LAB2PANEL_H
