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
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <itkImageFileReader.h>
#include <itkImageSliceIteratorWithIndex.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImageRegionIterator.h>
#include <itkConnectedComponentImageFilter.h>
#include "QuickView.h"
#include "itkImageFileWriter.h"
#include "itkPNGImageIOFactory.h"
#include "itkCastImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"

using namespace std;
#define W3 setw(5) << fixed << right << setprecision(3)

void GetAllPathsFromWorkspace(string workspacePath, vector< vector< string > >& allPaths);
void ExtractSliceAsImage(int repSlice, std::string pathToMhd, std::string pathToOutput, int, int);
void ExtractSliceAsMask(int repSlice, std::string pathToMhd, std::string pathToOutput);
void ExtractSliceAsAnnotatedImage(int repSlice, std::string pathToMhd, std::string pathToOutput, int, int);

template<class Iter_T, class Iter2_T>
double vectorDistance(Iter_T first, Iter_T last, Iter2_T first2);

template< unsigned int VDimension >
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
	typedef itk::Image< double, 3 >         ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;

	cout << "Start of the test program:" << endl;

	std::vector<std::vector<std::string>> allPaths; 
	std::string workspacePath = "F:\\workspace_f\\Workspace_BRATS\\brats\\BRATS2015_Training\\workspace\\extractForCNN";
	std::string workspacePathTrain = "F:\\workspace_f\\Workspace_BRATS\\brats\\BRATS2015_Training\\workspace\\extractForCNN\\training";

	GetAllPathsFromWorkspace(workspacePath, allPaths);

	std::vector<std::vector<std::string>>::iterator it;
	int i = 0;  // counter
	std::cout << std::setw(3);
	int testImageNr = 21;

	int maxX = std::numeric_limits<int>::min();
	int maxY = std::numeric_limits<int>::min();

	for (it = allPaths.begin(); it < allPaths.end(); it++, i++) {
		cout << allPaths[i][0].c_str() << endl;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName(allPaths[i][0].c_str());
		reader->Update();
		int x = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
		int y = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];

		cout << "dim: " << x << "," << y << endl;

		if (x > maxX) {
			maxX = x;
		}

		if (y > maxY) {
			maxY = y;
		}
	}

	i = 0;

	maxX = 126;
	maxY = 166;

	cout << "Max X is : " << maxX << endl;
	cout << "Max Y is : " << maxY << endl;
	pressEnter();

	for (it = allPaths.begin(); it < allPaths.end(); it++, i++) {

		//cout << allPaths[i][0] << endl;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName(allPaths[i][0].c_str());
		reader->Update();
		int labelCount[5] = {};

		itk::ImageSliceIteratorWithIndex<ImageType> it(reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion());
		int zSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];

		//cout << "Size z: " << zSize << endl;
		vector<int[5] > zSlices(zSize);
		vector<float[5] > zSlicesNorm(zSize);

		it.SetFirstDirection(0);
		it.SetSecondDirection(1);

		it.GoToBegin();
		int zCnt = 0;

		std::vector<int[2]> minsmax(5);
		for (int y = 0; y < 5; y++) {
			minsmax[y][0] = std::numeric_limits<int>::max();
			minsmax[y][1] = std::numeric_limits<int>::min();
		}

		while (!it.IsAtEnd()) {
			while (!it.IsAtEndOfSlice()) {
				while (!it.IsAtEndOfLine()) {
					ImageType::ValueType value = it.Get();
					labelCount[int(value)]++;
					zSlices[zCnt][int(value)]++;
					++it;
				}
				it.NextLine();
			}
			it.NextSlice();

			for (int y = 0; y < 5; y++) {
				if (zSlices[zCnt][1] != 0 && zSlices[zCnt][2] != 0 && zSlices[zCnt][3] != 0 && zSlices[zCnt][4] != 0) {
					if (zSlices[zCnt][y] < minsmax[y][0]) { minsmax[y][0] = zSlices[zCnt][y]; }
					if (zSlices[zCnt][y] > minsmax[y][1]) { minsmax[y][1] = zSlices[zCnt][y]; }
				}
			}

			zCnt++;
		}

		float ranges[5];
		for (int y = 0; y < 5; y++) {
			ranges[y] = minsmax[y][1] - minsmax[y][0];
			//cout << y << " : " << minsmax[y][0] << "," << minsmax[y][1] << endl;
		}

		int sumLabels = labelCount[1] + labelCount[2] + labelCount[3] + labelCount[4];
		float onePercent = sumLabels / 100;

		//cout << "1: " << labelCount[1] / onePercent << " 2: " << labelCount[2] / onePercent << " 3: " << labelCount[3] / onePercent << " 4: " << labelCount[4] / onePercent;
		//cout << " Sum of Labels: " << sumLabels << endl;

		float labelPercen[4] = { labelCount[1] / onePercent ,labelCount[2] / onePercent ,labelCount[3] / onePercent ,labelCount[4] / onePercent };

		float minDistance = std::numeric_limits<float>::max();
		float maxSumNorm = std::numeric_limits<int>::min();
		int repSlice = -1;
		for (int k = 0; k < zSize; k++) {
			bool isNotValid = false;

			for (int y = 0; y < 5; y++) {
				zSlicesNorm[k][y] = (zSlices[k][y] - minsmax[y][0]) / ranges[y];
			}

			float sumLabelsSlice = zSlicesNorm[k][1] + zSlicesNorm[k][2] + zSlicesNorm[k][3] + zSlicesNorm[k][4];
			float onePercentSlice = sumLabelsSlice / 100;
			float labelPercenSlice[4] = { zSlicesNorm[k][1] / onePercentSlice ,zSlicesNorm[k][2] / onePercentSlice ,zSlicesNorm[k][3] / onePercentSlice ,zSlicesNorm[k][4] / onePercentSlice };

			if (isnan(labelPercenSlice[0]) || isnan(labelPercenSlice[1]) || isnan(labelPercenSlice[2]) || isnan(labelPercenSlice[3])) {
				isNotValid = true;
			}

			if (isinf(labelPercenSlice[0]) || isinf(labelPercenSlice[1]) || isinf(labelPercenSlice[2]) || isinf(labelPercenSlice[3])) {
				isNotValid = true;
			}

			boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::variance> > acc;

			for (int l = 0; l < 4; l++) {
				if (labelPercenSlice[l] < 0.0) { isNotValid = true; }
			}

			for (int l = 1; l < 5; l++) {
				if (zSlicesNorm[k][l] < 0.0) { isNotValid = true; }
				acc(zSlicesNorm[k][l]);
			}

			//float distance = vectorDistance(labelPercen, labelPercen + 4, labelPercenSlice);
			
			//if (distance < 15 && !isNotValid) {
			float sumNorm = zSlicesNorm[k][1] + zSlicesNorm[k][2] + zSlicesNorm[k][3] + zSlicesNorm[k][4];
			float mean = boost::accumulators::mean(acc);

			if(!isNotValid){
				if (sumNorm > maxSumNorm) {
					maxSumNorm = sumNorm;
					//cout << "mean for slice " << k << " " << boost::accumulators::mean(acc) << ",";
					//cout << " sqrt:" << sqrt(boost::accumulators::variance(acc)) << ",";
					//cout << " norm sum: " << sumNorm << endl;
					////if (distance < minDistance) {
					//	//minDistance = distance;
					//	//cout << W3 << labelPercenSlice[0] << W3 << labelPercenSlice[1] << W3 << labelPercenSlice[2] << W3 << labelPercenSlice[3] <<
					//cout << "[" << W3 << zSlices[k][1] << "," << W3 << zSlices[k][2] << "," << W3 << zSlices[k][3] << "," << W3 << zSlices[k][4] << "]" << endl;// "],\t|\t" << endl;
					//cout << "[" << W3 << zSlicesNorm[k][1] << "," << W3 << zSlicesNorm[k][2] << "," << W3 << zSlicesNorm[k][3] << "," << W3 << zSlicesNorm[k][4] << "]" << endl; // "],\t|\t" << endl;
						//<< "Distance: " << W3 << distance << endl;
				//}
					repSlice = k;
				}
			}
		}
		
		if (repSlice != -1) {
			boost::filesystem::path filenameDataset(allPaths[i][0]);
			if (zSlicesNorm[repSlice][1] != 0 && zSlicesNorm[repSlice][2] != 0 && zSlicesNorm[repSlice][3] != 0 && zSlicesNorm[repSlice][4] != 0) {
				//cout << "Slice " << W3 << repSlice << W3 << filenameDataset.filename() << endl;
				//cout << "[" << W3 << zSlices[repSlice][1] << "," << W3 << zSlices[repSlice][2] << "," << W3 << zSlices[repSlice][3] << "," << W3 << zSlices[repSlice][4] << "]" << endl;// "],\t|\t" << endl;
				//cout << "[" << W3 << zSlicesNorm[repSlice][1] << "," << W3 << zSlicesNorm[repSlice][2] << "," << W3 << zSlicesNorm[repSlice][3] << "," << W3 << zSlicesNorm[repSlice][4] << "]" << endl; // "],\t|\t" << endl;

				for (int j = 0; j < 5; j++) {
					if (j == 0) {
						ExtractSliceAsAnnotatedImage(repSlice, allPaths[i][j], workspacePathTrain + "\\" + std::to_string(testImageNr) + "_manual1.png", maxX, maxY);
						cout << "/SegNet/CamVid/train/" + std::to_string(testImageNr) + "_training.png ";
							

					}
					else if (j == 4) {
						//cout << "Extract image" << endl;
						ExtractSliceAsImage(repSlice, allPaths[i][j], workspacePathTrain + "\\" + std::to_string(testImageNr) + "_training.png", maxX, maxY);
						//ExtractSliceAsMask(repSlice, allPaths[i][j], workspacePathTrain + "\\" + std::to_string(testImageNr) + "_training_mask.png");
						cout << "/SegNet/CamVid/trainannot/" + std::to_string(testImageNr) + "_manual1.png" << endl;
					}
				}
				testImageNr++;
			}
		}

		pressEnter();
		//cout << endl;
	}



	//std::string pngFileName(unquoteMystr);
	//std::string pngOrFileName(unquoteMystr);

	//boost::replace_all(pngFileName, ".mha", ".mha.png");
	//boost::replace_all(pngOrFileName, ".mha", ".mha.orig.png");

    //QApplication a(argc, argv);

    //mainwindowInterator w;
    //w.setWindowTitle(QObject::tr("ImageProcesser"));
    //w.setWindowIcon(QIcon("../data/logo/QT.ico"));
    //w.show();
    //return a.exec();
	pressEnter();
}

void GetAllPathsFromWorkspace(string workspacePath, vector< vector< string > >& allPaths) {
	int imageNr = 0;
	std::vector<std::string> fileList(5);
	for (auto it : recursive_directory_range(workspacePath))
	{
		std::stringstream Mystr;
		Mystr << it;

		if (boost::ends_with(Mystr.str(), ".mha\"")) {
			if (imageNr == 5) {
				imageNr = 0;
				allPaths.push_back(fileList);
			}

			std::string unquoteMystr = Mystr.str();

			boost::replace_all(unquoteMystr, "\"", "");

			if (boost::algorithm::contains(unquoteMystr, ".OT.")) {
				fileList[0] = unquoteMystr;
			}
			else if (boost::algorithm::contains(unquoteMystr, "_Flair")) {
				fileList[1] = unquoteMystr;
			}
			else if (boost::algorithm::contains(unquoteMystr, "_T1c")) {
				fileList[2] = unquoteMystr;
			}
			else if (boost::algorithm::contains(unquoteMystr, "_T1")) {
				fileList[3] = unquoteMystr;
			}
			else if (boost::algorithm::contains(unquoteMystr, "_T2")) {
				fileList[4] = unquoteMystr;
			}
			imageNr++;
		}
	}
}

template<class Iter_T, class Iter2_T>
double vectorDistance(Iter_T first, Iter_T last, Iter2_T first2) {
	double ret = 0.0;
	while (first != last) {
		double dist = (*first++) - (*first2++);
		ret += dist * dist;
	}
	return ret > 0.0 ? sqrt(ret) : 0.0;
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

void ExtractSliceAsImage(int repSlice, std::string pathToMhd, std::string pathToOutput, int maxX, int maxY) {
	typedef itk::Image< double, 3 >         ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;

	typedef itk::Image< unsigned char, 3 >  OutputImageType;

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(pathToMhd.c_str());
	reader->Update();

	typedef itk::RescaleIntensityImageFilter< ImageType, ImageType > RescaleType;
	RescaleType::Pointer rescale = RescaleType::New();
	rescale->SetInput(reader->GetOutput());
	rescale->SetOutputMinimum(0);
	rescale->SetOutputMaximum(itk::NumericTraits< unsigned char >::max());

	typedef itk::CastImageFilter< ImageType, OutputImageType > FilterType;
	FilterType::Pointer cast = FilterType::New();
	cast->SetInput(rescale->GetOutput());
	cast->Update();

	ImageType::SizeType upperExtendRegion;
	ImageType::SizeType lowerExtendRegion;

	int padX = maxX - reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
	int padY = maxY - reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];

	if (padX % 2 == 0) {
		padX = padX / 2;
		upperExtendRegion[0] = padX;
		lowerExtendRegion[0] = padX;
	}
	else{
		padX = padX / 2;
		upperExtendRegion[0] = padX;
		lowerExtendRegion[0] = padX + 1;
	}

	if (padY % 2 == 0) {
		padY = padY / 2;
		upperExtendRegion[1] = padY;
		lowerExtendRegion[1] = padY;
	}
	else {
		padY = padY / 2;
		upperExtendRegion[1] = padY;
		lowerExtendRegion[1] = padY + 1;
	}

	upperExtendRegion[2] = 0;
	lowerExtendRegion[2] = 0;

	ImageType::PixelType constantPixel = 0;

	typedef itk::ConstantPadImageFilter < OutputImageType, OutputImageType > PadFilterType;
	PadFilterType::Pointer filter = PadFilterType::New();
	filter->SetInput(cast->GetOutput());
	filter->SetPadUpperBound(upperExtendRegion);
	filter->SetPadLowerBound(lowerExtendRegion);
	filter->SetConstant(constantPixel);
	filter->Update();

	typedef itk::Image< unsigned char, 2 >	IntImageType2D;
	IntImageType2D::RegionType region;
	IntImageType2D::IndexType start;
	start[0] = 0;
	start[1] = 0;

	IntImageType2D::SizeType size;
	size[0] = filter->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
	size[1] = filter->GetOutput()->GetLargestPossibleRegion().GetSize()[1];

	region.SetSize(size);
	region.SetIndex(start);

	IntImageType2D::Pointer image2d = IntImageType2D::New();
	image2d->SetRegions(region);
	image2d->Allocate();
	image2d->FillBuffer(0);

	itk::ImageSliceIteratorWithIndex<OutputImageType> it(filter->GetOutput(), filter->GetOutput()->GetLargestPossibleRegion());
	itk::ImageSliceIteratorWithIndex<IntImageType2D> it2(image2d, image2d->GetLargestPossibleRegion());

	it.SetFirstDirection(0);
	it.SetSecondDirection(1);

	it2.SetFirstDirection(0);
	it2.SetSecondDirection(1);

	it.GoToBegin();
	it2.GoToBegin();

	//while (!it.IsAtEnd())
	//{
	for (int s = 0; s < repSlice; s++) {
		it.NextSlice();
	}
	while (!it.IsAtEndOfSlice())
	{
		while (!it.IsAtEndOfLine())
		{
			ImageType::ValueType value = it.Get();  // it.Set() doesn't exist in the Const Iterator
			it2.Set(value);
			++it;
			++it2;
		}
		it.NextLine();
		it2.NextLine();
	}
	//	it.NextSlice();
	//	it2.NextSlice();
	//}

	typedef itk::ImageFileWriter< IntImageType2D  > WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(pathToOutput);
	writer->SetInput(image2d);
	writer->Update();
}

void ExtractSliceAsMask(int repSlice, std::string pathToMhd, std::string pathToOutput) {
	typedef itk::Image< double, 3 >         ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(pathToMhd.c_str());
	reader->Update();

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

	itk::ImageSliceIteratorWithIndex<ImageType> it(reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion());
	itk::ImageSliceIteratorWithIndex<IntImageType2D> it2(image2d, image2d->GetLargestPossibleRegion());

	it.SetFirstDirection(0);
	it.SetSecondDirection(1);

	it2.SetFirstDirection(0);
	it2.SetSecondDirection(1);

	it.GoToBegin();
	it2.GoToBegin();

	//while (!it.IsAtEnd())
	//{
	for (int s = 0; s < repSlice; s++) {
		it.NextSlice();
	}
	while (!it.IsAtEndOfSlice())
	{
		while (!it.IsAtEndOfLine())
		{
			ImageType::ValueType value = it.Get();  // it.Set() doesn't exist in the Const Iterator
			it2.Set(255);
			++it;
			++it2;
		}
		it.NextLine();
		it2.NextLine();
	}
	//	it.NextSlice();
	//	it2.NextSlice();
	//}

	typedef itk::ImageFileWriter< IntImageType2D  > WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(pathToOutput);
	writer->SetInput(image2d);
	writer->Update();
}

void ExtractSliceAsAnnotatedImage(int repSlice, std::string pathToMhd, std::string pathToOutput, int maxX, int maxY) {
	typedef itk::Image< double, 3 >         ImageType;
	typedef itk::ImageFileReader<ImageType> ReaderType;

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(pathToMhd.c_str());
	reader->Update();

	ImageType::SizeType upperExtendRegion;
	ImageType::SizeType lowerExtendRegion;

	int padX = maxX - reader->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
	int padY = maxY - reader->GetOutput()->GetLargestPossibleRegion().GetSize()[1];

	if (padX % 2 == 0) {
		padX = padX / 2;
		upperExtendRegion[0] = padX;
		lowerExtendRegion[0] = padX;
	}
	else {
		padX = padX / 2;
		upperExtendRegion[0] = padX;
		lowerExtendRegion[0] = padX + 1;
	}

	if (padY % 2 == 0) {
		padY = padY / 2;
		upperExtendRegion[1] = padY;
		lowerExtendRegion[1] = padY;
	}
	else {
		padY = padY / 2;
		upperExtendRegion[1] = padY;
		lowerExtendRegion[1] = padY + 1;
	}

	upperExtendRegion[2] = 0;
	lowerExtendRegion[2] = 0;

	ImageType::PixelType constantPixel = 0;

	typedef itk::ConstantPadImageFilter < ImageType, ImageType > PadFilterType;
	PadFilterType::Pointer filter = PadFilterType::New();
	filter->SetInput(reader->GetOutput());
	filter->SetPadUpperBound(upperExtendRegion);
	filter->SetPadLowerBound(lowerExtendRegion);
	filter->SetConstant(constantPixel);
	filter->Update();

	typedef itk::Image< unsigned char, 2 >	IntImageType2D;
	IntImageType2D::RegionType region;
	IntImageType2D::IndexType start;
	start[0] = 0;
	start[1] = 0;

	IntImageType2D::SizeType size;
	size[0] = filter->GetOutput()->GetLargestPossibleRegion().GetSize()[0];
	size[1] = filter->GetOutput()->GetLargestPossibleRegion().GetSize()[1];

	region.SetSize(size);
	region.SetIndex(start);

	IntImageType2D::Pointer image2d = IntImageType2D::New();
	image2d->SetRegions(region);
	image2d->Allocate();
	image2d->FillBuffer(0);

	itk::ImageSliceIteratorWithIndex<ImageType> it(filter->GetOutput(), filter->GetOutput()->GetLargestPossibleRegion());
	itk::ImageSliceIteratorWithIndex<IntImageType2D> it2(image2d, image2d->GetLargestPossibleRegion());

	it.SetFirstDirection(0);
	it.SetSecondDirection(1);

	it2.SetFirstDirection(0);
	it2.SetSecondDirection(1);

	it.GoToBegin();
	it2.GoToBegin();

	//while (!it.IsAtEnd())
	//{
	for (int s = 0; s < repSlice; s++) {
		it.NextSlice();
	}
	while (!it.IsAtEndOfSlice())
	{
		while (!it.IsAtEndOfLine())
		{
			ImageType::ValueType value = it.Get();  // it.Set() doesn't exist in the Const Iterator
			if (value == 1 || value == 2 || value == 3 || value == 4) {
				it2.Set(value);
			}
			++it;
			++it2;
		}
		it.NextLine();
		it2.NextLine();
	}
	//	it.NextSlice();
	//	it2.NextSlice();
	//}

	typedef itk::ImageFileWriter< IntImageType2D  > WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(pathToOutput);
	writer->SetInput(image2d);
	writer->Update();
}