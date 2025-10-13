#pragma once
#include <QWidget>

class QSpinBox;

class Lab1Panel : public QWidget {
    Q_OBJECT
public:
    explicit Lab1Panel(QWidget *parent = nullptr);

signals:
    void logMessage(const QString &);

};
