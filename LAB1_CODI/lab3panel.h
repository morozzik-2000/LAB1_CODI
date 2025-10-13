#pragma once
#include <QWidget>
#include "ManualPlotDialog.h"

class Lab3Panel : public QWidget {
    Q_OBJECT
public:
    explicit Lab3Panel(QWidget *parent = nullptr);

signals:
    void logMessage(const QString &);

};
