#include "../include/videoeffect.hpp"
#include "../include/serialPort.hpp"
#include "../include/predict.hpp"
#include "../include/track.hpp"
#include "../include/json.hpp"
#include "../include/PID.hpp"

#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <fstream>
#include <math.h>
#include <ctime>

using namespace std;
using json = nlohmann::json;

int track(serialPort serial) {
	std::cout << "Smartcar has started!" << std::endl;

    std::ifstream f("../configs/params.json");
    if (!f.good()) {
      std::cout << "Error:file path not find." << std::endl;
      exit(-1);
    }

    json params;
    is >> params;

	cv::VideoCapture cap_track(1);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 160);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 120);

	Predict predict; 
	int ret = 0;

	Track track;
	
	PID pid(0, 1.5, 0, 0);

	cv::Mat frame_track = cv::Mat(160, 120, CV_8UC3);

	int angle = 0;
	int speed = 0;

	char buff[4] = { 0x00,0x00,0x0d,0x0a };

	while (1) {
		if (!cap_track.read(frame_track)) {
			std::cout << "Can not read frame!" << std::endl;
			return -1;
		}
		angle = 96;
		speed = 0;

		cv::resize(frame, frame, cv::Size(160, 120));
		int result = pid.pid(track.predictTurn(frame, 0));
		buff = track.result_to_command(result);

		if (serial.serial(buff, 4) == 0xff) {
			buff[0] = (char)0;
			buff[1] = (char)96;
			serial.serial(buff, 4);
			std::cout << "Smartcar has paused!" << std::endl;
			return -1;
		}
	}
}


int main(int argc, char** argv) {
	serialPort serial;
	serial.OpenPort("/dev/ttyPS1");
	serial.setup(38400, 0, 8, 1, 'N');

	char start[4];

	while (true) {
		if (serial.readBuffer(start, 4)) {
			if (start[0] == 0xee && start[1] == 0xee && start[2] == 0x0a && start[3] == 0x0a) {
				track(serial);
			}
		}
	}

	return 0;
}

int track_backup(serialPort serial) {
	std::cout << "Smartcar has started!" << std::endl;
	cv::VideoCapture cap(0);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	Predict predict; 

	int ret = 0;

	
	Track track;
	
	PID pid;			
	pid.init(0, 1.5, 0, 0);

	cv::Mat frame = cv::Mat(480, 640, CV_8UC3);
	cv::Mat singnal;

	int angle = 0;
	int speed = 0;

	std::string system_config_path;
	std::string model_config_path;

	system_config_path = "../configs/image.json";
	Predict::g_system_config = make_shared<SystemConfig>(system_config_path);
	model_config_path = Predict::g_system_config->model_config_path;
	Predict::g_model_config = make_shared<ModelConfig>(Predict::g_system_config->model_config_path);

	ret = predict.predictorInit();

	std::cout << ret << "ret";
	if (ret != 0) {
		std::cout << "Error!!! predictor init failed .\n";
		exit(-1);
	}
	
	char buff[4] = { 0x00,0x00,0x0d,0x0a };
	Predict::PredictResult predict_results;




	while (1) {
		if (!cap.read(frame)) {
			std::cout << "Can not read frame!" << std::endl;
			return -1;
		}
		angle = 96;
		speed = 0;

		singnal = videoeffect(frame);

		cv::resize(frame, frame, cv::Size(160, 120));
		int result = pid.pid(track.predictTurn(frame, 0));
		angle = track.result_to_angle(result);


		int pridict_index = predict.predict(singnal).index;
		if (pridict_index == 0) {
			speed = 40;
			angle = 96;
		}

		
		std::cout << "Index is : " << pridict_index << " Angle is:" << angle << " Speed is:" << speed << std::endl;

		buff[0] = (char)speed;
		buff[1] = (char)angle;

		if (serial.serial(buff, 4) == 0xff) {
			buff[0] = (char)0;
			buff[1] = (char)96;
			serial.serial(buff, 4);
			std::cout << "Smartcar has paused!" << std::endl;
			return -1;
		}
	}
}
