#pragma once
#include <QObject>
#include <QProcess>
#include <QString>
#include "Lab3Panel.h" // для OctaveParams
#include "OctaveParams.h"

// struct OctaveParamsPart2 {
//     int n;
//     int k;
//     int t;
//     int numWords;
//     double channelErrorProbability;
// };

class OctaveRunnerPart2 : public QObject
{
    Q_OBJECT
public:
    explicit OctaveRunnerPart2(QObject *parent = nullptr);
    void runOctave(OctaveParams_ &params);

    void setParams(const OctaveParams_ &p) { this->params = p; }

public slots:
    void run();

signals:
    void finished();
    void errorOccurred(const QString &msg);
    void logMessage(const QString &msg);

private:
    void writeOctaveScript(OctaveParams_ &p, const QString &path, const QString &outDir);
    QProcess *proc;
    QString outDir;

    OctaveParams_ params;

};
