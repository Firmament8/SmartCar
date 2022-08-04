#include "common.hpp"
#include "preprocess.hpp"
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include<ctime>
using namespace std;


class Predict {
	public:

	struct PredictResult {
		float score;
		int index;
	};

	static std::shared_ptr<SystemConfig> g_system_config;
	static std::shared_ptr<ModelConfig> g_model_config;
	static std::shared_ptr<PaddlePredictor> g_predictor;

	int predictorInit();
	PredictResult predict(cv::Mat inputFrame);
	void printResults(PredictResult& results);

};