#include "pointOperation.h"
#include <QMessageBox>

pointOperation::pointOperation(QWidget *parent, Qt::WindowFlags flags)
    :QWidget(parent, flags)
{
     inputImage = new IplImage();
     outputImage = new IplImage();
}

void pointOperation::color2Gray(char* fileName)
{
    inputImage = cvLoadImage(fileName);
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);

    cvCvtColor(inputImage, outputImage, CV_RGB2GRAY);
    //cvConvertImage(inputImage, outputImage, 0);
    outputFileName = ("../data/temp/color2gray.jpg");
    cvSaveImage(outputFileName, outputImage);

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);
}

void pointOperation::histgramEqualization(char* fileName)
{
    //QString to char*
    inputImage = cvLoadImage(fileName, CV_LOAD_IMAGE_UNCHANGED);
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);
    if(inputImage->nChannels == 3)
    {
        IplImage* temp = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);
        cvConvertImage(inputImage, temp);
        cvEqualizeHist(temp, outputImage);
    }
    else
    {
         cvEqualizeHist(inputImage, outputImage);
    }
    outputFileName = ("../data/temp/histgramEqualization.jpg");
    cvSaveImage(outputFileName, outputImage);

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);
}

void pointOperation::interpolation(char* fileName)
{
    inputImage = cvLoadImage(fileName, 0);
    outputImage = cvCreateImage(cvSize(inputImage->width * 2, inputImage->height * 2), IPL_DEPTH_8U, 1);
    cvResize(inputImage, outputImage, CV_INTER_CUBIC);//CV_INTER_CUBIC, CV_INTER_AREA, CV_INTER_LANCZOS4
    outputFileName = ("../data/temp/interpolation.jpg");
    cvSaveImage(outputFileName, outputImage);

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

   emit returnOutputFileName(outputFileName);
}

void pointOperation::grayHistgram(char* fileName)
{
    inputImage = cvLoadImage(fileName, CV_LOAD_IMAGE_UNCHANGED);

    if(inputImage->nChannels == 3)
    {
        QMessageBox::warning(this, tr("grayHistgram"), tr("grayHistgram 2"), QMessageBox::Ok);
        return;
    }

    int histSize = 256;
    int histHeight = 256; 
    float range[] = {0,255};
    float* ranges[] ={range};  

    CvHistogram* grayHist = cvCreateHist(1, &histSize, CV_HIST_ARRAY, ranges, 1); 

    cvCalcHist(&inputImage, grayHist, 0, 0);  

    cvNormalizeHist(grayHist, 1.0);  

    float maxValue = 0;  
    cvGetMinMaxHistValue(grayHist, 0, &maxValue, 0, 0);  

    IplImage* histImage = cvCreateImage(cvSize(histSize, histHeight), 8, 3);  
    cvZero(histImage);

    for(int i = 0; i < histSize; i++)  
    {  
        //float binNum = cvQueryHistValue_1D(grayHist, i);
		int binHeight = 0; // cvRound(binNum / maxValue * histHeight);
        cvRectangle(histImage,  
            cvPoint(i, histHeight - 1),  
            cvPoint(i + 1, histHeight - binHeight),  
            CV_RGB(255, 0, 0));    
    }  

    outputFileName = ("../data/temp/grayHistgram.jpg");
    cvSaveImage(outputFileName, histImage);

    cvReleaseImage(&inputImage);
    cvReleaseImage(&histImage);

    emit returnOutputFileName(outputFileName);
}

void pointOperation::colorHistgram(char* fileName)
{

}

void pointOperation::getSingleChannelImg(char* fileName)
{
    inputImage = cvLoadImage(fileName);
    outputImage = cvCreateImage(cvGetSize(inputImage), IPL_DEPTH_8U, 1);
    cvConvertImage(inputImage, outputImage, 0);
    outputFileName = ("../data/temp/singleChannel.jpg");
    cvSaveImage(outputFileName, outputImage);
    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);
}

pointOperation::~pointOperation()
{

}