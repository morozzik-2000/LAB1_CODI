#pragma once
#include <QString>
#include <QProcess>
#include <QObject>
#include <QVector>
#include "OctaveParams.h"

// struct OctaveParams {
//     int n;
//     int k;
//     int t;
//     int numWords;
//     double errorProb;
//     int part;
// };

class OctaveRunner : public QObject {
    Q_OBJECT
public:
    explicit OctaveRunner(QObject *parent = nullptr);

    void runOctave(OctaveParams_ &params);

signals:
    void finished();
    void errorOccurred(const QString &msg);
    void logMessage(const QString &msg);
    void plotDataReady(const QString &title, const QVector<double> &x, const QVector<double> &y);

private:
    void writeOctaveScript(OctaveParams_ &params, const QString &scriptPath, const QString &outDir);

    QProcess *proc;
    QString outDir;
};
