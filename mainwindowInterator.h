#ifndef MAINWINDOWINTERATOR_H
#define MAINWINDOWINTERATOR_H

#include <QtGui>
#include "mainwindow.h"
#include "vtkCustomCommands.h"
#include <QLabel>
#include <QPixmap>
#include <QProgressDialog>
#include <QTimer>
#include <vtkMetaImageReader.h>
#include <vtkDICOMImageReader.h>
#include <vtkStructuredPointsReader.h>//read vtk
#include <vtkGenericDataObjectReader.h>
#include <vtkPolyDataReader.h>
#include <vtkImageReader.h>//read raw
#include <vtkImageFlip.h>
#include <cv.h>

//库中预定义类
class vtkImageReader2;
class vtkImageViewer2;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkImageReader2Factory;
class vtkRenderWindowInteractor;
class vtkMetaImageReader;

//自定义类
class dicomProcess;
class geometryTransform;
class pointOperation;
class itkFilter;
class morphology;
class fourierTransform;
class edgeContour;
class segmentation;
class registration;

class mainwindowInterator : public mainWindow
{
    Q_OBJECT

public:
    mainwindowInterator(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~mainwindowInterator();

    
private:
    QString qstrInputFileName;
    QByteArray ba;
    char* inputFileName;
    char* outputFileName;
    int pixelType;
    int* imgSize;
    int imgDepth;
    IplImage* img;
    int dimension;
    QProgressDialog* progDlg;
    QTimer* timer;
    int currentValue;

    dicomProcess* pDicomPro; 
    geometryTransform* pGeomtry;
    morphology* pMorph;
    itkFilter* pFilter;
    pointOperation* pPointOpe;
    fourierTransform* pFourierTrans;
    edgeContour* pEdgeContour;
    segmentation* pSegmentation;
    registration* pRegistration;

    QWidget* img2DPropertywidget ;
    QWidget* img3DPropertywidget ;
    QLabel* img2DHeightName, *img3DHeightName;
    QLabel* img2DHeightNum, *img3DHeightNum;
    QLabel* img2DWidthName, *img3DWidthName;
    QLabel* img2DWidthNum, *img3DWidthNum;
    QLabel* img3DLengthName;
    QLabel* img3DLengthNum;
    QLabel* img2DDepthName, *img3DDepthName;
    QLabel* img2DDepthNum, *img3DDepthNum;
    QLabel* img2DChannelsName;
    QLabel* img2DChannelsNum;

    vtkImageReader2* originalReader;
    vtkImageViewer2* originalVtkViewer;
    vtkRenderer* originalRenderder;
    vtkRenderWindowInteractor* originalRenWinInteractor; 
    vtkImageReader2Factory* originalReaderFactory;

    vtkImageReader2* resultReader;
    vtkImageViewer2* resultVtkViewer;
    vtkRenderer* resultRenderder;
    vtkRenderWindowInteractor* resultRenWinInteractor; 
    vtkImageReader2Factory* resultReaderFactory;

    vtkNextSliceCallbk* pCall;
    vtkInteractorStyleImage* style;

    vtkMetaImageReader* mhdReader; 
    vtkStructuredPointsReader* vtkReader;//vtkStructuredPointsReader /vtkGenericDataObjectReader / vtkPolyDataReader
    vtkImageReader* rawReader;
    vtkImageFlip* rotate;
    vtkDICOMImageReader*dicomReader;

    
private:
    void createControlPanel();
    bool isSurpported2DImgType();
    bool isSurpported3DImgType();
    bool isMHDImgType();
    void initVisualizationPipeline();

signals:

    private slots:
        void updateResultViewer(char* fileName);
        void updateOriginalViewer(char* fileName);
        void clearResultViewer();
        void resetControlPanel();
        void clear();
        void retinalLayerVisualization();
        void abnormalRegionVisualization();
        void updateProgressDialog();

        void on_action_Open_triggered();
        void on_action_Open_3DImage_triggered();
        void on_action_OpenDICOM_triggered();
        void on_action_Save_triggered();

        void on_action_Color2Gray_triggered();
        void on_action_ColorHist_triggered();
        void on_action_GrayHist_triggered();
        void on_action_Interpolation_triggered();
        void on_action_SingleChannel_triggered();
        void on_action_HistgramEqualization_triggered();

        void on_action_Geometry_triggered();
        void on_action_Morphology_triggered();
        void on_action_Fourier_triggered();

        void on_action_MeanFilter_triggered();
        void on_action_GaussianFilter_triggered();
        void on_action_BilateralFilter_triggered();
        void on_action_LaplacianFilter_triggered();
        void on_action_CurvatureAnisotropicDiffusionFilter_triggered();

        void on_action_Edge_triggered();

        void on_action_VolumeRender_triggered();
        void on_action_SurfaceRender_triggered();

        void on_action_RegionGrowSeg_triggered();
        void on_action_WatershedSeg_triggered();
        void on_action_PEDSeg_triggered();

        void on_action_Registration_2D_triggered();
};
#endif // MAINWINDOWINTERATOR_H
