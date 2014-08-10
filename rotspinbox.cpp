#include "rotspinbox.h"

ROTSpinBox::ROTSpinBox(QWidget *parent) :
    QSpinBox(parent)
{
}

void ROTSpinBox::setValueMax(int val){
    setMaximum(val);
}

void ROTSpinBox::resetValue(){
    setValue(0);
}
