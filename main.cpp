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

using namespace std;
#define W3 setw(5) << fixed << right << setprecision(3)

void GetAllPathsFromWorkspace(string workspacePath, vector< vector< string > >& allPaths);
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

	GetAllPathsFromWorkspace(workspacePath, allPaths);

	std::vector<std::vector<std::string>>::iterator it;
	int i = 0;  // counter
	std::cout << std::setw(3);

	for (it = allPaths.begin(); it < allPaths.end(); it++, i++) {

		cout << allPaths[i][0] << endl;
		ReaderType::Pointer reader = ReaderType::New();
		reader->SetFileName(allPaths[i][0].c_str());
		reader->Update();
		int labelCount[5] = {};

		itk::ImageSliceIteratorWithIndex<ImageType> it(reader->GetOutput(), reader->GetOutput()->GetLargestPossibleRegion());
		int zSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize()[2];

		cout << "Size z: " << zSize << endl;
		vector<int[5] > zSlices(zSize);
		vector<float[5] > zSlicesNorm(zSize);

		it.SetFirstDirection(0);
		it.SetSecondDirection(1);

		it.GoToBegin();
		int zCnt = 0;

		std::vector<int[2]> minsmax(5);

		while (!it.IsAtEnd()) {
			minsmax[zCnt][0] = std::numeric_limits<int>::max();
			minsmax[zCnt][1] = std::numeric_limits<int>::min();

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
			cout << y << " : " << minsmax[y][0] << "," << minsmax[y][1] << endl;
		}

		int sumLabels = labelCount[1] + labelCount[2] + labelCount[3] + labelCount[4];
		float onePercent = sumLabels / 100;

		cout << "1: " << labelCount[1] / onePercent << " 2: " << labelCount[2] / onePercent << " 3: " << labelCount[3] / onePercent << " 4: " << labelCount[4] / onePercent;
		cout << " Sum of Labels: " << sumLabels << endl;

		float labelPercen[4] = { labelCount[1] / onePercent ,labelCount[2] / onePercent ,labelCount[3] / onePercent ,labelCount[4] / onePercent };

		float minDistance = std::numeric_limits<float>::max();
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
			if(!isNotValid){
				cout << "mean for slice " << k << " " << boost::accumulators::mean(acc) << endl;
				cout << sqrt(boost::accumulators::variance(acc)) << endl;
				//if (distance < minDistance) {
					//minDistance = distance;
					//cout << W3 << labelPercenSlice[0] << W3 << labelPercenSlice[1] << W3 << labelPercenSlice[2] << W3 << labelPercenSlice[3] <<
					cout << "[" << W3 << zSlices[k][1] << "," << W3 << zSlices[k][2] << "," << W3 << zSlices[k][3] << "," << W3 << zSlices[k][4] << "]" << endl;// "],\t|\t" << endl;
					cout << "[" << W3 << zSlicesNorm[k][1] << "," << W3 << zSlicesNorm[k][2] << "," << W3 << zSlicesNorm[k][3] << "," << W3 << zSlicesNorm[k][4] << "]" << endl; // "],\t|\t" << endl;
						//<< "Distance: " << W3 << distance << endl;
				//}
			}
		}

		pressEnter();

		for (int j = 0; j < 5; j++) {
		}
		cout << endl;
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