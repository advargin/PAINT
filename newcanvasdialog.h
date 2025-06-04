#ifndef NEWCANVASDIALOG_H
#define NEWCANVASDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


class NewCanvasDialog : public QDialog
{
    Q_OBJECT

public:

    explicit NewCanvasDialog(QWidget *parent = nullptr);
    QSize getCanvasSize() const;

private:
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;
};

#endif
