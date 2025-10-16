#ifndef CODECDIAGRAMWIDGET_H
#define CODECDIAGRAMWIDGET_H

#include <QWidget>

class CodecDiagramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CodecDiagramWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawArrow(QPainter &painter, int x1, int y1, int x2, int y2);
    void drawVectorText(QPainter &p, QPointF center, const QString &letter);
    void drawHatText(QPainter &p, QPointF center, const QString &letter);

};

#endif // CODECDIAGRAMWIDGET_H
