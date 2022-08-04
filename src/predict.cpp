#include "../include/predict.hpp"

std::shared_ptr<SystemConfig> Predict::g_system_config = nullptr;
std::shared_ptr<ModelConfig> Predict::g_model_config = nullptr;
std::shared_ptr<PaddlePredictor> Predict::g_predictor = nullptr;


int Predict::predictorInit() {

	std::vector<Place> valid_places({
		Place{TARGET(kFPGA), PRECISION(kFP16), DATALAYOUT(kNHWC)},
		Place{TARGET(kHost), PRECISION(kFloat)},
		Place{TARGET(kARM), PRECISION(kFloat)},
									});

	paddle::lite_api::CxxConfig config;

	if (Predict::g_model_config->is_combined_model) {
		config.set_model_file(Predict::g_model_config->model_file);
		config.set_param_file(Predict::g_model_config->params_file);
	} else {
		config.set_model_dir(Predict::g_model_config->model_params_dir);
	}

	config.set_valid_places(valid_places);

	Predict::g_predictor = paddle::lite_api::CreatePaddlePredictor(config);
	if (!Predict::g_predictor) {
		std::cout << "Error: CreatePaddlePredictor Failed." << std::endl;
		return -1;
	}
	std::cout << "Predictor Init Success !!!" << std::endl;
	return 0;
}

Predict::PredictResult Predict::predict(cv::Mat inputFrame) {

	auto input = Predict::g_predictor->GetInput(0);
	input->Resize(
		{ 1, 3, 40, 80 });

	if (Predict::g_system_config->use_fpga_preprocess) {
		auto* in_data = input->mutable_data<uint16_t>();
		fpga_preprocess(inputFrame, *Predict::g_model_config, input);
	} else {
		auto* in_data = input->mutable_data<float>();
		cpu_preprocess(inputFrame, *Predict::g_model_config, in_data);
	}

	if (Predict::g_model_config->is_yolo) {
		auto img_shape = Predict::g_predictor->GetInput(1);
		img_shape->Resize({ 1, 2 });
		auto* img_shape_data = img_shape->mutable_data<int32_t>();
		img_shape_data[0] = inputFrame.rows;
		img_shape_data[1] = inputFrame.cols;
	}

	Predict::g_predictor->Run();

	auto output = Predict::g_predictor->GetOutput(0);
	float* result_data = output->mutable_data<float>();
	int size = output->shape()[1];
	int max_index = 0;
	float score = 0;
	for (int i = 0; i < size; i++) {
		float value = result_data[i];
		if (value > score) {
			max_index = i;
			score = value;
		}
	}
	PredictResult predict_ret;
	predict_ret.index = max_index;
	predict_ret.score = score;

	return predict_ret;
}

void Predict::printResults(PredictResult& results) {

	if (Predict::g_model_config->labels.size() > 0) {
		std::cout << "label:" << Predict::g_model_config->labels[results.index] << ",";
	}
	std::cout << "index:" << results.index << ",score:" << results.score
		<< std::endl;
}