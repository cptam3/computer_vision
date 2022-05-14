#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>


#define IMAGE_folder "C:\\datasets_4186"     
#define GALLERY_LIST_FILE "gallery_4186" 
#define QUERY_LIST_FILE "query_4186" 
#define QUERY_TEXT_LIST_FILE "query_txt_4186" 
#define NUMBER_QUERY 20;
#define NUMBER_GALLERY 5020;

#define PI 3.14159265

using namespace std;
using namespace cv;

bool PeakChecking(Mat DoG[], int i, int x, int y) {

    bool isLarger = true;
    bool isSmaller = true;
    for (int layer = -1; layer < 2; layer++) {  //check 26 pixels around the pixels
        for (int k_X = -1; k_X < 2; k_X++) {
            for (int k_Y = -1; k_Y < 2; k_Y++) {
                if (!(k_X == 0 && k_Y == 0) && layer == 0) {
                    if ((int)DoG[i].at<uchar>(y, x) <= (int)DoG[i + layer].at<uchar>(y + k_Y, x + k_X)) {
                        isLarger = false;
                    }
                    if ((int)DoG[i].at<uchar>(y, x) >= (int)DoG[i + layer].at<uchar>(y + k_Y, x + k_Y)) {
                        isSmaller = false;
                    }
                }
                if (isLarger == false && isSmaller == false) {
                    return false;
                }
            }
        }
    }
    if (isLarger == true || isSmaller == true) {
        return true;
    }
}

double ORB_Match(Mat descriptors_q, Mat gallery) {
    vector<KeyPoint> keypoints_g;
    Mat descriptors_g;
    Ptr<Feature2D> orb = ORB::create();
    orb->detectAndCompute(gallery, Mat(), keypoints_g, descriptors_g);
    //SiftDescriptorExtractor extractor;
    //extractor.compute(gallery, keypoiints_g, descriptors_g);

    descriptors_g.convertTo(descriptors_g, CV_32F);

    FlannBasedMatcher matcher;
    vector<DMatch> matches;
    matcher.match(descriptors_q, descriptors_g, matches);

    double min_dist = 100;
    vector<DMatch> good_matches;
    for (int i = 0; i < matches.size(); i++) {
        double dist = matches[i].distance;  //find min. dist
        if (dist < min_dist)
            min_dist = dist;

    }
    for (int i = 0; i < matches.size(); i++) {
        if (matches[i].distance <= 3 * min_dist) {  //clustering
            good_matches.push_back(matches[i]);
        }
    }
    double score = (double)good_matches.size() / (double)matches.size();
    return 1 - score;
}
/*double sift(Mat Gallery, vector<vector<int>> query_keypoints, vector<vector<vector<double>>> query_descriptors) {
    Mat Gussian[5];

    Gussian[0] = Gallery;
    GaussianBlur(Gussian[0], Gussian[1], Size(5, 5), 1.6);
    GaussianBlur(Gussian[1], Gussian[2], Size(5, 5), sqrt(2) * 1.6);
    GaussianBlur(Gussian[2], Gussian[3], Size(5, 5), sqrt(2) * 1.6);
    GaussianBlur(Gussian[3], Gussian[4], Size(5, 5), sqrt(2) * 1.6);

    Mat DoG[4];
    for (int i = 3; i >= 0; i--) {
        DoG[i] = Gussian[i] - Gussian[i + 1];
        normalize(DoG[i], DoG[i], 255, 0, NORM_MINMAX);
    }


    vector<vector<int>> keypoints;
    for (int i = 1; i < 3; i++) {    //check 2 layers
        for (int y = 1; y < DoG[0].rows-1; y++) {   //check all pixels in a layers
            for (int x = 1; x < DoG[0].cols-1; x++) {
                if (PeakChecking(DoG, i, x, y)){
                    if (sqrt( ( (int)DoG[i].at<uchar>(y, x + 1) - (int)DoG[i].at<uchar>(y , x - 1) )*((int)DoG[i].at<uchar>(y, x + 1) - (int)DoG[i].at<uchar>(y, x - 1)) +
                        ((int)DoG[i].at<uchar>(y + 1, x) - (int)DoG[i].at<uchar>(y - 1, x))*( (int)DoG[i].at<uchar>(y + 1, x) - (int)DoG[i].at<uchar>(y - 1, x ) ) )>15) {   //use gradient to get strong edge
                        if ((int)DoG[i].at<uchar>(y, x)>100) {   //reduce noise
                            keypoints.push_back({ x,y });
                        }

                    }
                }
            }
        }
    }

    vector<int> peak(keypoints.size(), 0);
    vector<vector<double>> hist(keypoints.size(), vector<double>(36, 0));
    vector<vector<vector<double>>> descriptors(keypoints.size(), vector<vector<double>>(16, vector<double>(8, 0)));
    for (int i = 0; i < keypoints.size(); i++) {   //get gradient oritention 36-bins hists

        if (keypoints[i][0] > 8 && keypoints[i][0] < (DoG[0].cols - 9) && keypoints[i][1] > 8 && keypoints[i][1] < (DoG[0].rows - 9)) {
            for (int y = -7; y < 9; y++) {
                for (int x = -7; x < 9; x++) {

                    double Gx = DoG[0].at<uchar>(keypoints[i][1]+y, keypoints[i][0]+x + 1) - DoG[0].at<uchar>(keypoints[i][1]+y, keypoints[i][0]+x - 1);
                    double Gy = DoG[0].at<uchar>(keypoints[i][1]+y + 1, keypoints[i][0]+x) - DoG[0].at<uchar>(keypoints[i][1]+y - 1, keypoints[i][0]+x);
                    double mag = sqrt(Gx * Gx + Gy * Gy);
                    int ang = (int)(atan(Gy / (Gx+1))*180/PI +360)%360;   //0-360
                    ang = (int)round(ang / 10) % 36;  //0-35
                    if (ang >= 0 && ang < 36) {
                        hist[i][ang] += mag;
                    }
                }
            }
        }
        double highest = 0;
        int peakang = 0;
        for (int j = 0; j < 36; j++) {
            if (hist[i][j] > highest) {
                highest = hist[i][j];
                peakang = j;
            }
        }
        peak[i] = peakang;
        if (keypoints[i][0] > 8 && keypoints[i][0] < (DoG[0].cols - 9) && keypoints[i][1] > 8 && keypoints[i][1] < (DoG[0].rows - 9)) {
            int init_X = keypoints[i][0] - 7;
            int init_y = keypoints[i][1] - 7;
            for (int gridY = 0; gridY < 4; gridY++) {
                for (int gridX = 0; gridX < 4; gridX++) {
                    for (int y = 0; y < 4; y++) {
                        for (int x = 0; x < 4; x++) {
                            double Gx = DoG[0].at<uchar>(init_y + gridY*4 + y, init_X + gridX*4 + x + 1) - DoG[0].at<uchar>(init_y + gridY * 4 + y, init_X + gridX * 4 + x - 1);
                            double Gy = DoG[0].at<uchar>(init_y + gridY * 4 + y + 1, init_X + gridX * 4 + x) - DoG[0].at<uchar>(init_y + gridY * 4 + y - 1, init_X + gridX * 4 + x);
                            double mag = sqrt(Gx * Gx + Gy * Gy);
                            int ang = (int)(atan(Gy / (Gx + 1)) * 180 / PI + 360) % 360;//0-360
                            ang = (ang - peak[i] + 360) % 360;   //change oritention
                            ang = (int)round(ang / 45) % 8;  //0-7
                            if (ang >= 0 && ang < 8) {
                                descriptors[i][(int)(gridY*4+gridX)][ang] += mag;
                            }
                        }
                    }
                }
            }
        }
    }

    vector <vector<double>> matches(query_keypoints.size(), vector<double>(2, 1000000));
    int goodMatches = 0;
    for (int i = 0; i < query_keypoints.size(); i++) {
        for (int j = 0; j < keypoints.size(); j++) {
            double dist = 0;
            for (int gridY = 0; gridY < 4; gridY++) {
                for (int gridX = 0; gridX < 4; gridX++) {
                    for (int a = 0; a < 8; a++) {

                        dist += (descriptors[j][(int)(gridY * 4 + gridX)][a] - query_descriptors[i][(int)(gridY * 4 + gridX)][a]) * (descriptors[j][(int)(gridY * 4 + gridX)][a] - query_descriptors[i][(int)(gridY * 4 + gridX)][a])/10000;
                    }
                }
            }
            if (dist < matches[i][1]) {
                matches[i][1] = dist;     //search top two matches
                if (matches[i][0] > matches[i][1]) {    //sort
                    double temp = matches[i][0];
                    matches[i][0] = matches[i][1];
                    matches[i][1] = temp;
                }
            }
        }
        double ratio = matches[i][0] / matches[i][1];
        if ( ratio < 0.75) {  //find good matches
            goodMatches++;
        }
    }
    return goodMatches;
}*/

int main()
{
    double precision_scores[20][10];
    double rank_list[20][10];
    int numOfQuery = NUMBER_QUERY;
    int numofGallery = NUMBER_GALLERY;
    int rank_size = 10;

    char Query_Path[200];
    char Query_txt_Path[200];
    char Gallery_Path[200];

    for (int q = 0; q < numOfQuery; q++) {

        sprintf_s(Query_Path, 200, "%s\\%s\\%d.jpg", IMAGE_folder, QUERY_LIST_FILE, q);      //get the query path
        sprintf_s(Query_txt_Path, 200, "%s\\%s\\%d.txt", IMAGE_folder, QUERY_TEXT_LIST_FILE, q);    //get the txt path

        Mat Img = imread(Query_Path);
        if (!Img.data)
        {
            printf("Cannot find the input image!\n");
            system("pause");
            return -1;
        }
        Mat Grey;
        cvtColor(Img, Grey, COLOR_BGR2GRAY);
        Mat crop_Img;
        int txt_data[4];
        ifstream txt_file(Query_txt_Path);
        if (!txt_file.is_open()) {
            printf("Cannot find the txt file!\n");
            system("pause");
            return -1;
        }
        for (int i = 0; i < 4; i++) {
            txt_file >> txt_data[i];
        }
        txt_file.close();
        crop_Img = Grey(Range(txt_data[1], txt_data[1] + txt_data[3]), Range(txt_data[0], txt_data[0] + txt_data[2]));

        //char name[100];
        //sprintf_s(name, 100, "Query %d", i);
        //namedWindow(name, WINDOW_NORMAL);
        //imshow(name, crop_Img);

        //SIFT for query
       /* Mat Gussian[5];

        Gussian[0] = crop_Img;
        GaussianBlur(Gussian[0], Gussian[1], Size(5, 5), 1.6);
        GaussianBlur(Gussian[1], Gussian[2], Size(5, 5), sqrt(2) * 1.6);
        GaussianBlur(Gussian[2], Gussian[3], Size(5, 5), sqrt(2) * 1.6);
        GaussianBlur(Gussian[3], Gussian[4], Size(5, 5), sqrt(2) * 1.6);

        Mat DoG[4];
        for (int i = 3; i >= 0; i--) {
            DoG[i] = Gussian[i] - Gussian[i + 1];
            normalize(DoG[i], DoG[i], 255, 0, NORM_MINMAX);
        }


        vector<vector<int>> keypoints;
        for (int i = 1; i < 3; i++) {    //check 2 layers
            for (int y = 1; y < DoG[0].rows - 1; y++) {   //check all pixels in a layers
                for (int x = 1; x < DoG[0].cols - 1; x++) {
                    if (PeakChecking(DoG, i, x, y)) {
                        if (sqrt(((int)DoG[i].at<uchar>(y, x + 1) - (int)DoG[i].at<uchar>(y, x - 1)) * ((int)DoG[i].at<uchar>(y, x + 1) - (int)DoG[i].at<uchar>(y, x - 1)) +
                            ((int)DoG[i].at<uchar>(y + 1, x) - (int)DoG[i].at<uchar>(y - 1, x)) * ((int)DoG[i].at<uchar>(y + 1, x) - (int)DoG[i].at<uchar>(y - 1, x))) > 15) {   //use gradient to get strong edge
                            if ((int)DoG[i].at<uchar>(y, x) > 100) {   //reduce noise
                               // drawMarker(query, Point(x, y), (255, 0, 0), 0, 4, 1);
                                keypoints.push_back({ x,y });
                            }

                        }
                    }
                }
            }
        }

        //cout << keypoints.size()<<endl;

        vector<int> peak(keypoints.size(), 0);
        vector<vector<double>> hist(keypoints.size(), vector<double>(36, 0));
        vector<vector<vector<double>>> descriptors(keypoints.size(), vector<vector<double>>(16, vector<double>(8, 0)));
        for (int i = 0; i < keypoints.size(); i++) {   //get gradient oritention 36-bins hists

            if (keypoints[i][0] > 8 && keypoints[i][0] < (DoG[0].cols - 9) && keypoints[i][1] > 8 && keypoints[i][1] < (DoG[0].rows - 9)) {
                for (int y = -7; y < 9; y++) {
                    for (int x = -7; x < 9; x++) {

                        double Gx = DoG[0].at<uchar>(keypoints[i][1] + y, keypoints[i][0] + x + 1) - DoG[0].at<uchar>(keypoints[i][1] + y, keypoints[i][0] + x - 1);
                        double Gy = DoG[0].at<uchar>(keypoints[i][1] + y + 1, keypoints[i][0] + x) - DoG[0].at<uchar>(keypoints[i][1] + y - 1, keypoints[i][0] + x);
                        double mag = sqrt(Gx * Gx + Gy * Gy);
                        int ang = (int)(atan(Gy / (Gx + 1)) * 180 / PI + 360) % 360;   //0-360
                        ang = (int)round(ang / 10) % 36;  //0-35
                        if (ang >= 0 && ang < 36) {
                            hist[i][ang] += mag;
                        }
                    }
                }
            }
            double highest = 0;
            int peakang = 0;
            for (int j = 0; j < 36; j++) {
                if (hist[i][j] > highest) {
                    highest = hist[i][j];
                    peakang = j;
                }
            }
            peak[i] = peakang;
            if (keypoints[i][0] > 8 && keypoints[i][0] < (DoG[0].cols - 9) && keypoints[i][1] > 8 && keypoints[i][1] < (DoG[0].rows - 9)) {
                int init_X = keypoints[i][0] - 7;
                int init_y = keypoints[i][1] - 7;
                for (int gridY = 0; gridY < 4; gridY++) {
                    for (int gridX = 0; gridX < 4; gridX++) {
                        for (int y = 0; y < 4; y++) {
                            for (int x = 0; x < 4; x++) {
                                double Gx = DoG[0].at<uchar>(init_y + gridY * 4 + y, init_X + gridX * 4 + x + 1) - DoG[0].at<uchar>(init_y + gridY * 4 + y, init_X + gridX * 4 + x - 1);
                                double Gy = DoG[0].at<uchar>(init_y + gridY * 4 + y + 1, init_X + gridX * 4 + x) - DoG[0].at<uchar>(init_y + gridY * 4 + y - 1, init_X + gridX * 4 + x);
                                double mag = sqrt(Gx * Gx + Gy * Gy);
                                int ang = (int)(atan(Gy / (Gx + 1)) * 180 / PI + 360) % 360;//0-360
                                ang = (ang - peak[i] + 360) % 360;   //change oritention
                                ang = (int)round(ang / 45) % 8;  //0-8
                                if (ang >= 0 && ang < 8) {
                                    descriptors[i][(int)(gridY * 4 + gridX)][ang] += mag;
                                }
                            }
                        }
                    }
                }
            }
        }
        */

        vector<KeyPoint> keypoints_q;
        Mat descriptors_q;
        Ptr<Feature2D> orb = ORB::create();
        orb->detectAndCompute(crop_Img, Mat(), keypoints_q, descriptors_q);
        descriptors_q.convertTo(descriptors_q, CV_32F);

        double score_rank[10] = { 1000000 };
        int gallery_rank[10] = { 0 };
        int n = sizeof(score_rank) / sizeof(score_rank[0]);
        for (int i = 0; i < numofGallery; i++) {
            sprintf_s(Gallery_Path, 200, "%s\\%s\\%d.jpg", IMAGE_folder, GALLERY_LIST_FILE, i);
            Mat Gallery_Img = imread(Gallery_Path);
            if (!Gallery_Img.data)
            {
                continue;
            }
            //cout << i << " ";
            Mat Grey_Gallery;
            cvtColor(Gallery_Img, Grey_Gallery, COLOR_BGR2GRAY);

            double score = ORB_Match(descriptors_q, Grey_Gallery);
            for (int k = 0; k < rank_size; k++) {     //sort top 10 img
                if (score < score_rank[k]) {
                    for (int k1 = rank_size - 1; k1 > k; k1--) {
                        score_rank[k1] = score_rank[k1 - 1];
                        gallery_rank[k1] = gallery_rank[k1 - 1];
                    }
                    score_rank[k] = score;
                    gallery_rank[k] = i;
                    break;
                }
            }

        }

        cout << "Q" << q + 1 << ": ";
        for (int i = 0; i < rank_size; i++) {    //fill in the rank list
            precision_scores[q][i] = score_rank[i];
            rank_list[q][i] = gallery_rank[i];

            cout << rank_list[q][i] << ".jpg:" << precision_scores[q][i] << " ";
        }
        cout << "." << endl;

    }

    waitKey(0);
    destroyAllWindows();
    return 0;
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
