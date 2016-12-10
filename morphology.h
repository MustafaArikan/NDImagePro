#ifndef MORPHOLOGY_H
#define MORPHOLOGY_H

#include <QtGui>
#include <QPixmap>

#include "ui_morphology.h"
#include "highgui.h"
#include "cv.h"

class morphology : public QWidget
{
    Q_OBJECT

public:
    morphology(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~morphology();

    Ui::morphology* ui;

    
private:
   char* outputFileName;
    IplImage* inputImage, *outputImage;
    IplConvKernel* se;
    int iteration;
    int shape;
    int kernelColumns, kernelRows;

public:
     char* inputFileName;

    
private:
    void createConnect();
    void initUI();
    void setKernelShape();
public:
    void resetControlPanel();

signals:

    void returnOutputFileName(char* fileName);
    void reset();

    private slots:

        void on_dilationButton_clicked();
        void on_erosionButton_clicked();
        void on_openButton_clicked();
        void on_closeButton_clicked();
        void on_hatButton_clicked();
        void on_blackHatButton_clicked();
        void on_resetButton_clicked();

        void on_iterationSlider_valueChanged(int value);
        //spinBox
        void on_kernelColumnSpinBox_valueChanged(int value);
        void on_kernelRowSpinBox_valueChanged(int value);
};

#endif // MORPHOLOGY_H
