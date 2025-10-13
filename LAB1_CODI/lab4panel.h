#pragma once
#include <QWidget>
#include "ManualPlotDialog.h"

class Lab4Panel : public QWidget {
    Q_OBJECT
public:
    explicit Lab4Panel(QWidget *parent = nullptr);

signals:
    void logMessage(const QString &);

};
