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
class QCustomPlot;

class ManualPlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ManualPlotDialog(QWidget *parent = nullptr);

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

    void setupUI();
};

#endif // MANUALPLOTDIALOG_H
