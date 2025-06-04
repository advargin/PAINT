#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QAbstractButton>
#include <QtCore/QVector>
#include <QtGui/QColor>


class CanvasWidget;

namespace Ui {
class MainWindow;
}

//главное окно
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // констркутор/дестркутор
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    // цвет
    void colorSelectGroup_clicked(QAbstractButton* button);
    void on_color_custom_clicked();
    void onHistoryColorClicked();
    
    // размер кисти
    void on_brushSize_slider_valueChanged(int value);
    void on_brushSize_slider_sliderMoved(int position);
    void on_brushSize_slider_sliderReleased();
    
    // действия с файлом
    void onNewCanvas();
    void onOpenFile();
    void onSaveFile();
    void on_clearAll_clicked();

private:
    Ui::MainWindow *ui;
    QVector<QColor> colorHistory;
    QVector<QPushButton*> historyButtons;

    static const int MAX_HISTORY_COLORS = 5;
// СЕТАП интерфейса
    void setupUi();
    void setupMenuBar();
    void setupColorHistory();
    void setupDrawingTools();
    void setupBrushStyles();




    void addToColorHistory(const QColor& color);
    void updateHistoryButton(int index, const QColor& color);
};

#endif
