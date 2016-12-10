#include "mainwindowInterator.h"
#include "geometryTransform.h"
#include "orthogonalTransform.h"
#include "morphology.h"
#include "pointOperation.h"
#include "dicomProcess.h"
#include "itkFilter.h"
#include "edge_contour.h"
#include "segmentation.h"
#include "registration.h"

#include <itkImage.h>//ITK
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkMeanImageFilter.h>
#include <vtkMetaImageReader.h>//VTK
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkImageViewer2.h>//2D, 3D
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>
#include <vtkImageFlip.h>
#include <QVTKWidget.h>
#include <vtkImageActor.h>
#include <vtkImageReader2Factory.h>//PNG, BMP, TIFF, JPG
#include <vtkImageReader.h>
#include <vtkVolume16Reader.h>
#include <vtkImageData.h>
#include <vtkImageCast.h>
#include <vtkContourFilter.h>//surface & volume rendering
#include <vtkDecimatePro.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkStripper.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkVolume.h>
#include <vtkProperty.h>
#include <vtkOutlineFilter.h>
#include <vtkMarchingCubes.h>

#include "vtkCamera.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkProperty.h"
#include "vtkPolyDataNormals.h"
#include "vtkImageShiftScale.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkInteractorStyleRubberBand3D.h"
#include <QGridLayout>//QT
#include <QFileDialog>
#include <QTransform>
#include <QMessageBox>
#include <QString>


#include <string>//C++
#include <iostream>
using namespace std;

mainwindowInterator::mainwindowInterator(QWidget *parent, Qt::WindowFlags flags)
    :mainWindow(parent, flags)
{
    pGeomtry = NULL;
    pMorph = NULL;
    pFilter = NULL;
    pPointOpe = NULL;
    pFourierTrans = NULL;
    pEdgeContour = NULL;
    pDicomPro = NULL;
    pSegmentation = NULL;
    pRegistration = NULL;

    pCall = NULL;

    originalReader = vtkImageReader2::New();
    originalReaderFactory = vtkImageReader2Factory::New();
    originalVtkViewer = vtkImageViewer2::New();
    originalRenderder = vtkRenderer::New();
    originalRenWinInteractor = vtkRenderWindowInteractor::New();
    resultReader = vtkImageReader2::New();
    resultReaderFactory = vtkImageReader2Factory::New();
    resultVtkViewer = vtkImageViewer2::New();
    resultRenderder = vtkRenderer::New();
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
    ui->resultQVTKViewer->setEnabled(false);

    createControlPanel();
}

void mainwindowInterator::initVisualizationPipeline()
{
    if(originalReader != NULL)//1
    {
        originalReader->Delete();
        originalReader = vtkImageReader2::New();
    }
    if(originalReaderFactory != NULL)//2
    {
        originalReaderFactory->Delete();
        originalReaderFactory = vtkImageReader2Factory::New();
    }
    if(originalVtkViewer != NULL)//3
    {
        //if(pCall != NULL)
        //originalVtkViewer->RemoveObserver(pCall);
        //originalVtkViewer->Delete();

        originalVtkViewer = vtkImageViewer2::New();
    }
    if(originalRenderder != NULL)//4
    {
        originalRenderder->Delete();
        originalRenderder = vtkRenderer::New();
    }
    if(originalRenWinInteractor != NULL)//5
    {
        originalRenWinInteractor->Delete();
        originalRenWinInteractor = vtkRenderWindowInteractor::New();
    }
    if(resultReader != NULL)//6
    {
        resultReader->Delete();
        resultReader = vtkImageReader2::New();
    }
    if(resultReaderFactory != NULL)//7
    {
        resultReaderFactory->Delete();
        resultReaderFactory = vtkImageReader2Factory::New();
    }
    if(resultVtkViewer != NULL)//8
    {
        //if(pCall != NULL)
        //resultVtkViewer->RemoveAllObservers();
        //resultVtkViewer->Delete();
        resultVtkViewer = vtkImageViewer2::New();
    }
    if(resultRenderder != NULL)//9
    {
        resultRenderder->Delete();
        resultRenderder = vtkRenderer::New();
    }
    if(resultRenWinInteractor != NULL)//10
    {
        resultRenWinInteractor->Delete();
        resultRenWinInteractor = vtkRenderWindowInteractor::New();
    }
    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);//init
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
    //originalVtkViewer->SetRenderer(originalRenderder);
    //resultVtkViewer->SetRenderer(resultRenderder);
}

void mainwindowInterator::updateResultViewer(char* fileName)
{
    outputFileName = fileName;

    string fileExtension;
    fileExtension = fileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());

    if(!ui->resultQVTKViewer->isEnabled())
        ui->resultQVTKViewer->setEnabled(true);

    if(fileExtension == "mhd" || fileExtension == "MHD")//3D
    {
        mhdReader = vtkMetaImageReader::New();
        mhdReader->SetFileName(fileName);
        mhdReader->Update();

        pixelType =  mhdReader->GetDataScalarType();
        rotate = vtkImageFlip::New();
        rotate->SetInputConnection(mhdReader->GetOutputPort());
        rotate->SetFilteredAxis(1);
        resultVtkViewer->SetInputConnection(rotate->GetOutputPort());

        ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
        resultVtkViewer->SetRenderer(resultRenderder);
        resultVtkViewer->SetRenderWindow(ui->resultQVTKViewer->GetRenderWindow());
        pCall = vtkNextSliceCallbk::New();
        style = vtkInteractorStyleImage::New();
        style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);
        style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
        resultRenWinInteractor->SetInteractorStyle(style);
        resultRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);
        resultRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
        pCall->SetInteractorStyle(style);
        pCall->SetViewer(originalVtkViewer, resultVtkViewer);
        pCall->SetInteractor(resultRenWinInteractor);
        ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);
        resultVtkViewer->SetupInteractor(ui->resultQVTKViewer->GetRenderWindow()->GetInteractor());
        resultVtkViewer->SetSliceOrientationToXY();
        if(pixelType == VTK_UNSIGNED_SHORT || pixelType == VTK_SHORT)
        {
            imgDepth = 16;
            resultVtkViewer->SetColorLevel(32768);
            resultVtkViewer->SetColorWindow(65536);
        }
        else//VTK_UNSIGNED_CHAR 
        {
            imgDepth = 8;
            resultVtkViewer->SetColorLevel(128); 
            resultVtkViewer->SetColorWindow(256); 
        }

        resultVtkViewer->Render();
        resultRenWinInteractor->Initialize();
        resultRenWinInteractor->Start();
        //ui->resultQVTKViewer->GetRenderWindow()->Render();
    }
    else//2D
    {
        resultReader = resultReaderFactory->CreateImageReader2(fileName);
        resultReader->SetFileName(fileName);
        resultReader->Update();
        resultVtkViewer->SetInputConnection(resultReader->GetOutputPort());
        resultVtkViewer->UpdateDisplayExtent();  
        resultVtkViewer->SetRenderWindow(ui->resultQVTKViewer->GetRenderWindow());  
        resultVtkViewer->SetRenderer(resultRenderder);  
		resultVtkViewer->SetupInteractor(ui->resultQVTKViewer->GetRenderWindow()->GetInteractor());

        if(img->nChannels != 3)
        {
            resultVtkViewer->SetColorLevel(128);
            resultVtkViewer->SetColorWindow(256);
        }
        resultVtkViewer->SetSliceOrientationToXY();
        resultVtkViewer->GetSlice();
        resultVtkViewer->GetImageActor()->InterpolateOff();  
        resultRenderder->ResetCamera();  
        resultRenderder->DrawOn();  
        ui->resultQVTKViewer->GetRenderWindow()->Render();
    }
}

void mainwindowInterator::updateOriginalViewer(char* fileName)
{

    string fileExtension;
    fileExtension = fileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());

    if(fileExtension == "mhd" || fileExtension == "MHD")
    {
        mhdReader = vtkMetaImageReader::New();
        mhdReader->SetFileName(fileName);
        mhdReader->Update();
        pixelType =  mhdReader->GetDataScalarType();
        rotate = vtkImageFlip::New();
        rotate->SetInputConnection(mhdReader->GetOutputPort());
        rotate->SetFilteredAxis(1);
        originalVtkViewer->SetInputConnection(rotate->GetOutputPort());
    }

    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);
    originalVtkViewer->SetRenderer(originalRenderder);
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());

    pCall = vtkNextSliceCallbk::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); 
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);

    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);

    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer, resultVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);

    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());
    originalVtkViewer->SetSliceOrientationToXY();

    ui->originalQVTKViewer->GetRenderWindow()->Render();
}

void mainwindowInterator::updateProgressDialog()
{
    currentValue++;  
    if( currentValue == 100 )  
        currentValue = 0;  
   progDlg ->setValue(currentValue);
   QCoreApplication::processEvents();
   if(progDlg->wasCanceled()) 
   {
        //timer->stop();
        //progDlg->hide();
        //delete progDlg;
   }
}

 
void mainwindowInterator::createControlPanel()
{
 
    img2DPropertywidget = new QWidget();
    img2DHeightName = new QLabel();
    img2DHeightNum = new QLabel();
    img2DWidthName = new QLabel();
    img2DWidthNum = new QLabel();
    img2DDepthName = new QLabel();
    img2DDepthNum = new QLabel();
    img2DChannelsName = new QLabel();
    img2DChannelsNum = new QLabel();

    img2DHeightName->setText(tr("Height(pix)"));
    img2DWidthName->setText(tr("Width(pix)"));
    img2DDepthName->setText(tr("Depth"));
    img2DChannelsName->setText(tr("Channel Name"));

    QGridLayout* img2DPropertyLayout = new QGridLayout();
    img2DPropertyLayout->addWidget(img2DWidthName, 0, 0, 0); 
    img2DPropertyLayout->addWidget(img2DWidthNum, 0, 1, 0); 
    img2DPropertyLayout->addWidget(img2DHeightName, 1, 0, 0); 
    img2DPropertyLayout->addWidget(img2DHeightNum, 1, 1, 0); 
    img2DPropertyLayout->addWidget(img2DDepthName, 2, 0,  0); 
    img2DPropertyLayout->addWidget(img2DDepthNum, 2, 1,  0); 
    img2DPropertyLayout->addWidget(img2DChannelsName, 3, 0,  0); 
    img2DPropertyLayout->addWidget(img2DChannelsNum, 3, 1,  0); 

    img2DPropertywidget->setLayout(img2DPropertyLayout);
    img2DPropertywidget->setFixedSize(260, 110); 
    img2DPropertywidget->setVisible(false);
 
    img3DPropertywidget = new QWidget();
    img3DLengthName = new QLabel();
    img3DLengthNum = new QLabel();
    img3DWidthName = new QLabel();
    img3DWidthNum = new QLabel();
    img3DHeightName = new QLabel();
    img3DHeightNum = new QLabel();
    img3DDepthName = new QLabel();
    img3DDepthNum = new QLabel();

    img3DLengthName->setText(tr("图像长度(pix)"));
    img3DWidthName->setText(tr("图像宽度(pix)"));
    img3DHeightName->setText(tr("图像高度(slice)"));
    img3DDepthName->setText(tr("图像深度(位)"));

    QGridLayout* img3DPropertyLayout = new QGridLayout();
    img3DPropertyLayout->addWidget(img3DLengthName, 0, 0, 0);
    img3DPropertyLayout->addWidget(img3DLengthNum, 0, 1, 0); 
    img3DPropertyLayout->addWidget(img3DWidthName, 1, 0, 0);
    img3DPropertyLayout->addWidget(img3DWidthNum, 1, 1, 0); 
    img3DPropertyLayout->addWidget(img3DHeightName, 2, 0, 0);
    img3DPropertyLayout->addWidget(img3DHeightNum, 2, 1, 0); 
    img3DPropertyLayout->addWidget(img3DDepthName, 3, 0,  0);
    img3DPropertyLayout->addWidget(img3DDepthNum, 3, 1,  0); 

    img3DPropertywidget->setLayout(img3DPropertyLayout);
    img3DPropertywidget->setFixedSize(260, 110);
    img3DPropertywidget->setVisible(false);
}

void mainwindowInterator::clearResultViewer()  
{
    if(ui->resultQVTKViewer->isEnabled())
    {
        ui->resultQVTKViewer->GetRenderWindow()->RemoveRenderer(resultRenderder);
        resultRenderder = vtkRenderer::New();
        ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
        ui->resultQVTKViewer->GetRenderWindow()->Render();
    }
}

void mainwindowInterator::resetControlPanel()
{
    if(pGeomtry != NULL)
    {
        pGeomtry->resetControlPanel();
    }
    if(pMorph != NULL)
    {
        pMorph->resetControlPanel();
    }
}

void mainwindowInterator::clear()
{
    if(pGeomtry != NULL)//1
    {
        delete pGeomtry;
        pGeomtry = NULL;
    }
    if(pMorph != NULL)//2
    {
        delete pMorph;
        pMorph = NULL;
    }
    if(pPointOpe != NULL)//3
    {
        delete pPointOpe;
        pPointOpe = NULL;
    }
    if(pFilter != NULL)//4
    {
        delete pFilter;
        pFilter = NULL;
    }
    if(pFourierTrans != NULL)//5
    {
        delete pFourierTrans;
        pFourierTrans = NULL;
    }
    if(pEdgeContour != NULL)//6
    {
        delete pEdgeContour;
        pEdgeContour = NULL;
    }
    if(pDicomPro != NULL)//7
    {
        delete pDicomPro;
        pDicomPro = NULL;
    }
    if(pSegmentation != NULL)//8
    {
        delete pSegmentation;
        pSegmentation = NULL;
    }
}


void mainwindowInterator::on_action_Open_triggered()
{
    clearResultViewer();
    resetControlPanel();
    initVisualizationPipeline();

    QString filter;
    filter = tr("(*.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.png *.PNG *.tiff *.TIFF)");
    qstrInputFileName = QFileDialog::getOpenFileName(this, tr("On Action Trigerred"), QDir::currentPath(), filter);
    if (qstrInputFileName.isEmpty()) 
    {
        QMessageBox::warning(this, tr("Warning Message On_Acction_Triggered"), tr("Warning Message On_Acction_Triggered 2"), QMessageBox::Ok);
        return;
    }
    if (!qstrInputFileName.isEmpty()) 
    {
        ba = qstrInputFileName.toLatin1(); //QString to char*
        inputFileName = ba.data();
        originalReader = originalReaderFactory->CreateImageReader2(inputFileName);
        if (originalReader)
        {
            originalReader->SetFileName(inputFileName);
            originalReader->Update();
            pixelType = originalReader->GetDataScalarType();
            dimension = 2;//维度
        }
        else
        {
            QMessageBox::warning(this, tr("Triggered 2"),
                tr("Warning JPEG, JPG, PNG, BMP, TIFF"), 
                QMessageBox::Ok);
            return;
        }

        img = cvLoadImage(inputFileName, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
        img2DWidthNum->setText(QString::number(img->width, 10));
        img2DHeightNum->setText(QString::number(img->height, 10));
        img2DDepthNum->setText(QString::number(img->depth, 10));
        img2DChannelsNum->setText(QString::number(img->nChannels, 10));
        if(!img2DPropertywidget->isVisible())
        {
            ui->controlPanelLayout->addWidget(img2DPropertywidget);
            img2DPropertywidget->setVisible(true);
        }
        originalVtkViewer->UpdateDisplayExtent();
        originalVtkViewer->SetInputConnection(originalReader->GetOutputPort());
        originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());  
        originalVtkViewer->SetRenderer(originalRenderder);  
        originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());

        if(img->nChannels != 3)
        {
            originalVtkViewer->SetColorLevel(128);
            originalVtkViewer->SetColorWindow(256);
        }
        originalVtkViewer->SetSliceOrientationToXY();
        originalVtkViewer->GetImageActor()->InterpolateOff();  
        originalRenderder->ResetCamera();  
        originalRenderder->DrawOn(); 
        //originalVtkViewer->Render();
        ui->originalQVTKViewer->GetRenderWindow()->Render();
    }

    clear();

    if(ui->logoLable != NULL)
    {
        delete ui->logoLable;
        ui->logoLable = NULL;
    }

    if(img3DPropertywidget->isVisible())
    {
        img3DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img3DPropertywidget);//don't work
        //ui->controlPanelLayout->removeItem(ui->controlPanelLayout);
        ui->controlPanelLayout->update();
    }
}

void mainwindowInterator::on_action_Open_3DImage_triggered()
{
    clearResultViewer();
    resetControlPanel();
    initVisualizationPipeline();

    QString filter;
    filter = tr("(*.mhd *.raw *.vtk)");
    QString fileName = QFileDialog::getOpenFileName( this, QString(tr("打开图像")), QDir::currentPath() , filter );
    if ( fileName.isEmpty() == true ) return;
    // QString to char*
    ba = fileName.toLocal8Bit();
    inputFileName = ba.data();

    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());

    if(fileExtension == "mhd" || fileExtension == "MHD")
    {
        mhdReader = vtkMetaImageReader::New();
        mhdReader->SetFileName(inputFileName);
        mhdReader->Update();//cannot delete

        pixelType =  mhdReader->GetDataScalarType();

        dimension = mhdReader->GetFileDimensionality();
        imgSize = mhdReader->GetOutput()->GetExtent();
        rotate = vtkImageFlip::New();
        rotate->SetInputConnection(mhdReader->GetOutputPort());
        rotate->SetFilteredAxis(1);
        originalVtkViewer->SetInputConnection(rotate->GetOutputPort());
    }
    else  if(fileExtension == "vtk" || fileExtension == "VTK")
    {
        vtkReader = vtkStructuredPointsReader::New();
        vtkReader->SetFileName(inputFileName);
        vtkReader->Update();//cannot delete
        int dim[6] = {0, 180, 0, 216, 0, 180};
        imgSize = dim;
        pixelType = VTK_UNSIGNED_CHAR;
        dimension = 3;

        originalVtkViewer->SetInputConnection(vtkReader->GetOutputPort());
    }
    else  if(fileExtension == "raw" || fileExtension == "RAW")
    {
        rawReader = vtkImageReader::New();
        rawReader->SetFileName(inputFileName);
        rawReader->Update();//cannot delete
        pixelType =  rawReader->GetDataScalarType();
        rawReader->SetFileDimensionality(3); 
        rawReader->SetDataScalarType(pixelType);
        rawReader->SetDataExtent(0, 511, 0, 479, 0, 63);
        rawReader->SetDataSpacing(1, 1, 1);
        imgSize = rawReader->GetDataExtent();
        dimension = rawReader->GetFileDimensionality();
        originalVtkViewer->SetInputConnection(rawReader->GetOutputPort());
    }
    else
    {
        QMessageBox::warning(this, tr("on_action_Open_3DImage_triggered 1"),
            tr("on_action_Open_3DImage_triggered 2 MHD, RAW, VTK"), QMessageBox::Ok);
        return;
    }

    originalVtkViewer->UpdateDisplayExtent();
    ui->originalQVTKViewer->GetRenderWindow()->AddRenderer(originalRenderder);
    originalVtkViewer->SetRenderer(originalRenderder);
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());
    pCall = vtkNextSliceCallbk::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer, resultVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);
    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());
    originalVtkViewer->SetSliceOrientationToXY();
    if(pixelType == VTK_UNSIGNED_SHORT || pixelType == VTK_SHORT)
    {
        imgDepth = 16;
        originalVtkViewer->SetColorLevel(32768);
        originalVtkViewer->SetColorWindow(65536);
    }
    else//VTK_UNSIGNED_CHAR
    {
        imgDepth = 8;
        originalVtkViewer->SetColorLevel(128); 
        originalVtkViewer->SetColorWindow(256); 
    }

    img3DLengthNum->setText(QString::number(imgSize[1] + 1, 10));//int ->QString 
    img3DWidthNum->setText(QString::number(imgSize[3] + 1, 10));//int ->QString
    img3DHeightNum->setText(QString::number(imgSize[5] + 1, 10));//int ->QString
    img3DDepthNum->setText(QString::number(imgDepth, 10));//int ->QString
    if(!img3DPropertywidget->isVisible())
    {
        ui->controlPanelLayout->addWidget(img3DPropertywidget);
        img3DPropertywidget->setVisible(true);
    }

    clear();

    if(ui->logoLable != NULL)
    {
        delete ui->logoLable;
        ui->logoLable = NULL;
    }

    if(img2DPropertywidget->isVisible())
    {
        img2DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img2DPropertywidget);
        ui->controlPanelLayout->update();
    }


    originalRenderder->ResetCamera();
    originalRenderder->Render();
    //originalRenderder->DrawOn();
    ui->originalQVTKViewer->GetRenderWindow()->Render();
}

void mainwindowInterator::on_action_OpenDICOM_triggered()
{
    clearResultViewer();
    resetControlPanel();
    initVisualizationPipeline();
    QString fileName = QFileDialog::getExistingDirectory( this, QString(tr("on_action_OpenDICOM_triggered")), QDir::currentPath());
    if ( fileName.isEmpty() == true ) return;
    // QString to char*
    ba = fileName.toLocal8Bit();
    inputFileName = ba.data();
    dicomReader = vtkDICOMImageReader::New();
    dicomReader->SetDirectoryName(inputFileName);
    dicomReader->Update();
    dimension = dicomReader->GetFileDimensionality();
    pixelType = dicomReader->GetDataScalarType();
    originalVtkViewer->SetInputConnection(dicomReader->GetOutputPort());
    originalVtkViewer->UpdateDisplayExtent();

    originalVtkViewer->SetRenderer(originalRenderder);
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());
    pCall = vtkNextSliceCallbk::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer, resultVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);
    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());
    originalVtkViewer->SetSliceOrientationToXY();
    originalVtkViewer->SetColorLevel(128); 
    originalVtkViewer->SetColorWindow(256);

    if(ui->logoLable != NULL)
    {
        delete ui->logoLable;
        ui->logoLable = NULL;
    }

    if(img2DPropertywidget->isVisible())
    {
        img2DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img2DPropertywidget);
        ui->controlPanelLayout->update();
    }
    if(img3DPropertywidget->isVisible())
    {
        img3DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img3DPropertywidget);
        ui->controlPanelLayout->update();
    }
    clear();
    pDicomPro = new dicomProcess(this);
    ui->controlPanelLayout->addWidget(pDicomPro);
    pDicomPro->setFixedSize(330, 600);
    pDicomPro->inputFileName = this->inputFileName;
    pDicomPro->showImgProperty(dicomReader);

    originalVtkViewer->Render();
    //ui->originalQVTKViewer->GetRenderWindow()->Render();
}

bool mainwindowInterator::isSurpported2DImgType()
{
    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());
    if(fileExtension != "jpg" && fileExtension != "peg" && fileExtension != "bmp" 
        && fileExtension != "png" && fileExtension != "iff")//"peg"->"jpeg", "iff"->"tiff"
    {
        QMessageBox::warning(this, tr("isSurpported2DImgType 1"),
            tr("<p>isSurpported2DImgType 2</p>"
            " <p>isSurpported2DImgType jpg, jpeg, bmp, tiff, png</p>"),  QMessageBox::Ok);
        return false;
    }
    return true;
}

bool mainwindowInterator::isSurpported3DImgType()
{
    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());
    if(fileExtension != "mhd" && fileExtension != "raw" && fileExtension != "dcm" 
        && fileExtension != "vtk" )
    {
        QMessageBox::warning(this, tr("isSurpported3DImgType"),
            tr("<p>isSurpported3DImgType 1</p>"
            " <p> isSurpported3DImgType mhd, raw, dcm, vtk</p>"),  QMessageBox::Ok);
        return false;
    }
    return true;
}

bool mainwindowInterator::isMHDImgType()
{
    string fileExtension;
    fileExtension = inputFileName;//char* to string
    fileExtension = fileExtension.substr(fileExtension.length() - 3, fileExtension.length());
    if(fileExtension != "mhd" || fileExtension != "MHD")
    {
        QMessageBox::warning(this, tr("isMHDImgType"),
            tr("<p>isMHDImgType 2</p>"
            " <p>isMHDImgType 3</p>"),  QMessageBox::Ok);
        return false;
    }
    return true;
}

void mainwindowInterator::on_action_Save_triggered()
{
    QString outputFileName = QFileDialog::getSaveFileName(this, tr("保存文件"), "F:", tr("图片文件(*png *jpg *bmp *tiff )"));
    QPixmap* pix = new QPixmap();
    QString qstr= QString(QLatin1String(inputFileName));
    pix->load(qstr);
    pix->save(outputFileName);
}

void mainwindowInterator::on_action_Color2Gray_triggered()
{

    if(!isSurpported2DImgType())
    {
        return;
    }

    clear();
    pPointOpe = new pointOperation();

    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pPointOpe->color2Gray(inputFileName);
}

void mainwindowInterator::on_action_HistgramEqualization_triggered()
{

    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    pPointOpe = new pointOperation();
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pPointOpe->histgramEqualization(inputFileName);
}

void mainwindowInterator::on_action_Interpolation_triggered()
{

    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    pPointOpe = new pointOperation();
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pPointOpe->interpolation(inputFileName);
}

void mainwindowInterator::on_action_GrayHist_triggered()
{
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    pPointOpe = new pointOperation();
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pPointOpe->grayHistgram(inputFileName);
}

void mainwindowInterator::on_action_ColorHist_triggered()
{
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    pPointOpe = new pointOperation();
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pPointOpe->colorHistgram(inputFileName);
}

void mainwindowInterator::on_action_SingleChannel_triggered()
{
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    pPointOpe = new pointOperation();
    connect(pPointOpe, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));//返回主窗口更新视图
    pPointOpe->getSingleChannelImg(inputFileName);
}

void mainwindowInterator::on_action_Geometry_triggered() 
{
    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    pGeomtry = new geometryTransform(this);
    ui->controlPanelLayout->addWidget(pGeomtry);
    pGeomtry->setVisible(true);
    pGeomtry->inputFileName = &qstrInputFileName;

    connect(pGeomtry, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    connect(pGeomtry, SIGNAL(reset()), this, SLOT(clearResultViewer()));
}

void mainwindowInterator::on_action_Fourier_triggered()
{

    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    pFourierTrans = new fourierTransform();

    connect(pFourierTrans, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFourierTrans->discreteFourierTrans(inputFileName);
}

void mainwindowInterator::on_action_Morphology_triggered()
{

    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    pMorph = new morphology(this);
    ui->controlPanelLayout->addWidget(pMorph);
    pMorph->setVisible(true);
    pMorph->inputFileName = inputFileName;

    connect(pMorph, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    connect(pMorph, SIGNAL(reset()), this, SLOT(clearResultViewer()));
}

void mainwindowInterator::on_action_MeanFilter_triggered()
{
    clear();
    pFilter = new itkFilter();
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->meanFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_GaussianFilter_triggered()
{
    clear();
    pFilter = new itkFilter();
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->gaussianFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_BilateralFilter_triggered()
{
    clear();
    pFilter = new itkFilter();
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->bilateralFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_LaplacianFilter_triggered()
{
    clear();
	pFilter = new itkFilter();
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->laplacianFilter(inputFileName, dimension, pixelType);
}

void mainwindowInterator::on_action_CurvatureAnisotropicDiffusionFilter_triggered()
{ 
    clear();
    pFilter = new itkFilter();
    connect(pFilter, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pFilter->curvatureAnisotropicDiffusionFilter(inputFileName, dimension, pixelType);   
}

void mainwindowInterator::on_action_Edge_triggered()
{

    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    pEdgeContour = new edgeContour();
    connect(pEdgeContour, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pEdgeContour->sobelOperator(inputFileName);
}

void mainwindowInterator::on_action_RegionGrowSeg_triggered()
{
    QMessageBox::warning(this, tr("on_action_RegionGrowSeg_triggered"), tr("on_action_RegionGrowSeg_triggered 2"), QMessageBox::Ok);
    return;
}

void mainwindowInterator::on_action_WatershedSeg_triggered()
{
    QMessageBox::warning(this, tr("on_action_WatershedSeg_triggered 1"), tr("on_action_WatershedSeg_triggered 2"), QMessageBox::Ok);
    return;
}

void mainwindowInterator::on_action_PEDSeg_triggered()
{
    clear();
    pSegmentation = new segmentation();
    pSegmentation->setFixedSize(270, 300);
    ui->controlPanelLayout->addWidget(pSegmentation);
    pSegmentation->inputFileName = inputFileName;
    connect(pSegmentation, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    connect(pSegmentation, SIGNAL(returnInternalFileName(char*)), this, SLOT(updateOriginalViewer(char*)));
    connect(pSegmentation, SIGNAL(executeRetinalLayerVisualization()), this, SLOT(retinalLayerVisualization()));
    connect(pSegmentation, SIGNAL(executeAbnormalRegionVisualization()), this, SLOT(abnormalRegionVisualization()));
}

void mainwindowInterator::retinalLayerVisualization()
{
    progDlg = new QProgressDialog();
    progDlg->setWindowTitle("Please wait...");  
    progDlg->setRange(0, 100);
    progDlg->setFixedSize(350, 100);
    progDlg->show();
    timer = new QTimer();
    currentValue = 0;
    progDlg->setValue(currentValue);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProgressDialog()));
    timer->start(100);

    resultRenderder = vtkRenderer::New();
    vtkRenderWindow* renwin = vtkRenderWindow::New();
    renwin->AddRenderer(resultRenderder);
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    resultRenWinInteractor->SetRenderWindow(renwin);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
    ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);

    int surface[64][512];
    int dims[3]= {512, 480, 64};
    double R[4],G[4],B[4];//give color to retina surface
    R[0]=1;R[1]=0;R[2]=0;R[3]=1;
    G[0]=0;G[1]=1;G[2]=0;G[3]=1;
    B[0]=0;B[1]=0;B[2]=1;B[3]=0;

    ifstream fin;
    string str1[4] = {"1", "7", "11", "12"};
    string  str2=".txt";
    for(int slice = 0; slice < 4; slice++)
    {
        string  str ="F:/qt/project_try/imageProcess_2_3/data/ped/txt/15715_";
        str = str + str1[slice] + str2;
        fin.open(str,ios::in);
        if(fin.fail())
        {
            cout<<"File";
            exit(1);
        }
        for(int k=0;k<dims[2];k++)
            for(int i=0;i<dims[0];i++)
            {
                fin>>surface[k][i];
            }
            fin.close(); 
            fin.clear(); 

            vtkImageData *retinalSurfaceData = vtkImageData::New();
            int dimsSp[3];
            dimsSp[0]=512/2;
            dimsSp[1]=480/2;
            dimsSp[2]=64*4;
            retinalSurfaceData->SetDimensions(dimsSp);
            retinalSurfaceData->SetSpacing(1,1,1);
            //retinalSurfaceData->SetScalarTypeToUnsignedChar();

            for(int k=0;k<256;k++)
                for(int j=0;j<240;j++)
                    for(int i=0;i<256;i++)
                    {
                        unsigned char *pixel = (unsigned char*)(retinalSurfaceData->GetScalarPointer(i,j,k));
                        *pixel = 0;
                    }//must initiate a new memory

                    for(int k=0;k<dims[2]-1;k++)//z axis
                        for(int i=0;i<dims[0]/2;i++)//y axis
                        {
                            int z1 = surface[k][i*2]/2+slice*10;
                            int z2 = surface[k+1][i*2]/2+slice*10;
                            double zD = (double(z2)-double(z1))/4;
                            for(int j=0;j<4;j++)
                            {
                                unsigned char *pixel = (unsigned char*)(retinalSurfaceData->GetScalarPointer(i,(z1+zD*j),(k*4+j)));
                                *pixel = 2;//give all pixels a same intensity
                                unsigned char *pixel1 = (unsigned char*)(retinalSurfaceData->GetScalarPointer(i,(z1+zD*j+1),(k*4+j)));
                                *pixel1 = 2;
                                unsigned char *pixel2 = (unsigned char*)(retinalSurfaceData->GetScalarPointer(i,(z1+zD*j-1),(k*4+j)));
                                *pixel2 = 2;
                            }
                        }

                        vtkMarchingCubes *retinalSurfaceExtractor = vtkMarchingCubes::New();
                        retinalSurfaceExtractor->SetInputData(retinalSurfaceData);
                        retinalSurfaceExtractor->SetValue(slice, 2);
                        vtkDecimatePro *deci=vtkDecimatePro::New();
                        deci->SetInputConnection(retinalSurfaceExtractor->GetOutputPort());  
                        deci->SetTargetReduction(0.7);
                        vtkSmoothPolyDataFilter *smooth=vtkSmoothPolyDataFilter::New();   
                        smooth->SetInputConnection(deci->GetOutputPort());  
                        smooth->SetNumberOfIterations(100) ;
                        vtkPolyDataNormals *retinalSurfaceNormals = vtkPolyDataNormals::New();
                        retinalSurfaceNormals->SetInputConnection(smooth->GetOutputPort());
                        retinalSurfaceNormals->SetFeatureAngle(60.0);
                        vtkStripper *stripper=vtkStripper::New();   
                        stripper->SetInputData(retinalSurfaceNormals->GetOutput());  
                        vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
                        mapper->SetInputConnection(stripper->GetOutputPort());
                        mapper->ScalarVisibilityOff();
                        vtkActor *retinalSurface = vtkActor::New();
                        retinalSurface->SetMapper(mapper);
                        retinalSurface->GetProperty()->SetDiffuseColor(R[slice],G[slice],B[slice]);//give a specific color of surface
                        resultRenderder->AddActor(retinalSurface);
                        retinalSurfaceExtractor->Delete();
                        mapper->Delete();
                        retinalSurfaceNormals->Delete();
                        retinalSurface->Delete();
    }

    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp(0,-5,0);
    aCamera->SetPosition(0,0,1);
    aCamera->SetFocalPoint(0,0,0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->SetActiveCamera(aCamera);
    resultRenderder->ResetCamera();
    aCamera->Dolly(1.5);
    resultRenderder->SetBackground(0.0, 0.0, 0.0);
    resultRenderder->ResetCameraClippingRange();

    //resultRenWinInteractor->Initialize();
    //resultRenWinInteractor->Start();
    ui->resultQVTKViewer->GetRenderWindow()->Render();
    //timer->stop();
    delete progDlg;
}

void mainwindowInterator::abnormalRegionVisualization()
{

    resultRenderder = vtkRenderer::New();
    vtkRenderWindow* renwin = vtkRenderWindow::New();
    renwin->AddRenderer(resultRenderder);
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    resultRenWinInteractor->SetRenderWindow(renwin);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);//必须有
    ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);

    mhdReader = vtkMetaImageReader::New();
    mhdReader->SetFileName("../data/ped/15715_3d_result.mhd");

    double R[3],G[3],B[3];
    R[0]=1;R[1]=0;R[2]=0; 
    G[0]=0;G[1]=1;G[2]=0; 
    B[0]=0;B[1]=0;B[2]=1; 

    for(int i = 1; i < 3; i++)//error
    {
        vtkMarchingCubes *retinalSurfaceExtractor = vtkMarchingCubes::New();
        retinalSurfaceExtractor->SetInputConnection(mhdReader->GetOutputPort());
        retinalSurfaceExtractor->SetValue(i, i + 1);
        vtkDecimatePro *deci=vtkDecimatePro::New();
        deci->SetInputConnection(retinalSurfaceExtractor->GetOutputPort());  
        deci->SetTargetReduction(0.7);
        vtkSmoothPolyDataFilter *smooth=vtkSmoothPolyDataFilter::New();   
        smooth->SetInputConnection(deci->GetOutputPort());  
        smooth->SetNumberOfIterations(100) ;
        vtkPolyDataNormals *retinalSurfaceNormals = vtkPolyDataNormals::New();
        retinalSurfaceNormals->SetInputConnection(smooth->GetOutputPort());
        retinalSurfaceNormals->SetFeatureAngle(60.0);
        vtkStripper *stripper=vtkStripper::New();   
        stripper->SetInputData(retinalSurfaceNormals->GetOutput());  
        vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
        mapper->SetInputConnection(stripper->GetOutputPort());
        mapper->ScalarVisibilityOff();
        vtkActor *retinalSurface = vtkActor::New();
        retinalSurface->SetMapper(mapper);
        retinalSurface->GetProperty()->SetDiffuseColor(R[i],G[i],B[i]);//give a specific color of surface
        resultRenderder->AddActor(retinalSurface);

        retinalSurfaceExtractor->Delete();
        mapper->Delete();
        retinalSurfaceNormals->Delete();
        retinalSurface->Delete();
    }

    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp(0, -5, 0);
    aCamera->SetPosition(0, 0, 1);
    aCamera->SetFocalPoint(0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->SetActiveCamera(aCamera);
    //resultRenderder->ResetCamera(); 
    aCamera->Dolly(1.5);
    resultRenderder->SetBackground(0.0, 0.0, 0.0);
    resultRenderder->ResetCameraClippingRange();
    ui->resultQVTKViewer->GetRenderWindow()->Render();
    //resultRenWinInteractor->Initialize();
    //resultRenWinInteractor->Start();
}

void mainwindowInterator::on_action_Registration_2D_triggered()
{

    if(!isSurpported2DImgType())
    {
        return;
    }
    clear();
    QString filter;
    filter = tr(" (*.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.png *.PNG *.tiff *.TIFF)");
    QString qstrInputFileName2 = QFileDialog::getOpenFileName(this,
        tr("on_action_Registration_2D_triggered 1"), QDir::currentPath(), filter);
    if (qstrInputFileName2.isEmpty()) 
    {
        QMessageBox::warning(this, tr("on_action_Registration_2D_triggered 2"), tr("on_action_Registration_2D_triggered 3"), QMessageBox::Ok);
        return;
    }
    char* movingImageName;
    QByteArray ba = qstrInputFileName2.toLatin1(); //QString to char*
    movingImageName = ba.data();
    pRegistration = new registration();
    connect(pRegistration, SIGNAL(returnOutputFileName(char*)), this, SLOT(updateResultViewer(char*)));
    pRegistration->itkDemonsRegistration(inputFileName, movingImageName);
}

void mainwindowInterator::on_action_VolumeRender_triggered() 
{
    isMHDImgType();

    resultRenderder = vtkRenderer::New();
    vtkRenderWindow* renwin = vtkRenderWindow::New();
    renwin->AddRenderer(resultRenderder);
    resultRenWinInteractor = vtkRenderWindowInteractor::New();
    resultRenWinInteractor->SetRenderWindow(renwin);
    ui->resultQVTKViewer->GetRenderWindow()->AddRenderer(resultRenderder);
    ui->resultQVTKViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);

    mhdReader = vtkMetaImageReader::New();
    mhdReader->SetFileName(inputFileName);

    vtkPiecewiseFunction *opacityTransferFunction=vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint( 20, 0.0);
    opacityTransferFunction->AddPoint(255, 0.7);

    vtkColorTransferFunction *colorTransferFunction=vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(  0.0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint( 64.0, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(128.0, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(192.0, 0.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(255.0, 0.0, 0.2, 0.0);

    vtkVolumeProperty *volumeProperty=vtkVolumeProperty::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    volumeProperty->ShadeOn();
    volumeProperty->SetInterpolationTypeToLinear();

    vtkVolumeRayCastCompositeFunction  *compositeFunction = vtkVolumeRayCastCompositeFunction::New();
    vtkVolumeRayCastMapper *volumeMapper=vtkVolumeRayCastMapper::New();
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
    volumeMapper->SetInputConnection(mhdReader->GetOutputPort());//unsigned char or unsigned short

    vtkVolume *volume=vtkVolume::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);
    resultRenderder->AddVolume(volume);

    vtkCamera *aCamera = vtkCamera::New(); 
    aCamera->SetViewUp(0,-5,0);
    aCamera->SetPosition(0,0,1);
    aCamera->SetFocalPoint(0,0,0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->SetActiveCamera(aCamera);
    resultRenderder->ResetCamera();
    aCamera->Dolly(1.5);
    resultRenderder->SetBackground(0.0, 0.0, 0.0);
    resultRenderder->ResetCameraClippingRange();
    ui->resultQVTKViewer->GetRenderWindow()->Render();
}

void mainwindowInterator::on_action_SurfaceRender_triggered()
{
    progDlg = new QProgressDialog();
    progDlg->setWindowTitle("Please wait...");  
    progDlg->setRange(0, 100);
    progDlg->show();
    timer = new QTimer();
    currentValue = 0;
    progDlg->setValue(currentValue);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateProgressDialog()));
    timer->start(100);

    if(ui->logoLable == NULL)
    {
        QLabel* logo = new QLabel(this);
        logo->setPixmap(QPixmap("../data/logo/logo.jpg"));
        logo->setFixedSize(260, 300);
        ui->controlPanelLayout->addWidget(logo);
    }
    clear();

    clearResultViewer();

    if(img2DPropertywidget->isVisible())
    {
        img2DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img2DPropertywidget);
        ui->controlPanelLayout->update();
    }
    if(img3DPropertywidget->isVisible())
    {
        img3DPropertywidget->setVisible(false);
        ui->controlPanelLayout->removeWidget(img3DPropertywidget);
        ui->controlPanelLayout->update();
    }

    initVisualizationPipeline();
    vtkVolume16Reader *v16 = vtkVolume16Reader::New();
    v16->SetDataDimensions (64, 64);
    v16->SetImageRange (1, 93);
    v16->SetDataByteOrderToLittleEndian();
    v16->SetFilePrefix ("../data/headsq/quarter");
    v16->SetDataSpacing (3.2, 3.2, 1.5);

    originalVtkViewer->SetInputConnection(v16->GetOutputPort());
    originalVtkViewer->SetRenderer(originalRenderder);
    originalVtkViewer->SetRenderWindow(ui->originalQVTKViewer->GetRenderWindow());
    vtkNextSliceCallbk2* pCall = vtkNextSliceCallbk2::New();
    style = vtkInteractorStyleImage::New();
    style->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall); 
    style->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    originalRenWinInteractor->SetInteractorStyle(style);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelBackwardEvent, pCall);
    originalRenWinInteractor->AddObserver(vtkCommand::MouseWheelForwardEvent, pCall);
    pCall->SetInteractorStyle(style);
    pCall->SetViewer(originalVtkViewer);
    pCall->SetInteractor(originalRenWinInteractor);
    ui->originalQVTKViewer->GetRenderWindow()->SetInteractor(originalRenWinInteractor);
    originalVtkViewer->SetupInteractor(ui->originalQVTKViewer->GetRenderWindow()->GetInteractor());
    originalVtkViewer->SetSliceOrientationToXY();
    originalVtkViewer->SetColorLevel(128); 
    originalVtkViewer->SetColorWindow(256); 
    originalVtkViewer->Render();
    ui->originalQVTKViewer->GetRenderWindow()->Render();

    vtkContourFilter *skinExtractor = vtkContourFilter::New();
    skinExtractor->SetInputConnection(v16->GetOutputPort());
    skinExtractor->SetValue(0, 500);
    vtkPolyDataNormals *skinNormals = vtkPolyDataNormals::New();
    skinNormals->SetInputConnection(skinExtractor->GetOutputPort());
    skinNormals->SetFeatureAngle(60.0);
    vtkStripper *stripper = vtkStripper::New(); 
    stripper->SetInputData(skinNormals->GetOutput());  
    vtkPolyDataMapper *skinMapper = vtkPolyDataMapper::New();
    skinMapper->SetInputConnection(stripper->GetOutputPort());
    skinMapper->ScalarVisibilityOff();
    vtkActor *skin = vtkActor::New();
    skin->SetMapper(skinMapper);

    vtkOutlineFilter *outlineData = vtkOutlineFilter::New();
    outlineData->SetInputConnection(v16->GetOutputPort());
    vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();
    mapOutline->SetInputConnection(outlineData->GetOutputPort());
    vtkActor *outline = vtkActor::New();
    outline->SetMapper(mapOutline);
    outline->GetProperty()->SetColor(0,0,0);

    vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp (0, 0, -1);
    aCamera->SetPosition (0, 1, 0);
    aCamera->SetFocalPoint (0, 0, 0);
    aCamera->ComputeViewPlaneNormal();
    resultRenderder->AddActor(skin);
    resultRenderder->AddActor(outline);
    resultRenderder->SetActiveCamera(aCamera);
    resultRenderder->ResetCamera ();
    aCamera->Dolly(1.5);

    resultRenderder->ResetCameraClippingRange ();
    resultVtkViewer->GetRenderWindow()->SetInteractor(resultRenWinInteractor);
    resultVtkViewer->Render();
    //timer->stop();
    //delete progDlg;
}


mainwindowInterator::~mainwindowInterator()
{
    delete pGeomtry;
    delete pFourierTrans;
    delete pMorph;
    delete pFilter;
    delete pEdgeContour;
    delete pDicomPro;
    delete pSegmentation;
}