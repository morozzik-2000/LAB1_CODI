#include "CodecDiagramWidget.h"
#include <QPainter>
#include <QtMath>
#include <QPainterPath>

CodecDiagramWidget::CodecDiagramWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(130);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void CodecDiagramWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 2));

    // === Шрифты ===
    QFont formulaFont = painter.font();
    formulaFont.setPointSize(12);
    formulaFont.setBold(QFont::DemiBold);

    QFont boxFont = painter.font();
    boxFont.setPointSize(10);
    boxFont.setBold(QFont::Medium);

    int width = this->width();
    int height = this->height();
    int centerY = height / 2;

    int partWidth = width / 4;
    int rectWidth = 90;
    int rectHeight = 50;

    int coderX = partWidth;
    int channelX = partWidth * 2;
    int decoderX = partWidth * 3;

    // === Прямоугольники ===
    QRect coderRect(coderX - rectWidth/2, centerY - rectHeight/2, rectWidth, rectHeight);
    QRect channelRect(channelX - rectWidth/2, centerY - rectHeight/2, rectWidth, rectHeight);
    QRect decoderRect(decoderX - rectWidth/2, centerY - rectHeight/2, rectWidth, rectHeight);

    painter.setFont(boxFont);
    painter.drawRect(coderRect);
    painter.drawText(coderRect, Qt::AlignCenter, "Кодер");

    painter.drawRect(channelRect);
    painter.drawText(channelRect, Qt::AlignCenter, "Канал\n(ДСК)");

    painter.drawRect(decoderRect);
    painter.drawText(decoderRect, Qt::AlignCenter, "Декодер");

    // === Стрелки между блоками ===
    int arrowStartX, arrowEndX;
    arrowStartX = 20;
    arrowEndX = coderX - rectWidth/2;
    drawArrow(painter, arrowStartX, centerY, arrowEndX, centerY);

    arrowStartX = coderX + rectWidth/2;
    arrowEndX = channelX - rectWidth/2;
    drawArrow(painter, arrowStartX, centerY, arrowEndX, centerY);

    arrowStartX = channelX + rectWidth/2;
    arrowEndX = decoderX - rectWidth/2;
    drawArrow(painter, arrowStartX, centerY, arrowEndX, centerY);

    arrowStartX = decoderX + rectWidth/2;
    arrowEndX = width - 20;
    drawArrow(painter, arrowStartX, centerY, arrowEndX, centerY);

    // === Подписи над стрелками ===
    painter.setFont(formulaFont);

    auto drawCenteredAt = [&](int x1, int x2, const std::function<void(QPainter&, QPointF)>& fn) {
        QPointF center((x1 + x2) / 2.0, centerY - 35); // y-координата подписи
        fn(painter, center);
    };

    // m⃗
    drawCenteredAt(20, coderX - rectWidth/2, [&](QPainter &p, QPointF c) {
        drawVectorText(p, c, "m");
    });

    // u⃗
    drawCenteredAt(coderX + rectWidth/2, channelX - rectWidth/2, [&](QPainter &p, QPointF c) {
        drawVectorText(p, c, "u");
    });

    // r⃗ = u⃗ + e⃗  <-- аккуратно: рисуем элементы по очереди с нормальными знаками
    drawCenteredAt(channelX + rectWidth/2, decoderX - rectWidth/2, [&](QPainter &p, QPointF c) {
        // используем локальный painter state
        p.save();
        QFontMetrics fm(p.font());

        // Определим ширины элементов: буквы с вектором рисуем отдельно, но чтобы не смешивать стрелки,
        // сначала измерим текстовую ширину лазейкой через ширину буквы в том же шрифте.
        int w_r = fm.horizontalAdvance("r");
        int w_u = fm.horizontalAdvance("u");
        int w_e = fm.horizontalAdvance("e");
        int w_eq = fm.horizontalAdvance("="); // равно
        int w_plus = fm.horizontalAdvance("+");

        // Задаём промежутки
        int gap = qMax(4, fm.horizontalAdvance(" ")/2);

        // Начальная позиция так, чтобы всё было по центру:
        qreal totalWidth = (w_r + w_u + w_e) + (w_eq + w_plus) + 4*gap;
        QPointF pos = QPointF(c.x() - totalWidth/2.0, c.y());

        // r⃗
        drawVectorText(p, pos + QPointF(w_r/2.0, 0), "r");
        pos.rx() += w_r + gap;

        // =
        p.setPen(QPen(Qt::black, 1));
        p.drawText(pos + QPointF(0, fm.ascent()/2.5), "=");
        pos.rx() += w_eq + gap;

        // u⃗
        drawVectorText(p, pos + QPointF(w_u/2.0, 0), "u");
        pos.rx() += w_u + gap;

        // +
        p.setPen(QPen(Qt::black, 1));
        p.drawText(pos + QPointF(0, fm.ascent()/2.5), "+");
        pos.rx() += w_plus + gap;

        // e⃗
        drawVectorText(p, pos + QPointF(w_e/2.0, 0), "e");

        p.restore();
    });

    // m̂ (с крышечкой)
    drawCenteredAt(decoderX + rectWidth/2, width - 20, [&](QPainter &p, QPointF c) {
        drawHatText(p, c, "m");
    });
}

void CodecDiagramWidget::drawArrow(QPainter &painter, int x1, int y1, int x2, int y2)
{
    painter.save();
    QPen pen(Qt::black);
    pen.setWidth(2);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    painter.drawLine(x1, y1, x2, y2);

    // аккуратный наконечник (две короткие линии forming a V)
    double angle = atan2(y2 - y1, x2 - x1);
    const int headLen = 10;
    QPointF p1 = QPointF(x2, y2);
    QPointF h1 = QPointF(x2 - headLen * cos(angle - M_PI/6), y2 - headLen * sin(angle - M_PI/6));
    QPointF h2 = QPointF(x2 - headLen * cos(angle + M_PI/6), y2 - headLen * sin(angle + M_PI/6));
    painter.drawLine(p1, h1);
    painter.drawLine(p1, h2);

    painter.restore();
}

// Рисуем жирную букву с аккуратной векторной стрелкой над ней.
// center.x() и center.y() — центр области, где будет текст (по вертикали - baseline ориентированно).
void CodecDiagramWidget::drawVectorText(QPainter &p, QPointF center, const QString &letter)
{
    p.save();

    QFont font = p.font();
    font.setBold(QFont::DemiBold);
    p.setFont(font);
    QFontMetrics fm(font);

    // Текст позиционируем по baseline: вычисляем x,y для drawText
    int tw = fm.horizontalAdvance(letter);
    qreal textX = center.x() - tw / 2.0;
    qreal textY = center.y() + fm.ascent() / 2.5; // вертикальная поправка чтобы текст выглядел в центре области подписи
    QPointF textPos(textX, textY);

    // Рисуем букву
    p.setPen(QPen(Qt::black, 1));
    p.drawText(textPos, letter);

    // Рисуем стрелку над буквой: делаем её длиннее, выше и тоньше, чтобы не накладываться на текст
    qreal arrowTopY = textPos.y() - fm.ascent() - 3.0; // 3 пикс расстояния над буквой
    qreal arrowLeftX = center.x() - tw * 0.5 - 2; // немного шире буквы
    qreal arrowRightX = center.x() + tw * 0.5 + 2;

    QPen arrowPen(Qt::black);
    arrowPen.setWidth(2); // толщина линии стрелки (немного жирнее, чем формульный текст)
    arrowPen.setCapStyle(Qt::RoundCap);
    p.setPen(arrowPen);

    // линия стрелки
    p.drawLine(QPointF(arrowLeftX, arrowTopY), QPointF(arrowRightX, arrowTopY));

    // наконечник (V)
    double headLen = 8.0;
    // направлен вправо (стрелка направлена вправо в нашей диаграмме)
    QPointF tip(arrowRightX, arrowTopY);
    double angle = 0; // 0 radians -> вправо
    QPointF h1 = QPointF(tip.x() - headLen * cos(angle - M_PI/6), tip.y() - headLen * sin(angle - M_PI/6));
    QPointF h2 = QPointF(tip.x() - headLen * cos(angle + M_PI/6), tip.y() - headLen * sin(angle + M_PI/6));
    p.drawLine(tip, h1);
    p.drawLine(tip, h2);

    p.restore();
}

// Рисуем букву с "крышечкой" (hat) над ней
void CodecDiagramWidget::drawHatText(QPainter &p, QPointF center, const QString &letter)
{
    p.save();

    QFont font = p.font();
    font.setBold(QFont::DemiBold);
    p.setFont(font);
    QFontMetrics fm(font);

    int tw = fm.horizontalAdvance(letter);
    qreal textX = center.x() - tw / 2.0;
    qreal textY = center.y() + fm.ascent() / 2.5;
    QPointF textPos(textX, textY);

    p.setPen(QPen(Qt::black, 1));
    p.drawText(textPos, letter);

    // крышечка (hat)
    qreal hatY = textPos.y() - fm.ascent() - 3.0;
    qreal hatW = tw * 0.8;
    qreal left = center.x() - hatW/2.0;
    qreal right = center.x() + hatW/2.0;
    QPointF top(center.x(), hatY - 3.0);

    QPen hatPen(Qt::black);
    hatPen.setWidth(2);
    p.setPen(hatPen);

    QPainterPath hat;
    hat.moveTo(left, hatY + 3.0);
    hat.lineTo(top);
    hat.lineTo(right, hatY + 3.0);
    p.drawPath(hat);

    p.restore();
}
