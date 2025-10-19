#pragma once
#include <QWidget>
#include "OctaveRunner.h"
class QSpinBox;

class Lab1Panel : public QWidget {
    Q_OBJECT
public:
    explicit Lab1Panel(QWidget *parent = nullptr);
    OctaveParams getParams() const;
    QSpinBox *spinCodeLength;
    QSpinBox *spinInfoLength;
    QSpinBox *spinCorrecting;
    QSpinBox *spinWords;

signals:
    void logMessage(const QString &);

};
