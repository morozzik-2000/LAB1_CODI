#pragma once
#include <QObject>
#include <QProcess>
#include <QString>
#include "OctaveParams.h"

// struct OctaveParamsPart3 {
//     int n;
//     int k;
//     int t;
//     int numWords;
//     double channelErrorProbability;
// };

class OctaveRunnerPart3 : public QObject
{
    Q_OBJECT
public:
    explicit OctaveRunnerPart3(QObject *parent = nullptr);
    void runOctave(OctaveParams_ &params);

signals:
    void finished();
    void errorOccurred(const QString &msg);
    void logMessage(const QString &msg);

private:
    void writeOctaveScript(OctaveParams_ &p, const QString &path, const QString &outDir);
    QProcess *proc;
    QString outDir;
};
