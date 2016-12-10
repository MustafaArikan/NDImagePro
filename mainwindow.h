#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "ui_imageProcess.h"

class mainWindow : public QMainWindow
{
    Q_OBJECT

public:
    mainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~mainWindow();

public:
    Ui::mainWindowClass* ui;
private:
    void showStateBar();
    void setAboutMenu();

    public slots:
        virtual void updateResultViewer(char* fileName);

        virtual void on_action_Open_triggered();
        virtual void on_action_Open_3DImage_triggered();
        virtual void on_action_OpenDICOM_triggered();
        virtual void on_action_Save_triggered();
        virtual void on_action_About_triggered();
        virtual void on_action_Color2Gray_triggered();
        virtual void on_action_ColorHist_triggered();
        virtual void on_action_GrayHist_triggered();
        virtual void on_action_Interpolation_triggered();
        virtual void on_action_SingleChannel_triggered();
        virtual void on_action_HistgramEqualization_triggered();
        virtual void on_action_Geometry_triggered();
        virtual void on_action_Morphology_triggered();
        virtual void on_action_Fourier_triggered();
        virtual void on_action_MeanFilter_triggered();
        virtual void on_action_GaussianFilter_triggered();
        virtual void on_action_BilateralFilter_triggered();
        virtual void on_action_LaplacianFilter_triggered();
        virtual void on_action_CurvatureAnisotropicDiffusionFilter_triggered();
        virtual void on_action_Edge_triggered();
        virtual void on_action_VolumeRender_triggered();
        virtual void on_action_SurfaceRender_triggered();
        virtual void on_action_RegionGrowSeg_triggered();
        virtual void on_action_WatershedSeg_triggered();
        virtual void on_action_PEDSeg_triggered();
        virtual void on_action_Registration_2D_triggered();
};

#endif // MAINWINDOW_H
