#pragma once
#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QVector>
#include <QCloseEvent>

struct PlotData {
    QVector<double> pk;
    QVector<double> errors;
};

class ManualPlotDialogCompare : public QDialog
{
    Q_OBJECT
public:
    explicit ManualPlotDialogCompare(const QString& title,
                                     const QString& yAxisLabel,
                                     const QString& plotTitle,
                                     QWidget* parent = nullptr);

    PlotData getData() const { return data; }
    QString getTitle() const { return m_title; }

private slots:
    void addPoint();
    void removePoint();
    void clearPoints();

private:
    void updateTable();

    QString m_title;
    QString m_yAxisLabel;
    QString m_plotTitle;

    QTableWidget* table;
    QVector<double> m_pkValues;
    QVector<double> m_errorValues;
    PlotData data;

protected:
    void closeEvent(QCloseEvent* event) override;
};
