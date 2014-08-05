#include "rotimage.h"
#include <stdlib.h>
#include <math.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QPen>
#include <QPainter>
#include <QMessageBox>
#include <QString>

#define MAX_WIDTH 640
#define MAX_HEIGHT 640
#define CENTROID_SAMPLE_COUNT 100
#define OVERLAY_COLOR 0,0,200 // dead blue
#define GRABCUT_COLOR 200,0,0 // dead red
#define LINE_SCORE_GRADE -10

using namespace cv;

ROTimage::ROTimage(QWidget *parent) : QLabel(parent)
{

}

int ROTimage::openFilename(){
    image.release();
    tmp.release();
 //   image = imread(QFileDialog::getOpenFileName(this,tr("Open Image"), "./images",
  //                                              tr("Image Files (*.jpeg; *.jpg; *.bmp; *.png)")).toStdString());
    image = imread(QFileDialog::getOpenFileName().toStdString());

    if(!image.data || image.cols>MAX_WIDTH || image.rows>MAX_HEIGHT) {
        messagebox.setText(string.sprintf("Image not found or image dimension is \
                                          greater than 640x640 pixels"));
                                          messagebox.exec();
                           return(0);
    }
    else{
        // centroid variables
        intersect_x[4]=0, intersect_y[4]=0;
        centroid_x=0, centroid_y=0;
        x_accumulative=0, y_accumulative=0;
        x_temp=0, y_temp=0;
        count=0;
        image_centro.release();

        emit imageWidth(image.cols);
        emit imageHeight(image.rows);
        intersect_x[0] = image.cols/3;   intersect_y[0] = image.rows/3;
        intersect_x[1] = image.cols/3;   intersect_y[1] = 2*image.rows/3;
        intersect_x[2] = 2*image.cols/3; intersect_y[2] = image.rows/3;
        intersect_x[3] = 2*image.cols/3; intersect_y[3] = 2*image.rows/3;
        renderImage();
        return(0);
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

    try
    {
        // GrabCut segmentation
        grabCut(image,    // input image
                result,   // segmentation result
                grabcut_rect,// rectangle containing foreground
                bgModel,fgModel, // models
                1, // number of iterations
                GC_INIT_WITH_RECT); // use rectangle
        // Get the pixels marked as likely foreground
        compare(result, GC_PR_FGD,result,CMP_EQ);
        // Generate output image
        Mat foreground(image.size(),CV_8UC3,Scalar(0,0,0));
        image.copyTo(foreground,result); // bg pixels not copied
        image = foreground;
        renderImage();
    }

    catch(...)
    {
        messagebox.setText(string.sprintf("Open an image first before apply grabcut segmentation \
                                          or draw grabcut coordinates first"));
                                          messagebox.exec();
                           return;
    }
}

void ROTimage::applyGrayOtsu(){
    try
    {
        Mat foreground(image.size(),CV_8UC3,Scalar(0,0,0));
        image.copyTo(foreground,result); // bg pixels not copied
        Mat img_gray;
        Mat img_bw;
        cvtColor(foreground,img_gray,CV_RGB2GRAY);
        threshold(img_gray,img_bw,0,255,CV_THRESH_BINARY|CV_THRESH_OTSU);
        image = img_bw;
        renderImage();
    }

    catch(...)
    {
        messagebox.setText(string.sprintf("Open an image first or apply the grabcut \
                                          segmentation first"));
                                          messagebox.exec();
                           return;
    }
}

void ROTimage::drawOverlay(){
    try
    {
        QPen pen;
        painter.begin(&disp);
        pen.setWidth(2);
        pen.setColor(qRgb(OVERLAY_COLOR));
        painter.setPen(pen);
        painter.drawLine(0, image.rows/3,   image.cols, image.rows/3); //draw upper horizontal third line
        painter.drawLine(0, 2*image.rows/3, image.cols, 2*image.rows/3); //draw lower horizontal third line
        painter.drawLine(image.cols/3,      0,  image.cols/3,   image.rows); //draw left vertical third line
        painter.drawLine(2*image.cols/3,    0,  2*image.cols/3, image.rows); //draw right vertical third line
        painter.end();
        setPixmap(QPixmap::fromImage(disp));
        update();
    }

    catch(...)
    {
        messagebox.setText(string.sprintf("Open an image first before use grabcut \
                                          segmentation"));
                                          messagebox.exec();
                           return;
    }
}

void ROTimage::drawGrabcut(){

    renderImage();
    QPen pen;
    painter.begin(&disp);
    pen.setColor(qRgb(GRABCUT_COLOR));
    pen.setWidth(4);
    painter.setPen(pen);

    painter.drawLine(grabcut_xbegin, grabcut_ybegin, grabcut_xbegin, grabcut_yend); //draw left vertical line
    painter.drawLine(grabcut_xend  , grabcut_ybegin, grabcut_xend  , grabcut_yend); //draw right vertical line
    painter.drawLine(grabcut_xbegin, grabcut_ybegin, grabcut_xend  , grabcut_ybegin); //draw upper horizontal line
    painter.drawLine(grabcut_xbegin, grabcut_yend  , grabcut_xend  , grabcut_yend); //drraw lower horizontal line
    painter.end();
    setPixmap(QPixmap::fromImage(disp));
    update();
}

void ROTimage::setGrabcut_Xbegin(int pixel){
    grabcut_xbegin = pixel;
    grabcut_rect.x = grabcut_xbegin;
    drawGrabcut();
}

void ROTimage::setGrabcut_Ybegin(int pixel){
    grabcut_ybegin = pixel;
    grabcut_rect.y = grabcut_ybegin;
    drawGrabcut();
}

void ROTimage::setGrabcut_Xend(int pixel){
    grabcut_xend = pixel;
    grabcut_rect.width = grabcut_xend-grabcut_xbegin;
    drawGrabcut();
}

void ROTimage::setGrabcut_Yend(int pixel){
    grabcut_yend = pixel;
    grabcut_rect.height = grabcut_yend-grabcut_ybegin;
    drawGrabcut();
}

int ROTimage::checkRuleofThird(){

    try
    {
        while(count<CENTROID_SAMPLE_COUNT){
            population++;
            x_temp = grabcut_xbegin + rand()%(grabcut_xend-grabcut_xbegin);
            y_temp = grabcut_ybegin + rand()%(grabcut_yend-grabcut_ybegin);
            Scalar color = image.at<uchar>(y_temp, x_temp);
            if (color.val[0]==255){ // the pixel[temp] is white!!
                x_accumulative = x_accumulative + double(x_temp);
                y_accumulative = y_accumulative + double(y_temp);
                count++;
            }
        }
        centroid_x = x_accumulative/ (double) CENTROID_SAMPLE_COUNT;
        centroid_y = y_accumulative/ (double) CENTROID_SAMPLE_COUNT;

        painter.begin(&disp);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
        painter.drawEllipse(centroid_x,centroid_y,10,10);

        painter.end();
        setPixmap(QPixmap::fromImage(disp));
        update();

        qDebug("Centroid (x,y): %f %f",centroid_x,centroid_y);
        double hypotenuse = sqrt(pow(image.cols,2) + pow(image.rows,2));
        double distance;

        // rule of thirds check, intersection rule
        for (int i=0; i<4; i++){
            for (int j=0; j<8; j++){
                distance = sqrt(pow(centroid_x-intersect_x[i],2)+pow(centroid_y-intersect_y[i],2));
                if (distance < tolerance[j]*hypotenuse){
                    messagebox.setText(string.sprintf("Rule of Thirds: Yes. \
                                                      \nCentroid is at coordinates (%.2f, %.2f). Sample count: %d/%d\
                                                      \nDistance from nearest intersection is %.2f Pixel(s). Score %d.",
                                                      centroid_x, centroid_y, count,population, distance, score[j]));
                    messagebox.exec();
                    return(0);
                }
            }
        }

        // rule of thirds check, line rule
        for (int i=0; i<4; i++){
            switch (i) {
            case 1:
                distance = abs(centroid_x-image.cols/3);
                break;
            case 2:
                distance = abs(centroid_x-2*image.cols/3);
                break;
            case 3:
                distance = abs(centroid_y-image.rows/3);
                break;
            case 4:
                distance = abs(centroid_y-2*image.rows/3);
                break;
            }
            for (int j=0; j<8; j++){
                if (distance < tolerance[j]*hypotenuse){
                    messagebox.setText(string.sprintf("Rule of Thirds: Yes (Line Rule). \
                                                      \nCentroid is at coordinates (%.2f, %.2f). Sample count: %d/%d\
                                                      \nDistance from nearest line is %.2f Pixel(s). Score %d.",
                                                      centroid_x, centroid_y, count, population, distance, score[j]+LINE_SCORE_GRADE));
                    messagebox.exec();
                    return(0);
                }
            }
        }

        messagebox.setText(string.sprintf("Rule of Thirds: No.\nCentroid is at coordinates (%.2f, %.2f). \
                                          \nDistance from nearest intersection is %.2f Pixel(s).\nScore is %d."\
                                          ,centroid_x,centroid_y,distance,score[8]));
        messagebox.exec();
        return(0);

    }

    catch(...){
        messagebox.setText(string.sprintf("Open an image first before use grabcut segmentation"));
        messagebox.exec();
        return(0);
    }

}




int ROTimage::getHeight(){
    return(image.rows);
}

int ROTimage::getWidth(){
    return(image.cols);
}

//void ROTimage::exit()
//{
//    //exit(EXIT_FAILURE);
//}
