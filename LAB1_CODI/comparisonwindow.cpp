#include "ComparisonWindow.h"
#include "ManualPlotDialogCompare.h"
#include "PlotWindowCompare.h"
#include <QVBoxLayout>

ComparisonWindow::ComparisonWindow(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Сравнение BER");
    resize(400,300);

    QVBoxLayout* layout = new QVBoxLayout(this);

    btnPlot1 = new QPushButton("Ввести точки: вход декодера");
    btnPlot2 = new QPushButton("Ввести точки: выход декодера");
    btnBuild = new QPushButton("Построить сравнение");

    layout->addWidget(btnPlot1);
    layout->addWidget(btnPlot2);
    layout->addWidget(btnBuild);

    connect(btnPlot1,&QPushButton::clicked,this,&ComparisonWindow::enterPlot1);
    connect(btnPlot2,&QPushButton::clicked,this,&ComparisonWindow::enterPlot2);
    connect(btnBuild,&QPushButton::clicked,this,&ComparisonWindow::buildPlot);
}

void ComparisonWindow::enterPlot1()
{
    if (!manualPlotDialog1) {
        manualPlotDialog1 = new ManualPlotDialogCompare(
            "BER на входе декодера",  // title
            "BER на входе",                            // yAxisLabel
            "График BER входа",                        // plotTitle
            nullptr                                    // parent
            );

        manualPlotDialog1->setAttribute(Qt::WA_DeleteOnClose, false); // окно не удаляется при закрытии
        manualPlotDialog1->setWindowFlags(manualPlotDialog1->windowFlags() | Qt::Window); // топ-левел

        connect(manualPlotDialog1, &QObject::destroyed, [this]() {
            manualPlotDialog1 = nullptr;
        });
    }

    // Показываем окно
    manualPlotDialog1->show();
    manualPlotDialog1->raise();
    manualPlotDialog1->activateWindow();

    // Разворачиваем, если свернуто
    if (manualPlotDialog1->windowState() & Qt::WindowMinimized) {
        manualPlotDialog1->setWindowState(manualPlotDialog1->windowState() & ~Qt::WindowMinimized);
    }
}

void ComparisonWindow::enterPlot2()
{
    if (!manualPlotDialog2) {
        manualPlotDialog2 = new ManualPlotDialogCompare(
            "BER на выходе декодера",
            "BER на выходе",
            "График BER выхода",
            nullptr
            );

        manualPlotDialog2->setAttribute(Qt::WA_DeleteOnClose, false);
        manualPlotDialog2->setWindowFlags(manualPlotDialog2->windowFlags() | Qt::Window);

        connect(manualPlotDialog2, &QObject::destroyed, [this]() {
            manualPlotDialog2 = nullptr;
        });
    }

    manualPlotDialog2->show();
    manualPlotDialog2->raise();
    manualPlotDialog2->activateWindow();

    if (manualPlotDialog2->windowState() & Qt::WindowMinimized) {
        manualPlotDialog2->setWindowState(manualPlotDialog2->windowState() & ~Qt::WindowMinimized);
    }
}

void ComparisonWindow::buildPlot()
{
    // Проверяем, что оба окна существуют и в них есть данные
    if (!manualPlotDialog1 || !manualPlotDialog2) return;

    // Получаем данные напрямую из диалогов
    plot1 = manualPlotDialog1->getData();
    name1 = manualPlotDialog1->getTitle();
    plot2 = manualPlotDialog2->getData();
    name2 = manualPlotDialog2->getTitle();

    if(plot1.pk.isEmpty() || plot2.pk.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Оба графика должны содержать хотя бы одну точку.");
        return;
    }

    int N_dec = 10000; // заменить на реальное количество декодированных бит

    PlotWindowCompare* window = new PlotWindowCompare(plot1, name1, plot2, name2, N_dec, this);
    window->show();
}
