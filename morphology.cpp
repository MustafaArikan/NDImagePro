#include "morphology.h"
#include <QPixmap>

morphology::morphology(QWidget *parent, Qt::WindowFlags flags)
    :QWidget(parent, flags),
    ui(new Ui::morphology)
{
    ui->setupUi(this);

    iteration = 1;
    shape = CV_SHAPE_RECT;
    kernelColumns = 3;
    kernelRows = 3;

    initUI();

    createConnect();
}


void morphology::initUI()
{
    ui->rectSE->setChecked(true);
    ui->iterationSlider->setValue(1);
    ui->iterationSpinBox->setValue(1);
    ui->kernelColumnSpinBox->setValue(3);
    ui->kernelRowSpinBox->setValue(3);
}

void morphology::createConnect()
{
    connect(ui->iterationSlider, SIGNAL(valueChanged(int)), ui->iterationSpinBox, SLOT(setValue(int)));
    connect(ui->iterationSpinBox, SIGNAL(valueChanged(int)), ui->iterationSlider, SLOT(setValue(int)));
}

void morphology::setKernelShape()
{
    if(ui->rectSE->isChecked())
    {
        shape = CV_SHAPE_RECT;
    }
    if(ui->crossSE->isChecked())
    {
        shape = CV_SHAPE_CROSS;
    }
    if(ui->ellipseSE->isChecked())
    {
        shape = CV_SHAPE_ELLIPSE;
    }
}

void morphology::resetControlPanel()
{
    initUI();
    iteration = 1;
    shape = CV_SHAPE_RECT;
}

void morphology::on_iterationSlider_valueChanged(int value)
{
    iteration = value;
}

void morphology::on_kernelColumnSpinBox_valueChanged(int value)
{
    kernelColumns = value;
}

void morphology::on_kernelRowSpinBox_valueChanged(int value)
{
    kernelRows = value;
}

void morphology::on_dilationButton_clicked()
{
    setKernelShape();
    inputImage = cvLoadImage(inputFileName, 0);
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);

    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvDilate(inputImage, outputImage, se, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*
    cvReleaseImage(&inputImage);

    emit returnOutputFileName(outputFileName);
}

void morphology::on_erosionButton_clicked()
{
    setKernelShape();
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);

    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvErode(inputImage, outputImage, se, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

	emit returnOutputFileName(outputFileName);
}

void morphology::on_openButton_clicked()
{
    setKernelShape();
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);
    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_OPEN, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);
}

void morphology::on_closeButton_clicked()
{
    setKernelShape();
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);

    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_CLOSE, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);
}

void morphology::on_hatButton_clicked()
{
    setKernelShape();
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);

    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_TOPHAT, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);
}

void morphology::on_blackHatButton_clicked()
{
    setKernelShape();
    inputImage = cvLoadImage(inputFileName, 0);//QString to char*
    outputImage = cvCreateImage(cvGetSize(inputImage), inputImage->depth, inputImage->nChannels);

    CvMat* temp = cvCreateMat(inputImage->height, inputImage->width, CV_8UC1);
    se = cvCreateStructuringElementEx(kernelColumns, kernelRows,  kernelColumns / 2, kernelRows / 2, shape);
    cvMorphologyEx(inputImage, outputImage, temp, se, CV_MOP_BLACKHAT, iteration);

    outputFileName = "../data/temp/fourierTransform.jpg";//char*
    cvSaveImage(outputFileName, outputImage);//char*

    cvReleaseImage(&inputImage);
    cvReleaseImage(&outputImage);

    emit returnOutputFileName(outputFileName);
}

void morphology::on_resetButton_clicked()
{
    resetControlPanel();
    emit reset();
}

morphology::~morphology()
{
    delete ui;
}