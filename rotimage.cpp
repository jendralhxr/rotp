#include "rotimage.h"
#include <QFileDialog>
#define MAX_WIDTH 1280
#define MAX_HEIGHT 720
#define CENTROID_SAMPLE_COUNT 60
#define OVERLAY_COLOR 0,0,200 // dead blue
#define GRABCUT_COLOR 200,0,0 // dead red
#define TOLERANCE 0.05  // tolerance, proportional to image hypotenuse
#include <stdlib.h>
#include <math.h>

ROTimage::ROTimage(QWidget *parent) : QLabel(parent)
{
}

void ROTimage::openFilename(){
    image= cv::imread(QFileDialog::getOpenFileName().toStdString());
    if(!image.data || image.cols>MAX_WIDTH || image.rows>MAX_HEIGHT) {
        qDebug("File not found or invalid.");
    }
    else{
        emit imageWidth(image.cols);
        emit imageHeight(image.rows);
        intersect_x[0] = image.cols/3;   intersect_y[0] = image.rows/3;
        intersect_x[1] = image.cols/3;   intersect_y[1] = 2*image.rows/3;
        intersect_x[2] = 2*image.cols/3; intersect_y[2] = image.rows/3;
        intersect_x[3] = 2*image.cols/3; intersect_y[3] = 2*image.rows/3;
        renderImage();
    }
}

void ROTimage::renderImage(){
    switch (image.type()) {
    case CV_8UC1:
        cvtColor(image, tmp, CV_GRAY2RGB);
        break;
    case CV_8UC3:
        cvtColor(image, tmp, CV_BGR2RGB);
        break;
    }
    assert(tmp.isContinuous());
    disp = QImage(tmp.data, tmp.cols, tmp.rows, tmp.cols*3, QImage::Format_RGB888);
    setPixmap(QPixmap::fromImage(disp));
    update();
}

void ROTimage::applyGrabcut(){
    // GrabCut segmentation
    cv::grabCut(image,    // input image
                result,   // segmentation result
                grabcut_rect,// rectangle containing foreground
                bgModel,fgModel, // models
                1,        // number of iterations
                cv::GC_INIT_WITH_RECT); // use rectangle
    // Get the pixels marked as likely foreground
    cv::compare(result,cv::GC_PR_FGD,result,cv::CMP_EQ);
    // Generate output image
    cv::Mat foreground(image.size(),CV_8UC3,cv::Scalar(0,0,0));
    image.copyTo(foreground,result); // bg pixels not copied
    image = foreground;
    renderImage();
}

void ROTimage::applyGrayOtsu(){
    cv::Mat foreground(image.size(),CV_8UC3,cv::Scalar(0,0,0));
    image.copyTo(foreground,result); // bg pixels not copied
    cv::Mat img_gray;
    cv::Mat img_bw;
    cv::cvtColor(foreground,img_gray,CV_RGB2GRAY);
    cv::threshold(img_gray,img_bw,0,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
    image = img_bw;
    renderImage();
}

void ROTimage::drawOverlay(){
    //    assert(image.isContinuous());
    //    disp= QImage(image.data, image.cols, image.rows, image.cols*3, QImage::Format_RGB888);
    painter.begin(&disp);
    painter.setPen(qRgb(OVERLAY_COLOR));
    painter.drawLine(0, image.rows/3,   image.cols, image.rows/3);
    painter.drawLine(0, 2*image.rows/3, image.cols, 2*image.rows/3);
    painter.drawLine(image.cols/3,      0,  image.cols/3,   image.rows);
    painter.drawLine(2*image.cols/3,    0,  2*image.cols/3, image.rows);
    painter.end();
    setPixmap(QPixmap::fromImage(disp));
    update();
}

void ROTimage::drawGrabcut(){
    renderImage();
    painter.begin(&disp);
    painter.setPen(qRgb(GRABCUT_COLOR));
    painter.drawLine(grabcut_xbegin, grabcut_ybegin, grabcut_xbegin, grabcut_yend);
    painter.drawLine(grabcut_xend  , grabcut_ybegin, grabcut_xend  , grabcut_yend);
    painter.drawLine(grabcut_xbegin, grabcut_ybegin, grabcut_xend  , grabcut_ybegin);
    painter.drawLine(grabcut_xbegin, grabcut_yend  , grabcut_xend  , grabcut_yend);
    painter.end();
    setPixmap(QPixmap::fromImage(disp));
    update();
}

void ROTimage::setGrabcut_Xbegin(int pixel){
    grabcut_xbegin= pixel;
    grabcut_rect.x= grabcut_xbegin;
    drawGrabcut();
}

void ROTimage::setGrabcut_Ybegin(int pixel){
    grabcut_ybegin= pixel;
    grabcut_rect.y= grabcut_ybegin;
    drawGrabcut();
}

void ROTimage::setGrabcut_Xend(int pixel){
    grabcut_xend= pixel;
    grabcut_rect.width= grabcut_xend-grabcut_xbegin;
    drawGrabcut();
}

void ROTimage::setGrabcut_Yend(int pixel){
    grabcut_yend= pixel;
    grabcut_rect.height= grabcut_yend-grabcut_ybegin;
    drawGrabcut();
}

int ROTimage::checkRuleofThird(){
    // srand(some_random_number), please
    double x_acc, y_acc;
    int x_temp, y_temp;
    int count=0;
    while(count<CENTROID_SAMPLE_COUNT){
        x_temp = rand()%image.cols;
        y_temp = rand()%image.rows;
        qDebug("%d %d %d",count, x_temp, y_temp);
        cv::Scalar colour = image.at<uchar>(y_temp, x_temp);
        if (colour.val[0]==255){ // the pixel[temp] is white!!
            x_acc += x_temp;
            y_acc += y_temp;
            count++;
        }
    }
    centroid_x = x_acc/CENTROID_SAMPLE_COUNT;
    centroid_y = y_acc/CENTROID_SAMPLE_COUNT;
    qDebug("Centroid (x,y): %f %f",centroid_x,centroid_y);
    double hypotenuse = sqrt(pow(image.cols,2) + pow(image.rows,2));
    double distance;
    for (int i=0; i<4; i++){
        distance = sqrt(pow(centroid_x-intersect_x[i],2) \
                        + sqrt(pow(centroid_y-intersect_y[i],2)));
        if (distance < TOLERANCE*hypotenuse){
            qDebug("Rule of Third: Yes, distance %f px",distance);
            return(0);
        }
    }
    qDebug("Rule of Third: No");
    return(1);
}

int ROTimage::getHeight(){
    return(image.rows);
}

int ROTimage::getWidth(){
    return(image.cols);
}
