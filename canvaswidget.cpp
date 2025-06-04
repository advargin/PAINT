#include "canvaswidget.h"
#include <QMouseEvent>
#include <QPainter>

//конструктор
CanvasWidget::CanvasWidget(QWidget *parent)
    : QWidget(parent)
{
    // Создание объекта холста и подключение сигналов
    canvas = new Canvas(this);
    connect(canvas, &Canvas::undoAvailable, this, &CanvasWidget::undoAvailable);
    connect(canvas, &Canvas::redoAvailable, this, &CanvasWidget::redoAvailable);
    connect(canvas, &Canvas::canvasChanged, this, [this]() { update(); });
    
    // Установка начального размера и создание холста
    setFixedSize(400, 600);
    canvas->create(size());
}

CanvasWidget::~CanvasWidget() = default;

// передаем информацию в canvas о том что присходит с мышкой

void CanvasWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        canvas->startDrawing(event->pos());
        update();
    }
}


void CanvasWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        canvas->continueDrawing(event->pos());
        update();
    }
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        canvas->endDrawing(event->pos());
        update();
    }
}


// (событие рисования)
void CanvasWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(event->rect(), canvas->getImage(), event->rect());
}


//передаем в canvas все о параметрах кисти
void CanvasWidget::setColor(const QColor& selectedColor)
{
    canvas->setColor(selectedColor);
}

void CanvasWidget::setBrushSize(int selectedSize)
{
    canvas->setBrushSize(selectedSize);
}

void CanvasWidget::setDrawMode(Canvas::DrawMode mode)
{
    canvas->setDrawMode(mode);
}

void CanvasWidget::setBrushStyle(Canvas::BrushStyle style)
{
    canvas->setBrushStyle(style);
}




void CanvasWidget::clearAll()
{
    canvas->clear();
}


//ОПЕРАЦИИ С ХОЛСТОМ
void CanvasWidget::loadImage(const QImage& image)
{
    canvas->load(image);
    setFixedSize(image.size());
}

void CanvasWidget::resizeCanvas(const QSize& newSize)
{
    canvas->resize(newSize);
    setFixedSize(newSize);
}

QImage CanvasWidget::getImage()
{
    return canvas->getImage();
}


// общение с canvas по поводу UNDO/REDO

bool CanvasWidget::canUndo() const
{
    return canvas->canUndo();
}
bool CanvasWidget::canRedo() const
{
    return canvas->canRedo();
}
void CanvasWidget::undo()
{
    canvas->undo();
}
void CanvasWidget::redo()
{
    canvas->redo();
}


