#ifndef EDGECONTOUR_H
#define EDGECONTOUR_H

#include <QtGui>
#include <QPixmap>
#include "ui_edge_contour.h"
#include "highgui.h"
#include "cv.h"

class edgeContour : public QWidget
{
    Q_OBJECT

public:
    edgeContour(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~edgeContour();

private:
    char* outputFileName;
    char* inputFileName;
    QByteArray ba;   
    //OpenCV
    IplImage *inputImage, *outputImage;
public:
    void sobelOperator(char* fileName);
signals:
    void returnOutputFileName(char* fileName);
};

#endif // EDGECONTOUR_H
