#include "orthogonalTransform.h"
#include <QMessageBox>

orthogonalTransform::orthogonalTransform(QWidget *parent , Qt::WindowFlags flags)
    :QWidget(parent, flags)
{
 
}

void fourierTransform::discreteFourierTrans(char* fileName)
{
    inputImage = cvLoadImage(fileName, 0);
    if(inputImage == NULL || inputImage->nChannels == 3)
    {
        QMessageBox::warning(this, tr("discreteFourierTrans 1"), tr("discreteFourierTrans 2"),  QMessageBox::Ok);
        return;
    }
    CvMat* mat = cvCreateMat(inputImage->height, inputImage->width, CV_32FC1);
    for(int j = 0; j < mat->rows; j++)
        for(int i = 0; i < mat->cols; i++)
        {
            cvmSet(mat, j, i, double(inputImage->imageData[j * mat->cols + i]));
        }
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_32F, 1);

    cvDFT(mat, outputImage, CV_DXT_FORWARD,  0);
    
    outputFileName = "../data/temp/fourierTransform.jpg";//QString
    cvSaveImage(outputFileName, outputImage);//char*

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);
}

orthogonalTransform::~orthogonalTransform()
{

}