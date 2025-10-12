#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QSpinBox>
#include <QLabel>
#include "themestyles.h"


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onLab1Clicked();
    void onLab2Clicked();
    void onLab3Clicked();
    void onLab4Clicked();
    void onLabButtonClicked();

private:
    // Layouts
    QHBoxLayout *mainLayout;
    QVBoxLayout *leftLayout;
    QVBoxLayout *rightLayout;
    QVBoxLayout *topAreaLayout;

    // GroupBoxes
    QGroupBox *leftGroupBox;
    QGroupBox *topGroupBox;
    QGroupBox *logGroupBox;

    // Widgets
    QWidget *topArea;
    QTextEdit *logTextEdit;

    // Buttons
    QPushButton *btnLab1;
    QPushButton *btnLab2;
    QPushButton *btnLab3;
    QPushButton *btnLab4;

    QPushButton *runOctaveButton;

    QPushButton *saveLog;
    QPushButton *toggleTheme;


    // SpinBoxes

    QSpinBox *spinCodeLength = nullptr;
    QSpinBox *spinInfoLength = nullptr;
    QSpinBox *spinСorrectingAbility = nullptr;
    QSpinBox *spinInWordsfoLength = nullptr;



    // Private methods
    QGroupBox* createLeftPanel();
    QGroupBox* createTopPanel();
    QGroupBox* createLogPanel();
    void setupConnections();
    void clearTopArea();

    QWidget *lab1Panel;
    QWidget *lab2Panel;
    QWidget *lab3Panel;
    QWidget *lab4Panel;


    void hideAllPanels();

};

#endif // MAINWINDOW_H
