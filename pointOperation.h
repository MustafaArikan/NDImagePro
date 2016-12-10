#ifndef POINTOPERATION_H
#define POINTOPERATION_H

#include <QtGui>
#include <QPixmap>

#include "ui_pointOperation.h"
#include "highgui.h"
#include "cv.h"

class pointOperation : public QWidget
{
    Q_OBJECT

public:
    pointOperation(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~pointOperation();

    
private:
    char* outputFileName;
    //OpenCV
    IplImage *inputImage, *outputImage;

    
public:
    void color2Gray(char* fileName);
    void interpolation(char* fileName);
    void colorHistgram(char* fileName);
    void grayHistgram(char* fileName);
    void histgramEqualization(char* fileName);
    void getSingleChannelImg(char* fileName);

signals:
    void returnOutputFileName(char* fileName);
};

#endif // POINTOPERATION_H
