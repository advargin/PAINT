#include "drawingtools.h"
#include <QRandomGenerator>
#include <QtMath>
#include <QPen>
#include <QLineF>
#include <QPointF>
#include <QPainterPath>
#include <QRectF>
#include <QImage>
#include <QQueue>

using Qt::SolidLine;
using Qt::RoundCap;
using Qt::RoundJoin;


DrawingTools::DrawingTools(QObject *parent)
    : QObject(parent)
{
}
// конструктор/дестркутор
DrawingTools::~DrawingTools() = default;


void DrawingTools::drawFreely(QPainter& painter, const DrawingContext& context, const QPoint& currentPoint) const
{
    drawStyledSegment(painter, context, context.lastPoint, currentPoint);
}


void DrawingTools::drawLine(QPainter& painter, const DrawingContext& context) const
{
    drawStyledSegment(painter, context, context.startPoint, context.lastPoint);
}


void DrawingTools::drawRectangle(QPainter& painter, const DrawingContext& context) const
{
    QRect rect(context.startPoint, context.lastPoint);
    QPoint topRight(rect.right(), rect.top());
    QPoint bottomLeft(rect.left(), rect.bottom());
    QPoint bottomRight(rect.right(), rect.bottom());



    drawStyledSegment(painter, context, context.startPoint, topRight);
    drawStyledSegment(painter, context, topRight, bottomRight);
    drawStyledSegment(painter, context, bottomRight, bottomLeft);
    drawStyledSegment(painter, context, bottomLeft, context.startPoint);
}


void DrawingTools::drawEllipse(QPainter& painter, const DrawingContext& context) const
{
    QRect rect(context.startPoint, context.lastPoint);
    
    if (context.brushStyle == BrushStyle::Normal) {

        applyNormalStyle(painter, context, rect.topLeft(), rect.topLeft());
        painter.drawEllipse(rect);

    } else {

        approximateEllipse(painter, context, rect);
    }
}


void DrawingTools::drawStyledSegment(QPainter& painter, const DrawingContext& context, const QPoint& from, const QPoint& to) const
{
    switch (context.brushStyle) {
        case BrushStyle::Normal:
            applyNormalStyle(painter, context, from, to);
            break;

        case BrushStyle::SprayPaint:
            applySprayStyle(painter, context, from, to);
            break;

        case BrushStyle::Neon:
            applyNeonStyle(painter, context, from, to);
            break;
    }
}


void DrawingTools::applyNormalStyle(QPainter& painter, const DrawingContext& context, const QPoint& from, const QPoint& to) const
{
    painter.setPen(QPen(context.color, context.brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (from == to) {
        painter.drawPoint(from);
    } else {
        painter.drawLine(from, to);
    }
}


void DrawingTools::applySprayStyle(QPainter& painter, const DrawingContext& context, const QPoint& from, const QPoint& to) const
{
    painter.setPen(context.color);
    
    if (from == to) {

        const int density = context.brushSize * 2;
        const int radius = context.brushSize;
        
        for (int i = 0; i < density; ++i) {
            double angle = QRandomGenerator::global()->generateDouble() * 2 * M_PI;
            double distance = QRandomGenerator::global()->generateDouble() * radius;
            
            int dx = static_cast<int>(distance * cos(angle));
            int dy = static_cast<int>(distance * sin(angle));
            
            painter.drawPoint(from.x() + dx, from.y() + dy);
        }
    } else {

        QLineF line(from, to);
        int steps = qMax(1, static_cast<int>(line.length() / (context.brushSize/2)));
        
        for (int i = 0; i <= steps; ++i) {
            QPointF basePoint = line.pointAt(i / static_cast<float>(steps));
            const int density = context.brushSize;
            const int radius = context.brushSize / 2;
            
            for (int j = 0; j < density; ++j) {
                double angle = QRandomGenerator::global()->generateDouble() * 2 * M_PI;
                double distance = QRandomGenerator::global()->generateDouble() * radius;
                
                int dx = static_cast<int>(distance * cos(angle));
                int dy = static_cast<int>(distance * sin(angle));
                
                painter.drawPoint(basePoint.x() + dx, basePoint.y() + dy);
            }
        }
    }
}


void DrawingTools::applyNeonStyle(QPainter& painter, const DrawingContext& context, const QPoint& from, const QPoint& to) const
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setCompositionMode(QPainter::CompositionMode_Plus);
    

    int r = context.color.red();
    int g = context.color.green();
    int b = context.color.blue();
    

    qreal coreSize = context.brushSize * 0.4;
    qreal stepSize = (context.brushSize - coreSize) / 3.0;
    

    for (int layer = 3; layer >= 0; --layer) {
        qreal currentSize = coreSize + (layer * stepSize);
        QColor layerColor = (layer == 0) ? context.color : QColor(
            qMin(255, r + (layer * 40)),
            qMin(255, g + (layer * 40)),
            qMin(255, b + (layer * 40)),
            60 - (layer * 15)
        );
        
        QPen pen(layerColor, currentSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        
        if (from == to) {
            painter.drawPoint(from);
        } else {
            QPainterPath path;
            path.moveTo(from);
            path.lineTo(to);
            painter.strokePath(path, pen);
        }
    }
    
    painter.restore();
}


void DrawingTools::approximateEllipse(QPainter& painter, const DrawingContext& context, const QRect& rect, int segments) const
{
    for (int i = 0; i < segments; ++i) {
        double angle = (i * 2 * M_PI) / segments;
        double nextAngle = ((i + 1) * 2 * M_PI) / segments;
        
        int x1 = rect.center().x() + (rect.width() / 2.0) * cos(angle);
        int y1 = rect.center().y() + (rect.height() / 2.0) * sin(angle);
        int x2 = rect.center().x() + (rect.width() / 2.0) * cos(nextAngle);
        int y2 = rect.center().y() + (rect.height() / 2.0) * sin(nextAngle);
        
        drawStyledSegment(painter, context, QPoint(x1, y1), QPoint(x2, y2));
    }
}

void DrawingTools::fillArea(QPainter& painter, const DrawingContext& context, const QImage& sourceImage, const QPoint& startPoint) const
{
    if (!startPoint.isNull() && startPoint.x() >= 0 && startPoint.y() >= 0 && 
        startPoint.x() < sourceImage.width() && startPoint.y() < sourceImage.height()) {
        QImage workingImage = sourceImage.copy();
        QColor targetColor = workingImage.pixelColor(startPoint);
        

        if (targetColor == context.color) {
            return;
        }

        floodFill(workingImage, startPoint, targetColor, context.color);
        painter.drawImage(0, 0, workingImage);
    }
}

void DrawingTools::floodFill(QImage& image, const QPoint& startPoint, const QColor& targetColor, const QColor& replacementColor) const
{
    if (targetColor == replacementColor) {
        return;
    }

    QQueue<QPoint> queue;
    queue.enqueue(startPoint);

    while (!queue.isEmpty()) {
        QPoint current = queue.dequeue();
        int x = current.x();
        int y = current.y();

        if (x < 0 || x >= image.width() || y < 0 || y >= image.height()) {
            continue;
        }

        if (image.pixelColor(x, y) != targetColor) {
            continue;
        }

        image.setPixelColor(x, y, replacementColor);


        queue.enqueue(QPoint(x + 1, y));
        queue.enqueue(QPoint(x - 1, y));
        queue.enqueue(QPoint(x, y + 1));
        queue.enqueue(QPoint(x, y - 1));
    }
} 
