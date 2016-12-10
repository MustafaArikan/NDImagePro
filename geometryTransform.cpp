#include "geometryTransform.h"
#include <QTransform>

geometryTransform::geometryTransform(QWidget *parent , Qt::WindowFlags flags)
    :QWidget(parent, flags),
    ui(new Ui::geometryTransform)
{
    ui->setupUi(this);

    pix = new QPixmap();
    xFactor = 1;
    yFactor = 1;
    rotateFactor = 0;

    initUI();

    createConnect();
}

void geometryTransform::initUI()
{

    ui->x_translate_min->setText("0");
    ui->x_translate_max->setText("1000");
    ui->y_translate_min->setText("0");
    ui->y_translate_max->setText("1000");

    ui->x_scale_min->setText("1");
    ui->x_scale_max->setText("10");
    ui->y_scale_min->setText("1");
    ui->y_scale_max->setText("10");
}

void geometryTransform::createConnect()
{

    connect(ui->x_scale_current, SIGNAL(valueChanged(int)),  ui->Xslider_scale, SLOT(setValue(int)));//放缩
    connect(ui->Xslider_scale,SIGNAL(valueChanged(int)), ui->x_scale_current,   SLOT(setValue(int)));
    connect(ui->y_scale_current, SIGNAL(valueChanged(int)), ui->Yslider_scale, SLOT(setValue(int)));
    connect(ui->Yslider_scale, SIGNAL(valueChanged(int)), ui->y_scale_current, SLOT(setValue(int)));

    connect(ui->x_translate_current,  SIGNAL(valueChanged(int)), ui->Xslider_translate, SLOT(setValue(int)));//平移
    connect(ui->Xslider_translate, SIGNAL(valueChanged(int)), ui->x_translate_current,  SLOT(setValue(int)));
    connect(ui->y_translate_current, SIGNAL(valueChanged(int)),  ui->Yslider_translate, SLOT(setValue(int)));
    connect(ui->Yslider_translate, SIGNAL(valueChanged(int)), ui->y_translate_current,  SLOT(setValue(int)));

    connect(ui->dialNum, SIGNAL(valueChanged(int)), ui->dial_rotate, SLOT(setValue(int)));//旋转
    connect(ui->dial_rotate, SIGNAL(valueChanged(int)), ui->dialNum, SLOT(setValue(int)));
}

void geometryTransform::on_Xslider_scale_valueChanged(int value)
{

    ui->x_scale_current->setValue(value);
    xFactor = value;
}

void geometryTransform::on_Yslider_scale_valueChanged(int value)
{
    ui->y_scale_current->setValue(value);
    yFactor = value;
}

void geometryTransform::on_Xslider_translate_valueChanged(int value)
{
    ui->x_translate_current->setValue(value);
    xFactor = value;
}

void geometryTransform::on_Yslider_translate_valueChanged(int value)
{
    ui->y_translate_current->setValue(value);
    yFactor = value;
}

void geometryTransform::on_dial_rotate_valueChanged(int value)
{
    ui->dialNum->setValue(value);
    rotateFactor = value;
}

void geometryTransform::on_translateButton_clicked()
{
    pix->load(*inputFileName);
    QTransform transform;
    transform.translate(xFactor, yFactor);

    *pix = pix->transformed(transform);
    pix->save("../data/temp/imgTranslate.jpg");

     outputFileName = ("../data/temp/imgTranslate.jpg");
    emit returnOutputFileName(outputFileName);
}

void geometryTransform::on_scaleButton_clicked()
{
     pix->load(*inputFileName);

    QTransform transform;
    transform.scale(xFactor, yFactor);
    *pix = pix->transformed(transform);
    pix->save("../data/temp/imgScale.jpg");
    //delete pix;//释放资源
    outputFileName = ("../data/temp/imgScale.jpg");
    emit returnOutputFileName(outputFileName);
}

void geometryTransform::on_rotateButton_clicked()
{
     pix->load(*inputFileName);

    QTransform transform;
    transform.rotate(rotateFactor);
    *pix = pix->transformed(transform);
    pix->save("../data/temp/imgRotate.jpg");
    //delete pix;
    outputFileName = ("../data/temp/imgRotate.jpg");
    emit returnOutputFileName(outputFileName);
}

void geometryTransform::resetControlPanel()
{

    ui->Xslider_scale->setValue(0);
    ui->Yslider_scale->setValue(0);
    ui->Xslider_translate->setValue(0);
    ui->Yslider_translate->setValue(0);
    ui->dial_rotate->setValue(0);

    ui->x_scale_current->setValue(1);
    ui->y_scale_current->setValue(1);
    ui->x_translate_current->setValue(0);
    ui->y_translate_current->setValue(0);
    ui->dialNum->setValue(0);
    xFactor = 1;
    yFactor = 1;
    rotateFactor = 0;
}

void geometryTransform::on_resetButton_clicked()
{
    resetControlPanel();
    emit reset();
}

geometryTransform::~geometryTransform()
{
    delete ui;
}