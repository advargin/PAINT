#include "canvas.h"
#include <QRandomGenerator>
#include <QtMath>

// конструктор/деструктор
Canvas::Canvas(QObject *parent)
    : QObject(parent)
    , drawingTools(this)
{
    originalImage = QImage();
}

Canvas::~Canvas() = default;

// создаем
void Canvas::create(const QSize& size)
{
    image = QImage(size, QImage::Format_RGB32);
    image.fill(Qt::white);
    emit canvasChanged();
}

// очищаем
void Canvas::clear()
{
    saveState();
    image.fill(Qt::white);
    emit canvasChanged();
}

// меняем размер холста, не удаляя содержимое
void Canvas::resize(const QSize& newSize)
{
    saveState();
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(Qt::white);
    
    if (!image.isNull()) {
        QPainter painter(&newImage);
        painter.drawImage(QPoint(0, 0), image);
    }
    
    image = newImage;
    emit canvasChanged();
}


void Canvas::load(const QImage& newImage)
{
    saveState();
    image = newImage;
    emit canvasChanged();
}

//простые функции
QImage Canvas::getImage() const
{
    return image;
}

QSize Canvas::getSize() const
{
    return image.size();
}


void Canvas::setColor(const QColor& newColor)
{
    color = newColor;
}


void Canvas::setBrushSize(int size)
{
    brushSize = size;
}

void Canvas::setDrawMode(DrawMode mode)
{
    drawMode = mode;
}

void Canvas::setBrushStyle(BrushStyle style)
{
    brushStyle = style;
}

// начало рисования
void Canvas::startDrawing(const QPoint& point)
{
    if (!image.isNull() && point.x() >= 0 && point.y() >= 0 &&
        point.x() < image.width() && point.y() < image.height()) {
        isDrawing = true;
        startPoint = point;
        lastPoint = point;

        if (drawMode == DrawMode::Fill) {
            QPainter painter(&image);
            drawingTools.fillArea(painter, {color, brushSize, drawMode, brushStyle, startPoint, lastPoint}, image, point);
            isDrawing = false;
            saveState();
            emit canvasChanged();

        } else if (drawMode == DrawMode::FreeDraw) {
            saveState();
            QPainter painter(&image);
            DrawingTools::DrawingContext context{color, brushSize, drawMode, brushStyle, startPoint, lastPoint};
            drawingTools.drawFreely(painter, context, point);
            emit canvasChanged();

        } else {
            originalImage = image.copy();
        }
    }
}

// во время рисования (превью зависит от инструмента)
void Canvas::continueDrawing(const QPoint& point)
{
    if (!isDrawing || image.isNull()) return;
    
    if (drawMode == DrawMode::FreeDraw) {
        QPainter painter(&image);
        DrawingTools::DrawingContext context{color, brushSize, drawMode, brushStyle, lastPoint, point};
        drawingTools.drawFreely(painter, context, point);
        lastPoint = point;
        emit canvasChanged();
    } else if (drawMode != DrawMode::Fill) {

        image = originalImage.copy();
        QPainter painter(&image);
        lastPoint = point;
        DrawingTools::DrawingContext context{color, brushSize, drawMode, brushStyle, startPoint, lastPoint};
        
        switch (drawMode) {
            case DrawMode::Line:
                drawingTools.drawLine(painter, context);
                break;
            case DrawMode::Rectangle:
                drawingTools.drawRectangle(painter, context);
                break;
            case DrawMode::Ellipse:
                drawingTools.drawEllipse(painter, context);
                break;
            default:
                break;
        }
        
        emit canvasChanged();
    }
}

// окнец рисования (отпускаем кнопку)
void Canvas::endDrawing(const QPoint& point)
{
    if (!isDrawing) return;
    
    lastPoint = point;
    isDrawing = false;
    
    if (drawMode != DrawMode::FreeDraw && drawMode != DrawMode::Fill) {


        image = originalImage.copy();
        saveState();
        QPainter painter(&image);
        DrawingTools::DrawingContext context{color, brushSize, drawMode, brushStyle, startPoint, lastPoint};
        
        switch (drawMode) {
            case DrawMode::Line:
                drawingTools.drawLine(painter, context);
                break;
            case DrawMode::Rectangle:
                drawingTools.drawRectangle(painter, context);
                break;
            case DrawMode::Ellipse:
                drawingTools.drawEllipse(painter, context);
                break;
            default:
                break;
        }
        emit canvasChanged();
    }
}

//сохраняем состояние чтобы можно было отменить/повторить
void Canvas::saveState()
{
    if (undoStack.size() >= MAX_HISTORY) {
        undoStack.removeFirst();
    }
    undoStack.push(image);
    redoStack.clear();
    
    emit undoAvailable(!undoStack.isEmpty());
    emit redoAvailable(!redoStack.isEmpty());
}


void Canvas::undo()
{
    if (undoStack.isEmpty()) return;
    
    redoStack.push(image);
    image = undoStack.pop();
    
    emit undoAvailable(!undoStack.isEmpty());
    emit redoAvailable(!redoStack.isEmpty());
    emit canvasChanged();
}

void Canvas::redo()
{
    if (redoStack.isEmpty()) return;
    
    undoStack.push(image);
    image = redoStack.pop();
    
    emit undoAvailable(!undoStack.isEmpty());
    emit redoAvailable(!redoStack.isEmpty());
    emit canvasChanged();
}

//проверяем, есть ли сохраненные состояния вперед/назад.......
bool Canvas::canUndo() const
{
    return !undoStack.isEmpty();
}
bool Canvas::canRedo() const
{
    return !redoStack.isEmpty();
} 
