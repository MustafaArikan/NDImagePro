#include "edge_contour.h"

edgeContour::edgeContour(QWidget *parent , Qt::WindowFlags flags)
    :QWidget(parent, flags)
{

}

void edgeContour::sobelOperator(char* fileName)
{
    inputImage = cvLoadImage(fileName, 0);

    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_16S, 1);
    cvSobel(inputImage, outputImage, 1, 1, 3);
    outputFileName = ("../data/temp/sobelEdgeDetection.jpg");
    cvSaveImage(outputFileName, outputImage);

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);
}

edgeContour::~edgeContour()
{

}