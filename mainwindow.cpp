#include "mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QDebug>

mainWindow::mainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    ui(new Ui::mainWindowClass)
{
   ui->setupUi(this);

   ui->logoLable->setPixmap(QPixmap("../data/logo/logo.jpg"));

    showStateBar();
}

void mainWindow::showStateBar()
{
    ui->statusBar->showMessage(tr("Some Message 1"), 10000);

    QLabel* permanent = new QLabel(this);
    permanent->setFrameStyle(QFrame::Box | QFrame::Sunken);
    //permanent->setText("welcome to my blog: http://blog.csdn.net/learn_sunzhuli");
    permanent->setText(tr("QT, ITK, VTK, OPENCV | All in One"));
    ui->statusBar->addPermanentWidget(permanent);
}

void mainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("About"), tr("<p>About Text 1.0</p> <p>About Text</p><p>Some About</p>"));
}

void mainWindow::updateResultViewer(char* fileName)
{
  
}

void mainWindow::on_action_Open_triggered()
{
    
}

void mainWindow::on_action_OpenDICOM_triggered()
{

}

void mainWindow::on_action_Open_3DImage_triggered()
{

}

void mainWindow::on_action_Save_triggered()
{

}

void mainWindow::on_action_Color2Gray_triggered()
{
    
}

void mainWindow::on_action_HistgramEqualization_triggered()
{
   
}

void mainWindow::on_action_Interpolation_triggered()
{
   
}

void mainWindow::on_action_GrayHist_triggered()
{
    
}

void mainWindow::on_action_ColorHist_triggered()
{
   
}

void mainWindow::on_action_SingleChannel_triggered()
{
    
}

void mainWindow::on_action_Geometry_triggered()
{

}

void mainWindow::on_action_Morphology_triggered()
{

}

void mainWindow::on_action_Fourier_triggered()
{

}

void mainWindow::on_action_MeanFilter_triggered()
{
    
}

void mainWindow::on_action_GaussianFilter_triggered()
{

}

void mainWindow::on_action_BilateralFilter_triggered()
{

}

void mainWindow::on_action_LaplacianFilter_triggered()
{

}

void mainWindow::on_action_CurvatureAnisotropicDiffusionFilter_triggered()
{

}

void mainWindow::on_action_Edge_triggered()
{

}

void mainWindow::on_action_RegionGrowSeg_triggered()
{

}

void mainWindow::on_action_WatershedSeg_triggered()
{

}

void mainWindow::on_action_PEDSeg_triggered()
{

}

void mainWindow::on_action_Registration_2D_triggered()
{

}

void mainWindow::on_action_VolumeRender_triggered()
{

}

void mainWindow::on_action_SurfaceRender_triggered()
{

}


mainWindow::~mainWindow()
{
    delete ui;
}
