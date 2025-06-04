#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "canvaswidget.h"
#include "newcanvasdialog.h"

#include <QButtonGroup>
#include <QColorDialog>
#include <QFileDialog>
#include <QMenuBar>

// инициализация окна констркутор
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    


    setupUi();
    setupMenuBar();
    setupBrushStyles();
    setupDrawingTools();
    setupColorHistory();
    

    connect(ui->colorSelectGroup1, &QButtonGroup::buttonClicked,
            this, &MainWindow::colorSelectGroup_clicked);
    ui->colorIndicator->setStyleSheet("background-color: black");
    
    // сигналы
    connect(ui->undoBtn, &QPushButton::clicked, ui->canvasWidget, &CanvasWidget::undo);
    connect(ui->redoBtn, &QPushButton::clicked, ui->canvasWidget, &CanvasWidget::redo);
    connect(ui->canvasWidget, &CanvasWidget::undoAvailable, ui->undoBtn, &QPushButton::setEnabled);
    connect(ui->canvasWidget, &CanvasWidget::redoAvailable, ui->redoBtn, &QPushButton::setEnabled);
    
    // Размер холста
    ui->canvasWidget->setFixedSize(400, 600);
    adjustSize();
    setFixedSize(this->size());
}

MainWindow::~MainWindow() = default;

// удалить
void MainWindow::setupUi()
{
    if (ui->colorSelectGroup1->buttons().contains(ui->color_custom)) {
        ui->colorSelectGroup1->removeButton(ui->color_custom);
    }
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    QMenu* fileMenu = menuBar->addMenu(tr("&Файл"));
    
    auto newAct = new QAction(tr("&Новый"), this);
    newAct->setShortcuts(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &MainWindow::onNewCanvas);
    fileMenu->addAction(newAct);
    
    auto openAct = new QAction(tr("&Открыть..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::onOpenFile);
    fileMenu->addAction(openAct);
    
    auto saveAct = new QAction(tr("&Сохранить..."), this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::onSaveFile);
    fileMenu->addAction(saveAct);
}

void MainWindow::setupDrawingTools()
{

    auto drawModeGroup = findChild<QButtonGroup*>("drawModeGroup");
    if (!drawModeGroup) {
        drawModeGroup = new QButtonGroup(this);
    }

    if (auto brushBtn = findChild<QPushButton*>("brushBtn")) {
        drawModeGroup->addButton(brushBtn);
        brushBtn->setChecked(true);
    }
    if (auto lineBtn = findChild<QPushButton*>("lineBtn")) {
        drawModeGroup->addButton(lineBtn);
    }
    if (auto rectBtn = findChild<QPushButton*>("rectBtn")) {
        drawModeGroup->addButton(rectBtn);
    }
    if (auto ellipseBtn = findChild<QPushButton*>("ellipseBtn")) {
        drawModeGroup->addButton(ellipseBtn);
    }
    if (auto fillBtn = findChild<QPushButton*>("tool_fill")) {
        drawModeGroup->addButton(fillBtn);
    }

    // подключение
    connect(drawModeGroup, &QButtonGroup::buttonClicked, this, 
        [this](QAbstractButton* button) {
            if (!ui->canvasWidget) return;

            if (button->objectName() == "brushBtn") {
                ui->canvasWidget->setDrawMode(Canvas::DrawMode::FreeDraw);
            } else if (button->objectName() == "lineBtn") {
                ui->canvasWidget->setDrawMode(Canvas::DrawMode::Line);
            } else if (button->objectName() == "rectBtn") {
                ui->canvasWidget->setDrawMode(Canvas::DrawMode::Rectangle);
            } else if (button->objectName() == "ellipseBtn") {
                ui->canvasWidget->setDrawMode(Canvas::DrawMode::Ellipse);
            } else if (button->objectName() == "tool_fill") {
                ui->canvasWidget->setDrawMode(Canvas::DrawMode::Fill);
            }
        });
}

void MainWindow::setupBrushStyles()
{
    auto brushStyleGroup = findChild<QButtonGroup*>("brushStyleGroup");
    if (brushStyleGroup) {
        brushStyleGroup->setId(findChild<QPushButton*>("normalBtn"), static_cast<int>(Canvas::BrushStyle::Normal));
        brushStyleGroup->setId(findChild<QPushButton*>("sprayBtn"), static_cast<int>(Canvas::BrushStyle::SprayPaint));
        brushStyleGroup->setId(findChild<QPushButton*>("neonBtn"), static_cast<int>(Canvas::BrushStyle::Neon));
        
        connect(brushStyleGroup, &QButtonGroup::idClicked,
                [this](int id) { ui->canvasWidget->setBrushStyle(static_cast<Canvas::BrushStyle>(id)); });
    }
}

// история цветов, выбор и так далее - вся логика отображения и обновления цветовой панели
void MainWindow::setupColorHistory()
{
    colorHistory.resize(MAX_HISTORY_COLORS);
    historyButtons.resize(MAX_HISTORY_COLORS);
    
    historyButtons = {
        ui->historyBtn1, ui->historyBtn2, ui->historyBtn3,
        ui->historyBtn4, ui->historyBtn5
    };
    
    for (auto btn : historyButtons) {
        connect(btn, &QPushButton::clicked, this, &MainWindow::onHistoryColorClicked);
    }
}

void MainWindow::updateHistoryButton(int index, const QColor& color)
{
    if (index >= 0 && index < historyButtons.size()) {
        historyButtons[index]->setStyleSheet(QString("background-color: %1").arg(color.name()));
        historyButtons[index]->setEnabled(true);
    }
}

void MainWindow::addToColorHistory(const QColor& color)
{
    if (!colorHistory.isEmpty() && colorHistory[0] == color) return;
    
    for (int i = colorHistory.size() - 1; i > 0; --i) {
        colorHistory[i] = colorHistory[i-1];
        updateHistoryButton(i, colorHistory[i]);
    }
    
    colorHistory[0] = color;
    updateHistoryButton(0, color);
}

void MainWindow::onHistoryColorClicked()
{
    auto button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    int index = historyButtons.indexOf(button);
    if (index >= 0 && index < colorHistory.size()) {
        QColor color = colorHistory[index];
        ui->colorIndicator->setStyleSheet("background-color: " + color.name());
        ui->canvasWidget->setColor(color);
    }
}

void MainWindow::colorSelectGroup_clicked(QAbstractButton* button)
{
    QString buttonName = button->objectName();
    QColor selectedColor;

    if (buttonName == "color_red") selectedColor = Qt::red;
    else if (buttonName == "color_blue") selectedColor = Qt::blue;
    else if (buttonName == "color_green") selectedColor = Qt::darkGreen;
    else if (buttonName == "color_yellow") selectedColor = Qt::yellow;
    else if (buttonName == "color_black") selectedColor = Qt::black;
    else return;

    ui->colorIndicator->setStyleSheet("background-color: " + selectedColor.name());
    ui->canvasWidget->setColor(selectedColor);
    addToColorHistory(selectedColor);
}

void MainWindow::on_color_custom_clicked()
{
    QColor customColor = QColorDialog::getColor(Qt::white, this, tr(""), QColorDialog::ShowAlphaChannel);
    if (customColor.isValid()) {
        ui->colorIndicator->setStyleSheet("background-color: " + customColor.name());
        ui->canvasWidget->setColor(customColor);
        addToColorHistory(customColor);
    }
}







// размер кисти
void MainWindow::on_brushSize_slider_valueChanged(int value)
{
    ui->canvasWidget->setBrushSize(value);
    ui->brushSize_label->setText(QString::number(value));
}

void MainWindow::on_brushSize_slider_sliderMoved(int position)
{
    ui->canvasWidget->setBrushSize(position);
    ui->brushSize_label->setText(QString::number(position));
}

void MainWindow::on_brushSize_slider_sliderReleased()
{
    int value = ui->brushSize_slider->value();
    ui->canvasWidget->setBrushSize(value);
    ui->brushSize_label->setText(QString::number(value));
}

// Манипуляция с файлом открыть закрыть новый
void MainWindow::onNewCanvas()
{
    NewCanvasDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QSize size = dialog.getCanvasSize();
        ui->canvasWidget->clearAll();
        ui->canvasWidget->resizeCanvas(size);
        adjustSize();
        setFixedSize(this->size());
    }
}

void MainWindow::onOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Открыть изображение"), "",
        tr("Файлы изображений (*.png *.jpg *.jpeg *.bmp);;Все файлы (*)"));
        
    if (!fileName.isEmpty()) {
        QImage img(fileName);
        if (!img.isNull()) {
            ui->canvasWidget->loadImage(img);
            adjustSize();
            setFixedSize(this->size());
        }
    }
}

void MainWindow::onSaveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Сохранить изображение"), "",
        tr("PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)"));
        
    if (!fileName.isEmpty()) {
        ui->canvasWidget->getImage().save(fileName);
    }
}
//ОЧИСТКА
void MainWindow::on_clearAll_clicked()
{
    ui->canvasWidget->clearAll();
}

