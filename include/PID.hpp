#pragma once

class PID {
public:
	double expectations = 0;
	double Kp = 1, Ki = 0, Kd = 0;

	double value_back = 0;
	double value_sum = 0;
	
	PID(double expect, double p, double i, double d) {
		expectations = expect;
		Kp = p;
		Ki = i;
		Kd = d;

		value_sum = 0;
	}

	double proportion(double inputValue) {
		double result = Kp * (expectations - inputValue);
		return result;
	}

	double integral(double inputValue) {
		value_sum += (expectations - inputValue);
		double result = Ki * value_sum;
		return result;
	}

	double derivative(double inputValue) {
		double result = Kd * (inputValue - value_back);
		return result;
	}

	double pid(double inputValue) {
		value_back = inputValue;
		double result = inputValue + proportion(inputValue) + integral(inputValue) + derivative(inputValue);
		return result - inputValue;
	}
};