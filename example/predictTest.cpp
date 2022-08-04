#include "../include/predict.hpp"
#include "../include/videoeffect.hpp"
#include <unistd.h>
#include <opencv2/opencv.hpp>

#include<ctime>
using namespace std;

int main(int argc, char* argv[]) {
	Predict predict;
	clock_t start, end;

	cv::VideoCapture cap(0);

	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	cv::Mat frame = cv::Mat(480, 640, CV_8UC3);
	cv::Mat singnal, demo;

	int ret = 0;
	std::string system_config_path;
	std::string model_config_path;

	system_config_path = "../configs/image.json";
	Predict::g_system_config = make_shared<SystemConfig>(system_config_path);
	model_config_path = Predict::g_system_config->model_config_path;
	Predict::g_model_config = make_shared<ModelConfig>(Predict::g_system_config->model_config_path);



	ret = predict.predictorInit();
	if (ret != 0) {
		std::cout << "Error!!! predictor init failed .\n";
		exit(-1);
	}

	Predict::PredictResult predict_results;


	while (true) {
		start = clock();
		if (!cap.read(frame)) {
			std::cout << "Can not read frame" << std::endl;
			return -1;
		}

		singnal = videoeffect(frame);
		cv::resize(singnal, demo, cv::Size(800, 400));
		end = clock();

		cv::imshow("frame", demo);

		cv::waitKey(1);

		double endtime = (double)(end - start) / CLOCKS_PER_SEC;
		cout << "FPS:" << 1/endtime << endl;		//s为单位

		//cv::Mat lallalal = cv::imread(argv[1]);
		predict_results = predict.predict(singnal);
		predict.printResults(predict_results);
		//std::cout << singnal.channels() << std::endl;

	}
	return 0;
}