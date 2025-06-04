#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QImage>
#include "canvas.h"

// виджет холста, тут отработка действий маши
class CanvasWidget : public QWidget
{
    Q_OBJECT
public:
//РЕЖИМЫ
    enum DrawMode {
        FreeDraw,
        Line,
        Rectangle,
        Ellipse
    };

//СТИЛИ
    enum BrushStyle {
        Normal,
        SprayPaint,
        Neon
    };

    // Конструктор и деструктор
    explicit CanvasWidget(QWidget *parent = nullptr);
    ~CanvasWidget();

    // события мыши
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;


    void setColor(const QColor& selectedColor);
    void setBrushSize(int selectedSize);
    void setDrawMode(Canvas::DrawMode mode);
    void setBrushStyle(Canvas::BrushStyle style);


    void clearAll();
    void loadImage(const QImage& image);
    void resizeCanvas(const QSize& newSize);
    QImage getImage();


    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();

signals:
    void undoAvailable(bool available);
    void redoAvailable(bool available);

private:
    Canvas* canvas;
};

#endif
