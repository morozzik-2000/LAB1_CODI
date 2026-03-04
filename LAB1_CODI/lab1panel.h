#pragma once
#include <QWidget>
#include "OctaveRunner.h"
#include "OctaveParams.h"
#include <QPushButton>

class QSpinBox;

class Lab1Panel : public QWidget {
    Q_OBJECT
public:
    explicit Lab1Panel(QWidget *parent = nullptr);
    OctaveParams_ getParams() const;
    QSpinBox *spinCodeLength;
    QSpinBox *spinInfoLength;
    QSpinBox *spinCorrecting;
    QSpinBox *spinWords;


signals:
    void logMessage(const QString &);
    void paramsChanged(const OctaveParams_ &params);

private slots:


};
