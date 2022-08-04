#include <opencv2/opencv.hpp>

void cameraInit(cv::VideoCapture cap, int width = 640, int height = 480, int fps = 60, int brightness = 20, int contrast = 250, int saturation = 20) {
	// Initialize the camera
	cap.set(cv::CAP_PROP_FRAME_WIDTH, width);//宽度
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);//高度
	cap.set(cv::CAP_PROP_FPS, fps);
	cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
	cap.set(cv::CAP_PROP_BRIGHTNESS, brightness);//亮度 
	cap.set(cv::CAP_PROP_CONTRAST, contrast);//对比度 40
	cap.set(cv::CAP_PROP_SATURATION, saturation);
}

cv::Mat videoeffect(cv::Mat frame) {
	cv::Mat singnal = frame(cv::Range(240, 400), cv::Range(160, 480));

	cv::GaussianBlur(singnal, singnal, cv::Size(3, 3), 0, 0);
	cv::Mat hsv;

	cv::cvtColor(singnal, hsv, cv::COLOR_BGR2HSV);

	cv::Mat mask, mask_red1, mask_red2;

	cv::inRange(hsv, cv::Scalar(156, 43, 46), cv::Scalar(180, 255, 255), mask_red1);
	cv::inRange(hsv, cv::Scalar(0, 43, 46), cv::Scalar(10, 255, 255), mask_red2);
	cv::bitwise_or(mask_red1, mask_red2, mask);

	cv::Mat structureElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));
	cv::erode(mask, mask, structureElement);
	cv::dilate(mask, mask, structureElement);

	cv::resize(mask, mask, cv::Size(80, 40));

	cv::cvtColor(mask, mask, cv::COLOR_GRAY2BGR);

	return mask;
}
