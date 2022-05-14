/**
 * CS4185/CS5185 Multimedia Technologies and Applications
 * Course Assignment
 * Image Retrieval Task
 //*/

#define CVUI_IMPLEMENTATION
#include "c:\\cvui-2.7.0\\cvui.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>
#include <stdio.h>
#include <algorithm>

using namespace std;
using namespace cv;

#define IMAGE_folder "C:\\dataset"     // change to your folder location
#define IMAGE_LIST_FILE "dataset1"         //the dataset1 for retrieval

#define WINDOW1_NAME "Image Retrieval"

/**
 * @function main
 */

double colorHist(Mat img, Mat football) {

	Mat hisImg;
	Mat hisFootball;
	int bin = 50;
	float range[] = { 0,256 };
	const float* hisrange = { range };
	calcHist(&img, 1, 0, Mat(), hisImg, 1, &bin, &hisrange, true, false);
	calcHist(&football, 1, 0, Mat(), hisFootball, 1, &bin, &hisrange, true, false);
	normalize(hisImg, hisImg, 1, 0, NORM_L2, -1, Mat());
	normalize(hisFootball, hisFootball, 1, 0, NORM_L2, -1, Mat());
	return compareHist(hisImg, hisFootball, 3);
}

double ORBmatch(Mat img, Mat football) {
	vector<KeyPoint> keypoints_fb, keypoints_img;
	Mat descriptors_fb, descriptors_img;
	OrbFeatureDetector detector;
	detector.detect(football, keypoints_fb);
	detector.detect(img, keypoints_img);
	OrbDescriptorExtractor extractor;
	extractor.compute(football, keypoints_fb, descriptors_fb);
	extractor.compute(img, keypoints_img, descriptors_img);

	descriptors_img.convertTo(descriptors_img, CV_32F);
	descriptors_fb.convertTo(descriptors_fb, CV_32F);

	FlannBasedMatcher matcher;
	vector<DMatch> matches;
	matcher.match(descriptors_fb, descriptors_img, matches);

	double max_dist = 0; double min_dist = 100;

	for (int i = 0; i < matches.size(); i++) {
		double dist = matches[i].distance;
		if (dist < min_dist)
			min_dist = dist;
		if (dist > max_dist)
			max_dist = dist;
	}

	vector<DMatch> good_matches;

	for (int i = 0; i < matches.size(); i++) {
		if (matches[i].distance <= 3 * min_dist) {
			good_matches.push_back(matches[i]);
		}
	}
	return 10000/(good_matches.size()+1);
}


int main(int argc, char** argv)
{

	// There are 10 test images in total
	//double precision_scores[10], recall_scores[10];
	// output_LIST_FILE "searchResults990"~"searchResults999" : the search results will store in these files
	// SEARCH_IMAGE "990.jpg"~"999.jpg" : go through 990 to 999 as the search images to get your output
	char output_LIST_FILE[100], SEARCH_IMAGE[100];
	//int cnt = 0;

	String selected_img;
	bool isfootball = false;
	bool isbuilding = false;
	bool isdinosaur = false;
	bool iselephant = false;
	bool isrose = false;
	bool ishorse = false;
	bool isman = false;
	int search_image_id=990;
	const String windows[] = { WINDOW1_NAME };
	cvui::init(windows, 1);

	Mat frame = Mat(400, 650, CV_8UC3);

	while (true) {
		frame = Scalar(49, 52, 49);
		cvui::context(windows[0]);




		if (isfootball) {
			search_image_id = 991;
			Mat src = imread("C:\\dataset\\dataset1\\991.jpg");
			Mat src_resized;
			resize(src, src_resized, Size(), 0.5, 0.5, INTER_AREA);
			cvui::image(frame, 300, 90, src_resized);

			selected_img = "football";
			isbuilding = false;
			isdinosaur = false;
			iselephant = false;
			isrose = false;
			ishorse = false;
			isman = false;

		}
		if (isbuilding) {
			search_image_id = 286;

			Mat src = imread("C:\\dataset\\dataset1\\286.jpg");
			Mat src_resized;
			resize(src, src_resized, Size(), 0.7, 0.7, INTER_AREA);
			cvui::image(frame, 300, 90, src_resized);

			selected_img = "building";
			isfootball = false;
			isdinosaur = false;
			iselephant = false;
			isrose = false;
			ishorse = false;
			isman = false;
		}
		if (isdinosaur) {
			search_image_id = 400;

			Mat src = imread("C:\\dataset\\dataset1\\400.jpg");
			Mat src_resized;
			resize(src, src_resized, Size(), 0.7, 0.7, INTER_AREA);
			cvui::image(frame, 300, 90, src_resized);

			selected_img = "dinosaur";
			isfootball = false;
			isbuilding = false;
			iselephant = false;
			isrose = false;
			ishorse = false;
			isman = false;
		}
		if (iselephant) {
			search_image_id = 599;

			Mat src = imread("C:\\dataset\\dataset1\\599.jpg");
			Mat src_resized;
			resize(src, src_resized, Size(), 0.5, 0.5, INTER_AREA);
			cvui::image(frame, 300, 90, src_resized);

			selected_img = "elephant";
			isfootball = false;
			isbuilding = false;
			isdinosaur = false;
			isrose = false;
			ishorse = false;
			isman = false;
		}
		if (isrose) {
			search_image_id = 628;

			Mat src = imread("C:\\dataset\\dataset1\\628.jpg");
			Mat src_resized;
			resize(src, src_resized, Size(), 0.7, 0.7, INTER_AREA);
			cvui::image(frame, 300, 90, src_resized);

			selected_img = "rose";
			isfootball = false;
			isbuilding = false;
			isdinosaur = false;
			iselephant = false;
			ishorse = false;
			isman = false;
		}
		if (ishorse) {
			search_image_id = 732;

			Mat src = imread("C:\\dataset\\dataset1\\732.jpg");
			Mat src_resized;
			resize(src, src_resized, Size(), 0.7, 0.7, INTER_AREA);
			cvui::image(frame, 300, 90, src_resized);

			selected_img = "horse";
			isfootball = false;
			isbuilding = false;
			isdinosaur = false;
			iselephant = false;
			isrose = false;
			isman = false;
		}
		if (isman) {
			search_image_id = 38;

			Mat src = imread("C:\\dataset\\dataset1\\38.jpg");
			Mat src_resized;
			resize(src, src_resized, Size(), 0.7, 0.7, INTER_AREA);
			cvui::image(frame, 300, 90, src_resized);

			selected_img = "man";
			isfootball = false;
			isbuilding = false;
			isdinosaur = false;
			iselephant = false;
			isrose = false;
			ishorse = false;
		}


		cvui::text(frame, 30, 30, "Select one input", 0.8);
		
		cvui::beginColumn(frame, 30, 70, -1, -1, 10);
		cvui::printf("Target = %s", selected_img.c_str(), 0.3);
		cvui::checkbox("Football", &isfootball);
		cvui::checkbox("Building", &isbuilding);
		cvui::checkbox("Dinosaur", &isdinosaur);
		cvui::checkbox("Elephant", &iselephant);
		cvui::checkbox("Rose", &isrose);
		cvui::checkbox("Horse", &ishorse);
		cvui::checkbox("Man", &isman);

		cvui::endColumn();
		cvui::text(frame, 300, 70, "Input Image");
		cvui::text(frame, 30, 370, "Press esc to exit the program",0.3);
		cvui::text(frame, 300, 370, "Retrived images are stored at the searchResults inside dataset file",0.3);
		if (cvui::button(frame, 30, 300, "Start Retrival")) {
			//set the output_LIST_FILE and SEARCH_IMAGE
			sprintf_s(output_LIST_FILE, 100, "searchResults%d", search_image_id);
			sprintf_s(SEARCH_IMAGE, 100, "%d.jpg", search_image_id);

			Mat src_input, gray_input;
			Mat db_img, db_gray_img;

			const int filename_len = 900;
			char tempname[filename_len];

			const int db_size = 1000;
			int db_id = 0;

			const int score_size = 10;   //change this to control return top n images
			double minscore[score_size] = { DBL_MAX };
			int minFilename[score_size];

			char minimg_name[filename_len];
			Mat min_img;

			sprintf_s(tempname, filename_len, "%s\\%s\\%s", IMAGE_folder, IMAGE_LIST_FILE, SEARCH_IMAGE);
			src_input = imread(tempname); // read input image
			if (!src_input.data)
			{
				printf("Cannot find the input image!\n");
				system("pause");
				return -1;
			}
			//imshow("Input", src_input);
			
			cv::cvtColor(src_input, gray_input, COLOR_BGR2GRAY); // Convert to grayscale

			//Read Database
			for (db_id; db_id < db_size; db_id++) {
				sprintf_s(tempname, filename_len, "%s\\%s\\%d.jpg", IMAGE_folder, IMAGE_LIST_FILE, db_id);
				db_img = imread(tempname); // read database image
				if (!db_img.data)
				{
					printf("Cannot find the database image number %d!\n", db_id + 1);
					system("pause");
					return -1;
				}
				cv::cvtColor(db_img, db_gray_img, COLOR_BGR2GRAY); // Convert to grayscale

				double ORBScore = ORBmatch(db_gray_img, gray_input);
				double ColorScore = colorHist(db_gray_img, gray_input);
				double tempScore = ORBScore*ColorScore;

				//store the top k min score ascending
				for (int k = 0; k < score_size; k++) {
					if (tempScore < minscore[k]) {
						for (int k1 = score_size - 1; k1 > k; k1--) {
							minscore[k1] = minscore[k1 - 1];
							minFilename[k1] = minFilename[k1 - 1];
						}
						minscore[k] = tempScore;
						minFilename[k] = db_id;
						break;
					}
				}
			}
			//read the top k max score image and write them to the a designated folder
			for (int k = 1; k < 4; k++) {
				
				sprintf_s(minimg_name, filename_len, "%s\\%s\\%d.jpg", IMAGE_folder, IMAGE_LIST_FILE, minFilename[k]);
				min_img = imread(minimg_name);
				printf("the most similar image %d is %d.jpg, the shape difference is %f\n", k + 1, minFilename[k], minscore[k]);
				sprintf_s(tempname, filename_len, "%s\\searchResults\\%s\\%d.jpg", IMAGE_folder, output_LIST_FILE, minFilename[k]);
				imwrite(tempname, min_img);
				imshow(tempname,min_img);
			}

		}

	cvui::imshow(windows[0], frame);

	if (cv::waitKey(20) == 27) {
		break;
	}
}

	return 0;
}




