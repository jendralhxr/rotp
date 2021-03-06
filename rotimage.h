#ifndef ROTIMAGE_H
#define ROTIMAGE_H

#include <QApplication>
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QMessageBox>
#include <QString>
#include <opencv2/opencv.hpp>

using namespace cv;

class ROTimage : public QLabel
{
    Q_OBJECT
public:
    explicit ROTimage(QWidget *parent = 0);
    int getHeight();
    int getWidth();

signals:
    void imageWidth(int width);
    void imageHeight(int height);
    void newlyOpen();

public slots:
    int openFilename();
    void drawOverlay();
    void drawGrabcutBoundary();
    void applyGrabcut();
    void applyGrayOtsu();
    void renderImage();
    void setGrabcut_Xbegin(int pixel);
    void setGrabcut_Ybegin(int pixel);
    void setGrabcut_Xend(int pixel);
    void setGrabcut_Yend(int pixel);
    int checkRuleofThirds();

private:
    //for tolerance and scoring
	double tolerance[8];
	int score[9];

    // for grabcut
    Mat result; // segmentation result (4 possible values)
    Mat bgModel,fgModel; // the models (internally used)
    int grabcut_xbegin, grabcut_ybegin, grabcut_xend, grabcut_yend;
    Rect grabcut_rect;

    // centroid variables
    double intersect_x[4], intersect_y[4];
    double centroid_x, centroid_y;
    Mat image, tmp;// image_centro;
    QImage disp;
    QPainter painter;
    double x_accumulative, y_accumulative;
    double x_temp, y_temp;
    double image_hypotenuse, distance_hypotenuse;
    int count;//, population;
    QString string;
    QMessageBox messagebox;

    //for sampling
    double proportion,min_sample,trust,deviation,q,tpq,grab_populate,whole_populate;
};

#endif // ROTIMAGE_H
