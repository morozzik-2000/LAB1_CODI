#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWidget* createLeftPanel();
    QWidget* createTopArea();

    QStackedWidget *stack = nullptr;
    QTextEdit *logTextEdit = nullptr;
    QPushButton *runOctaveButton = nullptr;

    // Left panel buttons
    QPushButton *btnLab1 = nullptr;
    QPushButton *btnLab2 = nullptr;
    QPushButton *btnLab3 = nullptr;
    QPushButton *btnLab4 = nullptr;

private slots:
    void onLab1Clicked();
    void onLab2Clicked();
    void onLab3Clicked();
    void onLab4Clicked();
    void onStackIndexChanged(int idx);
    void appendLog(const QString &msg);
};
