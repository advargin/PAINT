#ifndef DRAWINGTOOLS_H
#define DRAWINGTOOLS_H

#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QPen>
#include <QLineF>
#include <QQueue>

// вся логика самого рисования
class DrawingTools : public QObject
{
    Q_OBJECT

public:

    enum class Mode {
        FreeDraw,
        Line,
        Rectangle,
        Ellipse,
        Fill
    };


    enum class BrushStyle {
        Normal,
        SprayPaint,
        Neon
    };

    // контекст чтобы узнать как мы будем рисовать
    struct DrawingContext {
        QColor color;
        int brushSize;
        Mode mode;
        BrushStyle brushStyle;
        QPoint startPoint;
        QPoint lastPoint;
    };

    explicit DrawingTools(QObject *parent = nullptr);
    ~DrawingTools();


    void drawFreely(QPainter& painter, const DrawingContext& context, const QPoint& currentPoint) const;


    void drawLine(QPainter& painter, const DrawingContext& context) const;


    void drawRectangle(QPainter& painter, const DrawingContext& context) const;


    void drawEllipse(QPainter& painter, const DrawingContext& context) const;


    void fillArea(QPainter& painter, const DrawingContext& context, const QImage& sourceImage, const QPoint& startPoint) const;

private:
//ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ ЧТОБЫ ПОНЯТЬ КАК РИСОВАТЬ В КОНКРЕТНОМ СТИЛЕ
    void applyNormalStyle(QPainter& painter, const DrawingContext& context, const QPoint& from, const QPoint& to) const;
    void applySprayStyle(QPainter& painter, const DrawingContext& context, const QPoint& from, const QPoint& to) const;
    void applyNeonStyle(QPainter& painter, const DrawingContext& context, const QPoint& from, const QPoint& to) const;
    

    void drawStyledSegment(QPainter& painter, const DrawingContext& context, const QPoint& from, const QPoint& to) const;
    void approximateEllipse(QPainter& painter, const DrawingContext& context, const QRect& rect, int segments = 72) const;


    void floodFill(QImage& image, const QPoint& startPoint, const QColor& targetColor, const QColor& replacementColor) const;
};

#endif
