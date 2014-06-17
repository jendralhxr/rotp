#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QWidget>
#include "rotspinbox.h"
#include "rotimage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ROTimage *tes = new ROTimage();
//    tes->openFilename();

    ROTSpinBox *spinbox_Xstart = new ROTSpinBox();
    ROTSpinBox *spinbox_Xend = new ROTSpinBox();
    ROTSpinBox *spinbox_Ystart = new ROTSpinBox();
    ROTSpinBox *spinbox_Yend = new ROTSpinBox();
    QLabel *label_start = new QLabel("Grabut Start Point (x,y)");
    QLabel *label_end = new QLabel("Grabut End Point (x,y)");

    QPushButton *button_file = new QPushButton("Open Image");
    QPushButton *button_grabcut = new QPushButton("Apply Grabcut");
    QPushButton *button_otsu = new QPushButton("Apply Grayscale and Otsu Threshold");
    QPushButton *button_overlay = new QPushButton("Draw Rule of Thirds Overlay");
    QPushButton *button_rot = new QPushButton("Rule of Thirds Check");

    QGridLayout *layout = new QGridLayout();
    layout->addWidget(button_file,0,0,1,2,Qt::AlignLeft);
    layout->addWidget(label_start,1,0,1,2,Qt::AlignLeft);
    layout->addWidget(spinbox_Xstart,2,0,1,1,Qt::AlignLeft);
    layout->addWidget(spinbox_Ystart,2,1,1,1,Qt::AlignLeft);
    layout->addWidget(label_end,3,0,1,2,Qt::AlignLeft);
    layout->addWidget(spinbox_Xend,4,0,1,1,Qt::AlignLeft);
    layout->addWidget(spinbox_Yend,4,1,1,1,Qt::AlignLeft);
    layout->addWidget(button_grabcut,5,0,1,2,Qt::AlignLeft);
    layout->addWidget(button_otsu,6,0,1,2,Qt::AlignLeft);
    layout->addWidget(button_overlay,7,0,1,2,Qt::AlignLeft);
    layout->addWidget(button_rot,8,0,2,1,Qt::AlignLeft);
    layout->addWidget(tes,0,3,10,1,Qt::AlignLeft);

    QWidget *display = new QWidget();
    display->setLayout(layout);
    display->show();

    QObject::connect(tes,SIGNAL(imageWidth(int)),spinbox_Xstart,SLOT(setValueMax(int)));
    QObject::connect(tes,SIGNAL(imageWidth(int)),spinbox_Xend,SLOT(setValueMax(int)));
    QObject::connect(tes,SIGNAL(imageHeight(int)),spinbox_Ystart,SLOT(setValueMax(int)));
    QObject::connect(tes,SIGNAL(imageHeight(int)),spinbox_Yend,SLOT(setValueMax(int)));
    QObject::connect(tes,SIGNAL(imageWidth(int)),spinbox_Xend,SLOT(setValue(int)));
    QObject::connect(tes,SIGNAL(imageHeight(int)),spinbox_Yend,SLOT(setValue(int)));
    QObject::connect(spinbox_Xstart,SIGNAL(valueChanged(int)),tes,SLOT(setGrabcut_Xbegin(int)));
    QObject::connect(spinbox_Ystart,SIGNAL(valueChanged(int)),tes,SLOT(setGrabcut_Ybegin(int)));
    QObject::connect(spinbox_Xend,SIGNAL(valueChanged(int)),tes,SLOT(setGrabcut_Xend(int)));
    QObject::connect(spinbox_Yend,SIGNAL(valueChanged(int)),tes,SLOT(setGrabcut_Yend(int)));
    QObject::connect(button_grabcut,SIGNAL(clicked()),tes,SLOT(applyGrabcut()));
    QObject::connect(button_otsu,SIGNAL(clicked()),tes,SLOT(applyGrayOtsu()));
    QObject::connect(button_file,SIGNAL(clicked()),tes,SLOT(openFilename()));
    QObject::connect(button_overlay,SIGNAL(clicked()),tes,SLOT(drawOverlay()));
    QObject::connect(button_rot,SIGNAL(clicked()),tes,SLOT(checkRuleofThird()));
    return a.exec();
}
