#include "dicomProcess.h"
#include "itkImageFileReader.h"//ITK
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
//#include <QSqlQuery>//QT
#include <QString>
#include <QMessageBox>
//#include <QSqlTableModel>
#include <QTableView>
#include <QDialog>
#include <QTextCodec>
#include <string>
#include <string.h>//strlen
#include <iostream>
using namespace std;

dicomProcess::dicomProcess(QWidget *parent , Qt::WindowFlags flags)
    :QWidget(parent, flags),
    ui(new Ui::dicomProcess)
{
    ui->setupUi(this);
    //dbSQL= QSqlDatabase::addDatabase("QMYSQL");
    ////dbSQL.setHostName("localhost");
    //dbSQL.setHostName("192.168.1.112");
    //dbSQL.setPort(3306);
    //dbSQL.setDatabaseName("imageprocess");
    //dbSQL.setUserName("root");
    //dbSQL.setPassword("mipav");
    //isConnected = dbSQL.open();
    if(!isConnected)
    {
        QMessageBox::warning(this, tr("提示信息对话框"), tr("连接数据库失败，请重新连接!"), QMessageBox::Ok);
        return;
    }
}

void dicomProcess::showImgProperty(vtkDICOMImageReader* reader)
{
    int* imgSize;
    double* origin;
    double* spacing;
    imgSize = reader->GetDataExtent();
    origin = reader->GetDataOrigin();
    spacing = reader->GetDataSpacing();
    ui->imgLengthLabel->setText(QString::number(imgSize[1] + 1, 10));//int ->QString 
    ui->imgWidthLabel->setText(QString::number(imgSize[3] + 1, 10)); 
    ui->imgSliceLabel->setText(QString::number(imgSize[5] + 1, 10)); 
    ui->pixelSpacingLabel->setText(QString::number(spacing[0]) + (" ") 
        + QString::number(spacing[1]) + (" ") + QString::number(spacing[2]));
    ui->imgOriginLabel->setText(QString::number(origin[0]) + (" ") 
        + QString::number(origin[1]) + (" ") + QString::number(origin[2]));

    if(reader->GetDataScalarType() == VTK_UNSIGNED_CHAR)
    {
        ui->imgDepthLabel->setText(QString::number(8, 10));
    }
    else if(reader->GetDataScalarType() == VTK_UNSIGNED_SHORT || reader->GetDataScalarType() == VTK_SHORT)
    {
        ui->imgDepthLabel->setText(QString::number(16, 10));
    }

    typedef unsigned short    PixelType;//signed
    const unsigned int      Dimension = 3;
    typedef itk::Image< PixelType, Dimension >         ImageType;
    typedef itk::ImageSeriesReader< ImageType >        ReaderType;
    ReaderType::Pointer itkReader = ReaderType::New();
    typedef itk::GDCMImageIO       ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    itkReader->SetImageIO( dicomIO );
    typedef itk::GDCMSeriesFileNames NamesGeneratorType;
    NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
    nameGenerator->SetUseSeriesDetails( true );
    nameGenerator->AddSeriesRestriction("0008|0021" );
    nameGenerator->SetDirectory(inputFileName);

    typedef std::vector< std::string >    SeriesIdContainer;
    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
    std::string seriesIdentifier;
    seriesIdentifier = seriesUID.begin()->c_str();
    typedef std::vector< std::string >   FileNamesContainer;
    FileNamesContainer fileNames;
    fileNames = nameGenerator->GetFileNames( seriesIdentifier );
    itkReader->SetFileNames( fileNames );
    itkReader->Update();
    dicomIO->GetMetaDataDictionary();

    nameInfo =   new char[50];//new char(15);
    ID = new char[50];
    time = new char[50];
    manufacture = new char[50];
    //char* windowCenter = new char(15);//No API
    //char* windowWidth = new char(15);
    modility = new char[15];
    hospital = new char[50];
    int pixelType =  dicomIO->GetPixelType();
    int componetType = dicomIO->GetComponentType();//SHORT, UNSIGNED SHORT
    int fileType = dicomIO->GetFileType();
    dicomIO->GetPatientName(nameInfo);
    dicomIO->GetModality(modility);
    dicomIO->GetPatientID(ID);
    dicomIO->GetManufacturer(manufacture);
    dicomIO->GetStudyDate(time);
    dicomIO->GetInstitution(hospital);
    ImageIOType::ByteOrder byteOrder;
    byteOrder = dicomIO->GetByteOrder();
    ImageIOType::TCompressionType compressType;
    compressType = dicomIO->GetCompressionType();

    QString patientName, patientSex, patientAge;
    for(size_t i = 0; i < strlen(nameInfo); i++)
    {
        if(nameInfo[i] == 'M') 
        {
            patientSex = ("M");
            std::string patientInfo = nameInfo;
            patientInfo = patientInfo.substr(0, i - 1);
            char temp[15];
            strcpy(temp, patientInfo.c_str());
            patientName = QString(QLatin1String(temp));
            //patientName = QString::fromStdString(patientInfo);//
        } 
        if(nameInfo[i] == 'F') 
        {
            patientSex = ("F");
            string patientInfo = nameInfo;
            patientInfo = patientInfo.substr(0, i - 1);
            char temp[15];
            strcpy(temp, patientInfo.c_str());//string to char*
            patientName = QString(QLatin1String(temp));//char* to QString
        } 
        if(nameInfo[i] == 'Y') patientAge = QString(nameInfo[i - 2]) + QString(nameInfo[i - 1]);//char to QString
    }

    ui->manufactureLabel->setText(QString(QLatin1String(manufacture)));
    ui->hospitalLabel->setText(QString(QLatin1String(hospital)));

    if(pixelType == 1)
        ui->pixelTypeLabel->setText("SCALAR");//IOPixelType
    else if(pixelType == 2)
        ui->pixelTypeLabel->setText("RGB");
    else if(pixelType == 3)
        ui->pixelTypeLabel->setText("RGBA");

    if(fileType == 0)
        ui->fileTypeLabel->setText("ASCII");//从itkimageiobase.h获取
    else if(fileType == 1) 
        ui->fileTypeLabel->setText("Binary");

    if(byteOrder == ImageIOType::ByteOrder::LittleEndian)
        ui->byteOrderLabel->setText("LittleEndian");
    else  if(byteOrder == ImageIOType::ByteOrder::BigEndian)
        ui->byteOrderLabel->setText("BigEndian");

    if(compressType == ImageIOType::TCompressionType::JPEG2000)
        ui->compressTypeLabel->setText("JPEG2000");
    else if(compressType == ImageIOType::TCompressionType::JPEG)
        ui->compressTypeLabel->setText("JPEG");
    else if(compressType == ImageIOType::TCompressionType::JPEG) 
        ui->compressTypeLabel->setText("JPEGLS");

    ui->nameLineEdit->setText(patientName); 
    ui->sexLineEdit->setText(patientSex);
    ui->ageLineEdit->setText(patientAge);
    ui->IDLineEdit->setText(QString(QLatin1String(ID)));//char* to QString
    ui->modalityLineEdit->setText(QString(QLatin1String(modility)));
    ui->timeLineEdit->setText(QString(QLatin1String(time)));

    delete[] nameInfo;
    delete[] ID;
    delete[] time;
    delete[] manufacture;
    delete[] modility;
    delete[] hospital;
}

void dicomProcess::on_addButtonSQL_clicked()
{

}

void dicomProcess::on_deleteButtonSQL_clicked()
{

}

void dicomProcess::on_modifyButtonSQL_clicked()
{

}

void dicomProcess::on_queryButtonSQL_clicked()
{

}

void dicomProcess::on_clearButtonSQL_clicked()
{
    ui->IDLineEdit->setText("");
    ui->nameLineEdit->setText("");
    ui->sexLineEdit->setText("");
    ui->ageLineEdit->setText("");
    ui->modalityLineEdit->setText("");
    ui->timeLineEdit->setText("");
}

void dicomProcess::on_showAllTableButtonSQL_clicked()
{

}

dicomProcess::~dicomProcess()
{

}