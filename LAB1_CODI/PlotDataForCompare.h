#pragma once
#include <QVector>
#include <QString>

struct PlotData
{
    QVector<double> pk;
    QVector<double> ber;
    QString name;
};
