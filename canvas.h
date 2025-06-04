#ifndef CANVAS_H
#define CANVAS_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <QPoint>
#include <QStack>
#include <QPainter>
#include "drawingtools.h"

//  класс canvas управляет поверхностью рисования и взаимодействием с пользователем
class Canvas : public QObject
{
    Q_OBJECT

public:
//псевдонимы для инструментов и стилей кисти
    using DrawMode = DrawingTools::Mode;
    using BrushStyle = DrawingTools::BrushStyle;




    explicit Canvas(QObject *parent = nullptr);
    ~Canvas();

    void create(const QSize& size);
    void clear();


    void resize(const QSize& newSize);

    void load(const QImage& image);

    QImage getImage() const;
    QSize getSize() const;

 // ТЕКУЩИе параметры рисования
    void setColor(const QColor& color);

    void setBrushSize(int size);
    void setDrawMode(DrawMode mode);

    void setBrushStyle(BrushStyle style);


    void startDrawing(const QPoint& point);
    void continueDrawing(const QPoint& point);
    void endDrawing(const QPoint& point);





    bool canUndo() const;

    bool canRedo() const;

    void undo();
    void redo();

signals:

    void undoAvailable(bool available);
    void redoAvailable(bool available);
    void canvasChanged();

private:
//история
    static const int MAX_HISTORY = 5;
    
//ЭТО ТЕКУЩИЙ ХОЛСТ
    QImage image;
    QImage originalImage;
    bool isDrawing = false;
    QPoint startPoint;
    QPoint lastPoint;
    
// свойства кисти
    QColor color = Qt::black;
    int brushSize = 5;
    DrawMode drawMode = DrawMode::FreeDraw;
    BrushStyle brushStyle = BrushStyle::Normal;
    


    QStack<QImage> undoStack;
    QStack<QImage> redoStack;
    DrawingTools drawingTools;

    void saveState(); //Сохраняем состояние
};

#endif
