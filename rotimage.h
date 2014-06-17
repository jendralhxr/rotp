#ifndef ROTIMAGE_H
#define ROTIMAGE_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <opencv2/opencv.hpp>

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

public slots:
    int openFilename();
    void drawOverlay();
    void drawGrabcut();
    void applyGrabcut();
    void applyGrayOtsu();
    void renderImage();
    void setGrabcut_Xbegin(int pixel);
    void setGrabcut_Ybegin(int pixel);
    void setGrabcut_Xend(int pixel);
    void setGrabcut_Yend(int pixel);
    int checkRuleofThird();

private:
    // for grabcut
    cv::Mat result; // segmentation result (4 possible values)
    cv::Mat bgModel,fgModel; // the models (internally used)
    int grabcut_xbegin, grabcut_ybegin, grabcut_xend, grabcut_yend;
    cv::Rect grabcut_rect;

    // centroid variables
    double intersect_x[4], intersect_y[4];
    double centroid_x, centroid_y;
    // cv::Mat -> QImage -> QLabel render
    cv::Mat image, tmp;
    QImage disp;
    QPainter painter;
    double x_acc, y_acc;
    int x_temp, y_temp;
    int count;

};

#endif // ROTIMAGE_H
