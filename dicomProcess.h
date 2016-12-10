#ifndef DICOMPROCESS_H
#define DICOMPROCESS_H

#include <QtGui>
#include "ui_dicomProcess.h"
#include <vtkDICOMImageReader.h>

class dicomProcess : public QWidget
{
    Q_OBJECT

public:
    dicomProcess(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~dicomProcess();

    Ui::dicomProcess* ui;

private:
    bool isConnected;
    char* nameInfo;
    char* ID;
    char* time;
    char* manufacture;
    char* modility;
    char* hospital;

public:
    char* inputFileName;

public:
    void showImgProperty(vtkDICOMImageReader* reader);

signals:

    private slots:
        void on_addButtonSQL_clicked();
        void on_deleteButtonSQL_clicked();
        void on_modifyButtonSQL_clicked();
        void on_queryButtonSQL_clicked();
        void on_clearButtonSQL_clicked();
        void on_showAllTableButtonSQL_clicked();
};

#endif // DICOMPROCESS_H
