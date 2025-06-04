#include "newcanvasdialog.h"
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>


NewCanvasDialog::NewCanvasDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Новый холст"));


    widthSpinBox = new QSpinBox(this);
    heightSpinBox = new QSpinBox(this);


    widthSpinBox->setRange(1, 3000);
    heightSpinBox->setRange(1, 3000);
    widthSpinBox->setValue(400);
    heightSpinBox->setValue(600);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *sizeLayout = new QHBoxLayout;


    sizeLayout->addWidget(new QLabel(tr("Ширина:")));
    sizeLayout->addWidget(widthSpinBox);
    sizeLayout->addWidget(new QLabel(tr("Высота:")));
    sizeLayout->addWidget(heightSpinBox);


    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this);


    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);


    mainLayout->addLayout(sizeLayout);
    mainLayout->addWidget(buttonBox);
}


QSize NewCanvasDialog::getCanvasSize() const
{
    return QSize(widthSpinBox->value(), heightSpinBox->value());
} 
