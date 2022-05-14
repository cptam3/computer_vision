// DepthImage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
#include <fstream>
#include <string>
#include <vector>

#define IMAGE_folder "C:\\StereoMatchingTestings"
#define IMG_1 "view1.png"
#define IMG_2 "view5.png"
#define output "disp2.png"

using namespace std;
using namespace cv;

double SAD(Mat img1,Mat img2) {
    Mat img_1 = img1;
    Mat img_2 = img2;
    if (img_1.type() != CV_32F) {
        img_1.convertTo(img_1, CV_32F);
    }
    if (img_2.type() != CV_32F) {
        img_2.convertTo(img_2, CV_32F);
    }
   
    double sum = 0;
    for (int y = 0; y < img_1.rows; y++) {
        for (int x = 0; x < img_1.cols; x++) {
            float diff = abs(img_1.at<float>(y, x) - img_2.at<float>(y, x));
            sum += (double)diff;
        }
    }
    
    return sum;
}

int main()
{
    vector<string> img { "Art", "Dolls", "Reindeer" };
    char img1_path[200];
    char img2_path[200];
    char output_path[200];

    int window_size = 15;  

    for (int i = 0; i < img.size(); i++) {
        sprintf_s(img1_path, 200, "%s\\%s\\%s", IMAGE_folder, img[i].c_str(), IMG_1);
        sprintf_s(img2_path, 200, "%s\\%s\\%s", IMAGE_folder, img[i].c_str(), IMG_2);
        Mat img1 = imread(img1_path);
        Mat img2 = imread(img2_path);
        double img_w = img1.cols;
        double img_h = img1.rows;
        int num_window_w = img_w / (double)window_size;
        int num_window_h = img_h / (double)window_size;
        int new_w = num_window_w * window_size;
        int new_h = num_window_h * window_size;
        Mat new_img1;
        Mat new_img2;
        resize(img1, new_img1, Size(new_w, new_h), INTER_LINEAR);
        resize(img2, new_img2, Size(new_w, new_h), INTER_LINEAR);

        Mat Grey_img1;
        Mat Grey_img2;
        cvtColor(new_img1, Grey_img1, COLOR_BGR2GRAY);
        cvtColor(new_img2, Grey_img2, COLOR_BGR2GRAY);

        vector<vector<double>> disparity(new_h, vector<double>(new_w, 0));
        Mat disparityMap;
        disparityMap = Mat::zeros(new_h, new_w, CV_8U);

        for (int y = 0; y < num_window_h; y++) {
            for (int x = 0; x < num_window_w; x++) {
                Mat crop_img1;
                crop_img1 = Grey_img1(Range(y * window_size, y * window_size + window_size - 1), Range(x * window_size, x * window_size + window_size - 1));

                double min_score = 10000000;
                int best_window = 0;

                int min_slide = x * window_size -  (int)(num_window_w*0.3) * window_size;
                if (min_slide < 0) {
                    min_slide = 0;
                }
                int max_slide = x * window_size;
                for (int slide = min_slide; slide < max_slide-1; slide ++) {
                    Mat crop_img2;
                    crop_img2 = Grey_img2(Range(y * window_size, y * window_size + window_size - 1), Range(slide, slide + window_size -1));
                    
                    double score = SAD(crop_img1,crop_img2);
                    if (score < min_score) {
                        min_score = score;
                        best_window = slide;
                    }
                }
                int dist_x = abs(x*window_size - best_window);

                for (int wy = 0; wy < window_size; wy++) {
                    for (int wx = 0; wx < window_size; wx++) {
                        int dx = x * window_size + wx;
                        int dy = y * window_size + wy;
                        disparity[dy][dx] = dist_x;
                    }
                }
            }
        }
        cout << img[i] << "'s disparity is calculated" << endl;
        for (int y = 0; y < new_h; y++) {
            for (int x = 0; x < new_w; x++) {
                double dispar = disparity[y][x];
                if (dispar > 255) {
                    dispar = 255;
                }
                disparityMap.at<uchar>(y, x) = (int)dispar;
            }
        }
        
        for (int y = disparityMap.rows - 2; y > 0; y--) {
            for (int x = disparityMap.cols - 2; x > 0; x--) {
                
                int confidence = 8;
                int temp = disparityMap.at<uchar>(y, x + 1);
                
                if (abs(disparityMap.at<uchar>(y, x) - disparityMap.at<uchar>(y - 1, x - 1)) > 60) {
                    confidence--;
                    temp = disparityMap.at<uchar>(y - 1, x - 1);
                }
                if (abs(disparityMap.at<uchar>(y, x) - disparityMap.at<uchar>(y - 1, x)) > 60) {
                    confidence--;
                    temp = disparityMap.at<uchar>(y - 1, x);
                }
                if (abs(disparityMap.at<uchar>(y, x) - disparityMap.at<uchar>(y - 1, x + 1)) > 60) {
                    confidence--;
                    temp = disparityMap.at<uchar>(y - 1, x + 1);
                }
                if (abs(disparityMap.at<uchar>(y, x) - disparityMap.at<uchar>(y , x - 1)) > 60) {
                    confidence--;
                    temp = disparityMap.at<uchar>(y, x - 1);
                }
                if (abs(disparityMap.at<uchar>(y, x) - disparityMap.at<uchar>(y, x + 1)) > 60) {
                    confidence--;
                    temp = disparityMap.at<uchar>(y, x + 1);
                }
                if (abs(disparityMap.at<uchar>(y, x) - disparityMap.at<uchar>(y + 1, x - 1)) > 60) {
                    confidence--;
                    temp = disparityMap.at<uchar>(y + 1, x - 1);
                }
                if (abs(disparityMap.at<uchar>(y, x) - disparityMap.at<uchar>(y + 1, x)) > 60) {
                    confidence--;
                    temp = disparityMap.at<uchar>(y + 1, x);
                }
                if (abs(disparityMap.at<uchar>(y, x) - disparityMap.at<uchar>(y + 1, x + 1)) > 60) {
                    confidence--;
                    temp = disparityMap.at<uchar>(y + 1, x + 1);
                }

                if (confidence<4) {
                    disparityMap.at<uchar>(y, x) = temp;
                }
                
                if (disparityMap.at<uchar>(y, x) == 255 || disparityMap.at<uchar>(y, x) == 0) {
                    disparityMap.at<uchar>(y, x) = temp;
                }  
            }
        }
        
        GaussianBlur(disparityMap, disparityMap,Size(5,5),0);
        
        resize(disparityMap, disparityMap, Size(img_w, img_h), INTER_LINEAR);
        sprintf_s(output_path, 200, "%s\\%s\\%s", IMAGE_folder, img[i].c_str(), output);
        imwrite(output_path, disparityMap);
        cout << img[i] << " is outputted" << endl;

    }


    waitKey(0);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
