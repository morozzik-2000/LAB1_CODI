#include "manualplotdialog.h"
#include <QHeaderView>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include "qcustomplot.h"


ManualPlotDialog::ManualPlotDialog(
        const QString &instructionText,
        const QString &yAxisLabel,
        const QString &plotTitle,
        QWidget *parent)
        : QDialog(parent),
          m_instructionText(instructionText),
          m_yAxisLabel(yAxisLabel),
          m_plotTitle(plotTitle)
{
    setupUI();
}

void ManualPlotDialog::setupUI()
{
    setWindowTitle("Построение графика BER вручную");
    setMinimumSize(800, 600);

    auto *layout = new QVBoxLayout(this);

    // Инструкция
    auto *instructionLabel = new QLabel(m_instructionText);
    layout->addWidget(instructionLabel);

    // Таблица для ввода точек
    m_table = new QTableWidget(0, 2, this);
    m_table->setHorizontalHeaderLabels(QStringList() << "p_k" << "Количество ошибок");
    m_table->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_table);

    // Кнопки управления таблицей
    auto *tableButtonsLayout = new QHBoxLayout();

    auto *addButton = new QPushButton("Добавить точку");
    auto *removeButton = new QPushButton("Удалить выбранную");
    auto *clearButton = new QPushButton("Очистить все");

    tableButtonsLayout->addWidget(addButton);
    tableButtonsLayout->addWidget(removeButton);
    tableButtonsLayout->addWidget(clearButton);
    tableButtonsLayout->addStretch();

    layout->addLayout(tableButtonsLayout);

    // Кнопки диалога
    auto *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto *plotButton = new QPushButton("Построить график");
    buttonBox->addButton(plotButton, QDialogButtonBox::ActionRole);

    layout->addWidget(buttonBox);

    // Подключаем сигналы
    connect(addButton, &QPushButton::clicked, this, &ManualPlotDialog::addPoint);
    connect(removeButton, &QPushButton::clicked, this, &ManualPlotDialog::removePoint);
    connect(clearButton, &QPushButton::clicked, this, &ManualPlotDialog::clearPoints);
    connect(plotButton, &QPushButton::clicked, this, &ManualPlotDialog::plotGraph);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Добавляем начальные точки из ваших данных
    QVector<double> initialPk = {0.0010, 0.0015, 0.0020, 0.0025, 0.0030, 0.0035, 0.0040, 0.0045, 0.0050,
                                 0.0055, 0.0060, 0.0065, 0.0070, 0.0075, 0.0080, 0.0085, 0.0090, 0.0095,
                                 0.0100, 0.0150, 0.0200, 0.0250, 0.0300, 0.0350, 0.0400, 0.0450, 0.0500,
                                 0.0550, 0.0600, 0.0650, 0.0700, 0.0750, 0.0800, 0.0850, 0.0900, 0.0950, 0.1000};

    QVector<double> initialErrors = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 8, 12, 54, 106, 147, 270, 534, 1034, 1177, 1666, 2154,
                                     2775, 3425, 4122, 4576, 5045};

    for (int i = 0; i < initialPk.size(); ++i) {
        m_pkValues.append(initialPk[i]);
        m_errorValues.append(initialErrors[i]);
    }

    updateTable();
}

void ManualPlotDialog::addPoint()
{
    m_pkValues.append(0.0);
    m_errorValues.append(0.0);
    updateTable();

    // Переходим к редактированию новой строки
    m_table->setCurrentCell(m_table->rowCount() - 1, 0);
    m_table->edit(m_table->currentIndex());
}

void ManualPlotDialog::removePoint()
{
    int currentRow = m_table->currentRow();
    if (currentRow >= 0 && currentRow < m_pkValues.size()) {
        m_pkValues.removeAt(currentRow);
        m_errorValues.removeAt(currentRow);
        updateTable();
    }
}

void ManualPlotDialog::clearPoints()
{
    if (QMessageBox::question(this, "Очистка",
        "Вы уверены, что хотите удалить все точки?") == QMessageBox::Yes) {
        m_pkValues.clear();
        m_errorValues.clear();
        updateTable();
    }
}

void ManualPlotDialog::updateTable()
{
    m_table->setRowCount(m_pkValues.size());

    for (int i = 0; i < m_pkValues.size(); ++i) {
        // p_k
        auto *pkItem = new QTableWidgetItem(QString::number(m_pkValues[i], 'f', 4));
        m_table->setItem(i, 0, pkItem);

        // Количество ошибок
        auto *errorItem = new QTableWidgetItem(QString::number(m_errorValues[i]));
        m_table->setItem(i, 1, errorItem);
    }
}

void ManualPlotDialog::plotGraph()
{
    // Обновляем данные из таблицы
    for (int i = 0; i < m_table->rowCount(); ++i) {
        if (i < m_pkValues.size()) {
            bool pkOk, errorOk;
            double pk = m_table->item(i, 0)->text().toDouble(&pkOk);
            double errors = m_table->item(i, 1)->text().toDouble(&errorOk);

            if (pkOk && errorOk) {
                m_pkValues[i] = pk;
                m_errorValues[i] = errors;
            }
        }
    }

    // Создаем окно для графика
    auto *plotDialog = new QDialog(this);
    plotDialog->setWindowTitle(m_plotTitle);
    plotDialog->setMinimumSize(1000, 700);

    auto *layout = new QVBoxLayout(plotDialog);

    // Создаем виджет для графика
    auto *customPlot = new QCustomPlot;
    layout->addWidget(customPlot);

    // Рассчитываем BER
    const int N_dec_new = 64000; // 64 бита × 1000 слов
    QVector<double> berValues;
    for (double errors : m_errorValues) {
        berValues.append(errors / N_dec_new);
    }

    // Создаем график
    QVector<double> sortedPk = m_pkValues;
    QVector<double> sortedBer = berValues;

    // Сортируем по p_k для красивого графика
    for (int i = 0; i < sortedPk.size() - 1; ++i) {
        for (int j = i + 1; j < sortedPk.size(); ++j) {
            if (sortedPk[i] > sortedPk[j]) {
                std::swap(sortedPk[i], sortedPk[j]);
                std::swap(sortedBer[i], sortedBer[j]);
            }
        }
    }

    // Обычный график
    customPlot->addGraph();
    customPlot->graph(0)->setData(sortedPk, sortedBer);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(0, 0, 255), 6));
    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(0)->setPen(QPen(QColor(0, 0, 255), 2));

    // Настройка осей
    customPlot->xAxis->setLabel("Вероятность канальной ошибки p_k");
    customPlot->yAxis->setLabel(m_yAxisLabel);
    customPlot->xAxis->setRange(0, *std::max_element(sortedPk.constBegin(), sortedPk.constEnd()) * 1.1);
    customPlot->yAxis->setRange(0, *std::max_element(sortedBer.constBegin(), sortedBer.constEnd()) * 1.1);

    // Включаем взаимодействия
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // Создаем список для хранения выделенных областей
    QList<QCPItemRect*> highlightedAreas;

    // Переменные для выделения области
    QPointF selectionStart, selectionEnd;
    bool isSelecting = false;
    QCPItemRect *currentSelectionRect = nullptr;

    // Обработчик нажатия мыши - начало выделения
    connect(customPlot, &QCustomPlot::mousePress, [=, &isSelecting, &selectionStart, &currentSelectionRect](QMouseEvent *event) {
        if (event->button() == Qt::RightButton) { // Выделение правой кнопкой мыши
            isSelecting = true;
            double x = customPlot->xAxis->pixelToCoord(event->pos().x());
            double y = customPlot->yAxis->pixelToCoord(event->pos().y());
            selectionStart = QPointF(x, y);

            // Создаем прямоугольник выделения
            currentSelectionRect = new QCPItemRect(customPlot);
            currentSelectionRect->setBrush(QBrush(QColor(255, 0, 0, 80))); // Полупрозрачный красный
            currentSelectionRect->setPen(QPen(Qt::red, 2));
            currentSelectionRect->topLeft->setType(QCPItemPosition::ptPlotCoords);
            currentSelectionRect->bottomRight->setType(QCPItemPosition::ptPlotCoords);
            currentSelectionRect->topLeft->setCoords(selectionStart.x(), selectionStart.y());
            currentSelectionRect->bottomRight->setCoords(selectionStart.x(), selectionStart.y());
        }
    });

    // Обработчик движения мыши - обновление выделения
    connect(customPlot, &QCustomPlot::mouseMove, [=, &isSelecting, &selectionStart, &currentSelectionRect, &selectionEnd](QMouseEvent *event) {
        if (isSelecting && currentSelectionRect) {
            double x = customPlot->xAxis->pixelToCoord(event->pos().x());
            double y = customPlot->yAxis->pixelToCoord(event->pos().y());
            selectionEnd = QPointF(x, y);

            // Обновляем размеры прямоугольника
            double left = qMin(selectionStart.x(), selectionEnd.x());
            double right = qMax(selectionStart.x(), selectionEnd.x());
            double top = qMin(selectionStart.y(), selectionEnd.y());
            double bottom = qMax(selectionStart.y(), selectionEnd.y());

            currentSelectionRect->topLeft->setCoords(left, top);
            currentSelectionRect->bottomRight->setCoords(right, bottom);
            customPlot->replot();
        }
    });

    // Обработчик отпускания мыши - завершение выделения
    connect(customPlot, &QCustomPlot::mouseRelease, [=, &isSelecting, &highlightedAreas, &currentSelectionRect](QMouseEvent *event) {
        if (event->button() == Qt::RightButton && isSelecting) {
            isSelecting = false;
            if (currentSelectionRect) {
                highlightedAreas.append(currentSelectionRect);
                currentSelectionRect = nullptr;
                customPlot->replot();
            }
        }
    });

    customPlot->axisRect()->setupFullAxesBox();

    // Добавляем информацию о коде прямо на график
    QCPItemText *textLabel = new QCPItemText(customPlot);
    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignLeft);
    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    textLabel->position->setCoords(0.05, 0.08);
    textLabel->setText(QString("BCH(127,64,10)\nN = %1 бит").arg(N_dec_new));
    textLabel->setFont(QFont(font().family(), 10));
    textLabel->setPen(QPen(Qt::black));
    textLabel->setBrush(QBrush(Qt::white));
    textLabel->setPadding(QMargins(5, 5, 5, 5));

    // Добавляем инструкцию по выделению
    QCPItemText *instructionText = new QCPItemText(customPlot);
    instructionText->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
    instructionText->position->setType(QCPItemPosition::ptAxisRectRatio);
    instructionText->position->setCoords(0.95, 0.02);
    instructionText->setText("ЛКМ: перемещение/масштабирование\nПКМ+перетаскивание: выделение области");
    instructionText->setFont(QFont(font().family(), 8));
    instructionText->setPen(QPen(Qt::darkGray));
    instructionText->setTextAlignment(Qt::AlignRight);

    customPlot->replot();

    // Кнопки для управления
    auto *buttonBox = new QDialogButtonBox();

    // Кнопка закрытия
    auto *closeButton = new QPushButton("Закрыть");
    buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);

    // Кнопка сохранения
    auto *saveButton = new QPushButton("Сохранить");
    buttonBox->addButton(saveButton, QDialogButtonBox::ActionRole);

    // Кнопка сброса масштаба
    auto *resetZoomButton = new QPushButton("Сбросить масштаб");
    buttonBox->addButton(resetZoomButton, QDialogButtonBox::ActionRole);

    // Кнопка очистки выделений
    auto *clearHighlightsButton = new QPushButton("Очистить выделения");
    buttonBox->addButton(clearHighlightsButton, QDialogButtonBox::ActionRole);

    layout->addWidget(buttonBox);

    // Подключаем кнопку сохранения
    connect(saveButton, &QPushButton::clicked, this, [=]() {
        savePlot(customPlot);
    });

    // Подключаем кнопку сброса масштаба
    connect(resetZoomButton, &QPushButton::clicked, this, [=]() {
        customPlot->xAxis->setRange(0, *std::max_element(sortedPk.constBegin(), sortedPk.constEnd()) * 1.1);
        customPlot->yAxis->setRange(0, *std::max_element(sortedBer.constBegin(), sortedBer.constEnd()) * 1.1);
        customPlot->replot();
    });

    // Подключаем кнопку очистки выделений
    connect(clearHighlightsButton, &QPushButton::clicked, this, [=, &highlightedAreas]() {
        // Удаляем все выделенные области
        for (QCPItemRect *rect : highlightedAreas) {
            customPlot->removeItem(rect);
        }
        highlightedAreas.clear();
        customPlot->replot();
    });

    connect(closeButton, &QPushButton::clicked, plotDialog, &QDialog::reject);

    plotDialog->exec();
}

// НОВАЯ ФУНКЦИЯ ДЛЯ СОХРАНЕНИЯ ГРАФИКА
void ManualPlotDialog::savePlot(QCustomPlot *customPlot)
{
    // Диалог выбора файла
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Сохранить график",
        QDir::homePath() + "/BER_Graph_BCH127_64_10",
        "PNG Image (*.png);;JPEG Image (*.jpg *.jpeg);;PDF Document (*.pdf);;All Files (*)"
        );

    if (fileName.isEmpty()) {
        return; // Пользователь отменил сохранение
    }

    bool success = false;
    QString fileExtension = QFileInfo(fileName).suffix().toLower();

    // Сохраняем в зависимости от расширения
    if (fileExtension == "png") {
        success = customPlot->savePng(fileName, customPlot->width(), customPlot->height());
    }
    else if (fileExtension == "jpg" || fileExtension == "jpeg") {
        success = customPlot->saveJpg(fileName, customPlot->width(), customPlot->height());
    }
    else if (fileExtension == "pdf") {
        success = customPlot->savePdf(fileName, customPlot->width(), customPlot->height());
    }
    else {
        // По умолчанию сохраняем как PNG
        fileName += ".png";
        success = customPlot->savePng(fileName, customPlot->width(), customPlot->height());
    }

    // Показываем сообщение о результате
    if (success) {
        QMessageBox::information(this, "Успех",
                                 QString("График успешно сохранен в файл:\n%1").arg(fileName));
    } else {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось сохранить график. Проверьте права доступа к файлу.");
    }
}
