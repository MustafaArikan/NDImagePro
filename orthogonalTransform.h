#ifndef ORTHOGONALTRANSFORM_H
#define ORTHOGONALTRANSFORM_H

#include <QtGui>
#include "ui_orthogonalTransform.h"
#include "highgui.h"
#include "cv.h"
#include <QPixmap>

class orthogonalTransform : public QWidget
{
    Q_OBJECT

public:
    orthogonalTransform(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~orthogonalTransform();

    
public:
    char* outputFileName;
    //OpenCV
    IplImage *inputImage, *outputImage;

    
private:

signals:
       void returnOutputFileName(char* fileName);

    private slots:
};


class fourierTransform : public orthogonalTransform
{
    Q_OBJECT

        
public:
    void discreteFourierTrans(char* fileName);
};
#endif // ORTHOGONALTRANSFORM_H
