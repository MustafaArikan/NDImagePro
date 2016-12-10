#include "mainwindow.h"
#include "mainwindowInterator.h"
#include <QtGui>
#include <QApplication>
#include <QTextCodec>
#include <QString>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <itkImageFileReader.h>
#include <itkImageSliceIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageRegionIterator.h>
#include <itkConnectedComponentImageFilter.h>
#include "QuickView.h"
#include "itkImageFileWriter.h"
#include "itkPNGImageIOFactory.h"

template< unsigned int VDimension >
int ReadScalarImage(const char* inputFileName, const itk::ImageIOBase::IOComponentType componentType);

itk::Image< unsigned char, 2>::Pointer EliminateNonRegions(int voxelTh, int regionCount, itk::Image<unsigned char, 2>::Pointer labelImage);

using namespace boost::filesystem;
struct recursive_directory_range
{
	typedef recursive_directory_iterator iterator;
	recursive_directory_range(path p) : p_(p) {}

	iterator begin() { return recursive_directory_iterator(p_); }
	iterator end() { return recursive_directory_iterator(); }

	path p_;
};

void pressEnter() {
	cout << "Press Enter to Continue";
	cin.ignore();
}


int main(int argc, char *argv[])
{
	cout << "Start of the test program:" << endl;

	int imageNr = 21;
	for (auto it : recursive_directory_range("D:\\workspace\\Projects\\RegProject\\GA_Morpho_FAF_update"))
	{
		std::stringstream Mystr;
		Mystr << it;

		

		if (boost::ends_with(Mystr.str(), ".jpg.manAnnot.mhd\"")) {
			cout << Mystr.str() << endl;
			std::string unquoteMystr = Mystr.str();

			pressEnter();

			boost::replace_all(unquoteMystr, "\"", "");

			typedef itk::ImageIOBase::IOComponentType ScalarPixelType;

			itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(unquoteMystr.c_str(), itk::ImageIOFactory::ReadMode);
			if (!imageIO){
				std::cerr << "Could not CreateImageIO for: " << Mystr.str() << std::endl;
				return EXIT_FAILURE;
			}
			imageIO->SetFileName(unquoteMystr.c_str());
			imageIO->ReadImageInformation();

			const ScalarPixelType pixelType = imageIO->GetComponentType();
			std::cout << "Pixel Type is " << imageIO->GetComponentTypeAsString(pixelType) // 'double'
				<< std::endl;
			const size_t numDimensions = imageIO->GetNumberOfDimensions();
			std::cout << "numDimensions: " << numDimensions << std::endl; // '2'
			std::cout << "component size: " << imageIO->GetComponentSize() << std::endl; // '8'
			std::cout << "pixel type (string): " << imageIO->GetPixelTypeAsString(imageIO->GetPixelType()) << std::endl; // 'vector'
			std::cout << "pixel type: " << imageIO->GetPixelType() << std::endl; // '5'

			typedef itk::Image< float, 2 >         ImageType;
			typedef itk::ImageFileReader<ImageType> ReaderType;

			ReaderType::Pointer reader = ReaderType::New();
			reader->SetFileName(unquoteMystr.c_str());
			reader->Update();

			std::string pngFileName(unquoteMystr);
			std::string pngOrFileName(unquoteMystr);

			boost::replace_all(unquoteMystr, ".jpg.manAnnot.mhd", ".jpg.mhd");

			boost::replace_all(pngFileName, ".jpg.manAnnot.mhd", ".jpg.manAnnot.mhd.png");
			boost::replace_all(pngOrFileName, ".jpg.manAnnot.mhd", ".jpg.mhd.png");

			ReaderType::Pointer reader2 = ReaderType::New();
			reader2->SetFileName(unquoteMystr.c_str());
			reader2->Update();

			typedef itk::Image< unsigned char, 2 >	IntImageType2D;
			IntImageType2D::RegionType region;
			IntImageType2D::IndexType start;
			start[0] = 0;
			start[1] = 0;

			IntImageType2D::SizeType size;
			size[0] = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
			size[1] = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];

			region.SetSize(size);
			region.SetIndex(start);

			IntImageType2D::Pointer image2d = IntImageType2D::New();
			image2d->SetRegions(region);
			image2d->Allocate();
			image2d->FillBuffer(0);

			IntImageType2D::Pointer image2dOr = IntImageType2D::New();
			image2dOr->SetRegions(region);
			image2dOr->Allocate();
			image2dOr->FillBuffer(0);

			IntImageType2D::Pointer emptyImage = IntImageType2D::New();
			emptyImage->SetRegions(region);
			emptyImage->Allocate();
			emptyImage->FillBuffer(255);

			itk::ImageSliceIteratorWithIndex<IntImageType2D> it2(image2d, image2d->GetLargestPossibleRegion());
			itk::ImageSliceIteratorWithIndex<IntImageType2D> it2_or(image2dOr, image2dOr->GetLargestPossibleRegion());
			itk::ImageSliceIteratorWithIndex<ImageType> it(reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion());
			itk::ImageSliceIteratorWithIndex<ImageType> it_or(reader2->GetOutput(), reader2->GetOutput()->GetLargestPossibleRegion());

			it.SetFirstDirection(0);
			it.SetSecondDirection(1);

			it_or.SetFirstDirection(0);
			it_or.SetSecondDirection(1);

			it2.SetFirstDirection(0);
			it2.SetSecondDirection(1);

			it2_or.SetFirstDirection(0);
			it2_or.SetSecondDirection(1);

			it.GoToBegin();
			it_or.GoToBegin();
			it2.GoToBegin();
			it2_or.GoToBegin();

			while (!it.IsAtEnd())
			{
				while (!it.IsAtEndOfSlice())
				{
					while (!it.IsAtEndOfLine())
					{
						ImageType::ValueType value = it.Get();  // it.Set() doesn't exist in the Const Iterator
						ImageType::ValueType value_or = it_or.Get();  // it.Set() doesn't exist in the Const Iterator
						if (value == 255 && value_or-value != 0) {
							it2.Set(value);
						}
						it2_or.Set(value_or);

						++it;
						++it_or;
						++it2;
						++it2_or;
					}
					it.NextLine();
					it_or.NextLine();
					it2.NextLine();
					it2_or.NextLine();

				}
				it.NextSlice();
				it_or.NextSlice();
				it2.NextSlice();
				it2_or.NextSlice();
			}

			typedef itk::ConnectedComponentImageFilter <IntImageType2D, IntImageType2D> ConnectedComponentImageFilterType;
			ConnectedComponentImageFilterType::Pointer labelFilter = ConnectedComponentImageFilterType::New();
			labelFilter->FullyConnectedOn();
			labelFilter->SetInput(image2d);
			labelFilter->Update();

			cout << "Coutn: " << labelFilter->GetObjectCount() << endl;
			//labelFilter->ReleaseDataFlagOn();
			IntImageType2D::Pointer newImage2d;
			newImage2d = EliminateNonRegions(3, labelFilter->GetObjectCount(), labelFilter->GetOutput());

			QuickView viewer;
			viewer.AddImage(newImage2d.GetPointer(), false, "2D Image");
			viewer.Visualize();

			boost::filesystem::path p(unquoteMystr.c_str());
			boost::filesystem::path dir = p.parent_path();
			

			std::string imageFileName = dir.string() + "\\" + std::to_string(imageNr) + "_training.tif";
			std::string vesselsFilename = dir.string() + "\\" + std::to_string(imageNr) + "_manual1.png";
			std::string maskFilename = dir.string() + "\\" + std::to_string(imageNr) + "_training_mask.png";

			cout << imageFileName << endl;

			typedef  itk::ImageFileWriter< IntImageType2D > WriterType;
			WriterType::Pointer writer = WriterType::New();
			cout << "Filename: " << pngFileName << endl;
			itk::PNGImageIOFactory::RegisterOneFactory();
			writer->SetFileName(vesselsFilename);
			writer->SetInput(newImage2d);
			writer->Update();

			writer->SetFileName(imageFileName);
			writer->SetInput(image2dOr);
			writer->Update();

			writer->SetFileName(maskFilename);
			writer->SetInput(emptyImage);
			writer->Update();

			imageNr++;

			//switch (pixelType)
			//{
			//case itk::ImageIOBase::SCALAR:
			//{
			//	if (numDimensions == 2)
			//	{
			//		return ReadScalarImage< 2 >(unquoteMystr.c_str(), componentType);
			//	}
			//	else if (numDimensions == 3)
			//	{
			//		return ReadScalarImage< 3 >(unquoteMystr.c_str(), componentType);
			//	}
			//	else if (numDimensions == 4)
			//	{
			//		return ReadScalarImage< 4 >(unquoteMystr.c_str(), componentType);
			//	}
			//}

			//const unsigned int Dimension = 2;

			//typedef unsigned char                      PixelType;
			//typedef itk::Image< PixelType, Dimension > ImageType;
			//typedef itk::ImageFileReader< ImageType >  ReaderType;
			//ReaderType::Pointer reader = ReaderType::New();
			//reader->SetFileName(Mystr.str());
			//reader->Update();
			//cout << "Dimension: " << reader->GetOutput()->GetImageDimension() << endl;
			//cout << "Dimension: " << reader->GetOutput()->Get << endl;
		}
	}

    //QApplication a(argc, argv);

    //mainwindowInterator w;
    //w.setWindowTitle(QObject::tr("ImageProcesser"));
    //w.setWindowIcon(QIcon("../data/logo/QT.ico"));
    //w.show();
    //return a.exec();
	std::string newString; cin >> newString;
}


template< unsigned int VDimension >
int ReadScalarImage(const char* inputFileName,
	const itk::ImageIOBase::IOComponentType componentType)
{
	switch (componentType)
	{
	default:
	case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
		std::cerr << "Unknown and unsupported component type!" << std::endl;
		return EXIT_FAILURE;

	case itk::ImageIOBase::UCHAR:
	{
		typedef unsigned char PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}

	case itk::ImageIOBase::CHAR:
	{
		typedef char PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}

	case itk::ImageIOBase::USHORT:
	{
		typedef unsigned short PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}

	case itk::ImageIOBase::SHORT:
	{
		typedef short PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}

	case itk::ImageIOBase::UINT:
	{
		typedef unsigned int PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}

	case itk::ImageIOBase::INT:
	{
		typedef int PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}

	case itk::ImageIOBase::ULONG:
	{
		typedef unsigned long PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}

	case itk::ImageIOBase::LONG:
	{
		typedef long PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}

	case itk::ImageIOBase::FLOAT:
	{
		typedef float PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}

	case itk::ImageIOBase::DOUBLE:
	{
		typedef double PixelType;
		typedef itk::Image< PixelType, VDimension > ImageType;

		typename ImageType::Pointer image = ImageType::New();

		if (ReadImage< ImageType >(inputFileName, image) == EXIT_FAILURE)
		{
			return EXIT_FAILURE;
		}

		std::cout << image << std::endl;
		break;
	}
	}
	return EXIT_SUCCESS;
}

itk::Image< unsigned char, 2>::Pointer EliminateNonRegions(int voxelTh, int regionCount, itk::Image<unsigned char, 2>::Pointer labelImage) {
	using namespace std;

	typedef itk::Image< unsigned char, 2>	IntImageType2D;	// Int image type
	typedef itk::ImageRegionIteratorWithIndex<IntImageType2D> ImageIteratorType;
	typedef itk::ImageRegionIterator<IntImageType2D> ImageIteratorTypeFirst;

	IntImageType2D::Pointer newImage;
	newImage = IntImageType2D::New();
	newImage->SetRegions(labelImage->GetLargestPossibleRegion());
	newImage->SetSpacing(labelImage->GetSpacing());
	newImage->Allocate();
	newImage->FillBuffer(0);

	ImageIteratorType iterNewImage(newImage, newImage->GetLargestPossibleRegion());
	ImageIteratorTypeFirst iterfirstImage(labelImage, labelImage->GetLargestPossibleRegion());

	int howmany = voxelTh;
	int count = 0;

	vector<int> voxelcount(regionCount + 1);
	std::fill(voxelcount.begin(), voxelcount.end(), 0);
	cout << "Count: " << regionCount << endl;

	iterfirstImage.GoToBegin();
	while (!iterfirstImage.IsAtEnd())
	{
		int value = iterfirstImage.Get();
		if (value >= 0 && value <= regionCount) {
			voxelcount[value]++;
		}
		++iterfirstImage;
	}//while

	for (int i = 0; i <= regionCount; i++)
	{
		//cout << i << ":" << voxelcount[i] << ",";
		if (voxelcount[i] > howmany) {
			count++;
		}
		else {
			voxelcount[i] = 0;
		}
	}

	iterfirstImage.GoToBegin();
	iterNewImage.GoToBegin();

	while (!iterfirstImage.IsAtEnd())
	{
		int value = iterfirstImage.Get();
		if (voxelcount[value] != 0)
		{
			IntImageType2D::IndexType index = iterfirstImage.GetIndex();
			iterNewImage.SetIndex(index);
			//iterNewImage.Set(value);
			if (value != 0)
				iterNewImage.Set(255);
		}
		++iterfirstImage;
	}//while

	cout << "New count: " << count << endl;

	return newImage;
}