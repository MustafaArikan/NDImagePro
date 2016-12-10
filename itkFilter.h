#ifndef ITKFILTER_H
#define ITKFILTER_H

#include <QtGui>
#include <QPixmap>
#include "ui_itkFilter.h"
#include "highgui.h"
#include "cv.h"

class itkFilter : public QWidget
{
    Q_OBJECT

public:
    itkFilter(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~itkFilter();

private:
    char outputFileName[100];

signals:
   void returnOutputFileName(char* fileName);

public:
    void meanFilter(char* fileName, int dimension, int pixelType);
    void gaussianFilter(char* fileName, int dimension, int pixelType);
    void bilateralFilter(char* fileName, int dimension, int pixelType);
    void laplacianFilter(char* fileName, int dimension, int pixelType);
    void curvatureAnisotropicDiffusionFilter(char* fileName, int dimension, int pixelType);
};

#endif // ITKFILTER_H
