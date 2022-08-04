#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc_c.h>

class Track {
private:
	int result[2];
	bool flag_1 = 0, flag_2 = 0;


public:
	int predictTurn(cv::Mat img, int threshold, int track_deepth, bool demo) {

		cv::Mat origin = img.clone();

		cv::GaussianBlur(img, img, cv::Size(5, 5), 0, 0);
		cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
		cv::threshold(img, img, threshold, 255, cv::THRESH_BINARY);
		cv::Mat structureElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));
		cv::erode(img, img, structureElement);

		int k = 0, count = 0;
		for (int i = 0; i < img.cols; i++) {
			if (img.at<uchar>(119, i) == 255) {
				k += i;
				count++;
			}
		}
		k = k / (count + 1);

		int j_min = k, j_max = k;

		for (int i = 119; i > 120 - track_deepth; i--) {
			j_min = k;
			j_max = k;
			do {
				j_min--;
			} while (j_min >= 0 && img.at<uchar>(i, j_min) == 255);

			do {
				j_max++;
			} while (j_max <= 159 && img.at<uchar>(i, j_max) == 255);

			k = (int)((j_min + j_max) / 2);

			if (demo) {
				cv::circle(img, cv::Point(k, i), 1, cv::Scalar(0, 0, 255), -1);
			}
		}

		if (demo) {
			cv::resize(origin, origin, cv::Size(640, 480));
			cv::resize(img, img, cv::Size(640, 480));
			cv::imshow("origin", origin);
			cv::imshow("img", img);
			cv::waitKey(1);
		}

		//if(j_min <= 1 && j_max <= 159) {
		//	flag_1 = 1;
		//}
		//if(flag_1 == 1 && j_min >= 1 && j_max <= 159) {
		//	flag_1 = 0;
		//	flag_2 = 1;
		//}

		//if(flag_1 == 1 && flag_2 == 1) {
		//	flag_1 = 1;
		//	flag_2 = 0;
		//	return -80;
		//}

		return k - 80;
	}

	char* result_to_command(int result, int angle = -1, int speed = -1) {
		char buff[4] = { 0x00, 0x00, 0x0d, 0x0a };
		if (angle != -1){
			buff[0] = (char)speed;
			buff[1] = (char)angle;
		} else {
			angle = (int)((result / 80.0) * 45 + 96);
			if (angle < 51) {
				angle = 51;
			}
			if (angle > 141) {
				angle = 141;
			}
			buff[1] = (char)angle;
		}

		if (speed != -1){
			buff[0] = (char)speed;
		} else {
			speed = 30;
			buff[0] = (char)speed;
		}

		return buff;
	}

};
