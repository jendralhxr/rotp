#ifndef ROTSPINBOX_H
#define ROTSPINBOX_H

#include <QSpinBox>

class ROTSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit ROTSpinBox(QWidget *parent = 0);

signals:

public slots:
    void setValueMax(int val);
    void resetValue();
};

#endif // ROTSPINBOX_H
