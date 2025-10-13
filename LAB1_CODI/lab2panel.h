#pragma once
#include <QWidget>

class Lab2Panel : public QWidget {
    Q_OBJECT
public:
    explicit Lab2Panel(QWidget *parent = nullptr);

signals:
    void logMessage(const QString &);

};
