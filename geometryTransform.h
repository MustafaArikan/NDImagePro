#ifndef GEOMETRYTRANSFORM_H
#define GEOMETRYTRANSFORM_H

#include <QtGui>
#include "ui_geometryTransform.h"
#include <QPixmap>

class geometryTransform : public QWidget
{
    Q_OBJECT

public:
    geometryTransform(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~geometryTransform();

    enum{
        Xslider_scale = 1,  Yslider_scale, Xslider_rotate,  Yslider_rotate, 
    };

public:
    Ui::geometryTransform* ui;
    QPixmap* pix;
    QString* inputFileName;
    char* outputFileName;

private:
    int xFactor, yFactor, rotateFactor;

private:
    void initUI();
    void createConnect();
public:
    void resetControlPanel();

signals:
    void returnOutputFileName(char* fileName);
    void reset();

    private slots:
        void on_Xslider_translate_valueChanged(int value);
        void on_Yslider_translate_valueChanged(int value);
        void on_Xslider_scale_valueChanged(int value);
        void on_Yslider_scale_valueChanged(int value);
        void on_dial_rotate_valueChanged(int value);

        void on_translateButton_clicked();
        void on_scaleButton_clicked();
        void on_rotateButton_clicked();
        void on_resetButton_clicked();
};

#endif // GEOMETRYTRANSFORM_H
