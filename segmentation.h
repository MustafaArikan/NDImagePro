#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <QtGui>
#include "ui_segmentation.h"
#include <string>
using namespace std;//for string type

class segmentation : public QWidget
{
    Q_OBJECT

public:
    segmentation(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~segmentation();

    Ui::segmentation* ui;

private:
    string filePrefix;
    char outputFileName[100];
    char initFileName[100];
    char* initResultFileName;

public:
    char* inputFileName;

signals:
     void returnOutputFileName(char* fileName);
     void returnInternalFileName(char* fileName);
     void executeRetinalLayerVisualization();
      void executeAbnormalRegionVisualization();

    private slots:
        void on_filterButton_clicked();
        void on_retinalLayerSegButton_clicked();
        void on_retinalLayerVisualizationButton_clicked();
        void on_abnormalRegionSegButton_clicked();
        void on_abnormalRegionVisualizationButton_clicked();
        void on_morphologyOperationButton_clicked();

private:
        void erosionOperation();
};

#endif // SEGMENTATION_H
