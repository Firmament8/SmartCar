#include "../include/videoeffect.hpp"

int main(int argc, char* argv[]) {
	cv::VideoCapture cap(1);

	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	cv::Mat frame = cv::Mat(480, 640, CV_8UC3);
	cv::Mat singnal, demo;

	int i = 468;
	bool flag = 0;

	while (true) {

		if (!cap.read(frame)) {
			std::cout << "Can not read frame" << std::endl;
			return -1;
		}

		singnal = videoeffect(frame);
		cv::resize(singnal, demo, cv::Size(800, 400));
		
		cv::imshow("frame", demo);

		char key = cv::waitKey(100) & 0xFF;

		if (key == 'q') {
			flag = !flag;
		}

		if (flag && i <= 1000) {
			cv::imwrite("C:/Users/ruri/Desktop/ppp/" + std::to_string(i) + ".jpg", singnal);
			i++;
		}
	}
	return 0;
}