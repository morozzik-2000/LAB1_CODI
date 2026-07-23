#pragma once
#include <QString>
#include <QProcess>
#include <QObject>
#include <QVector>
#include "OctaveParams.h"

class OctaveRunner : public QObject {
    Q_OBJECT
public:
    explicit OctaveRunner(QObject *parent = nullptr);
    void setParams(const OctaveParams_ &p) { this->params = p; }
    void runOctave(OctaveParams_ &params);

public slots:
    void run();

signals:
    void finished();
    void errorOccurred(const QString &msg);
    void logMessage(const QString &msg);
    void plotDataReady(const QString &title, const QVector<double> &x, const QVector<double> &y);


private:
    void writeOctaveScript(OctaveParams_ &params, const QString &scriptPath, const QString &outDir);
    QString m_resultsPath;
    QProcess *proc;
    QString outDir;
    OctaveParams_ params;
    QString findOctaveExecutable();
    QString findFileRecursive(const QString &startPath, const QStringList &patternParts, int depth = 0);
    QString deepSearchForOctave(const QString &startDir, int depth = 0);
};
