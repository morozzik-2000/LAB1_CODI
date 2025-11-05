#ifndef MANUALPLOTDIALOG_H
#define MANUALPLOTDIALOG_H

#include <QDialog>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>
#include "Lab1Panel.h"
#include <QCloseEvent>

class QCustomPlot;

class ManualPlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManualPlotDialog(
            const QString &instructionText = "Введите значения вероятности канальной ошибки (p_k) и количество ошибок на выходе декодере:",
            const QString &yAxisLabel = "BER_{дк} на выходе декодера",
            const QString &plotTitle = "График BER для BCH(127,64,10)",
            QWidget *parent = nullptr);

    QVector<double> getPkValues() const { return m_pkValues; }
    QVector<double> getErrorValues() const { return m_errorValues; }


private slots:
    void addPoint();
    void removePoint();
    void clearPoints();
    void plotGraph();
    void updateTable();
    void savePlot(QCustomPlot *customPlot);



private:
    QTableWidget *m_table;
    QVector<double> m_pkValues;
    QVector<double> m_errorValues;

    QString m_instructionText;
    QString m_yAxisLabel;
    QString m_plotTitle;

    int n = 127, k = 64, t = 10, numWords = 1000, N_dec_new;

    void setupUI();

protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // MANUALPLOTDIALOG_H
