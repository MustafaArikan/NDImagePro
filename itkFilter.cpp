#include "itkFilter.h"
#include <QProgressBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <Windows.h>
#include <string>
using namespace std;

itkFilter::itkFilter(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{

}

void itkFilter::meanFilter(char* fileName, int dimension, int pixelType)
{

    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "meanFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_meanFilter.jpg";
    else
        tempPath = tempPath + "_meanFilter.mhd";
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);
}

void itkFilter::gaussianFilter(char* fileName, int dimension, int pixelType) 
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "gaussianFilter_dll");//第二个参数为函数名
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_gaussianFilter.jpg";
    else
        tempPath = tempPath + "_gaussianFilter.mhd"; 
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);
}


void itkFilter::bilateralFilter(char* fileName, int dimension, int pixelType) 
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "bilateralFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_bilateralFilter.jpg";
    else
    {
        QMessageBox::warning(this, tr("Warning Text 1"),
            tr("Warning Text 2"), QMessageBox::Ok);
        return;
    }
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);
}

void itkFilter::laplacianFilter(char* fileName, int dimension, int pixelType)
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "laplacianFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_laplacianFilter.jpg";
    else
        tempPath = tempPath + "_laplacianFilter.mhd";
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);
}

void itkFilter::curvatureAnisotropicDiffusionFilter(char* fileName, int dimension, int pixelType)
{
    HINSTANCE hfilter = LoadLibraryA("filter_dll.dll");
    typedef void(*pFilter)(char* inputFilePath, int dimension, int pixelType); 
    pFilter filter = (pFilter)GetProcAddress(hfilter, "curvatureAnisotropicDiffusionFilter_dll");
    filter(fileName, dimension, pixelType);
    FreeLibrary(hfilter);

    string tempPath;
    tempPath = fileName;//char* to string
    tempPath = tempPath.substr(0, tempPath.length() - 4);
    if(dimension == 2)
        tempPath = tempPath + "_curvatureAnisotropicDiffusionFilter.jpg";
    else 
        tempPath = tempPath + "_curvatureAnisotropicDiffusionFilter.mhd";
    strcpy(outputFileName, tempPath.c_str());//string to char*

    emit returnOutputFileName(outputFileName);
}

itkFilter::~itkFilter()
{

}

