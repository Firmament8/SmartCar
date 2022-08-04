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