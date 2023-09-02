#include <iostream>
#include <vector>
#include <chrono>
#include <conio.h>
#include <cmath>
#include <Windows.h>
#include <tchar.h>
#include <string>

class Stream {
public:
	int clickLimit;
	long beginTime = -1;

	std::vector<long long> clickTimes;
	std::vector<long long> timeIntervals;
	std::vector<double> deviations;

	char keyOne = 'z';
	char keyTwo = 'x';
};

void Logger(std::string msg) {
	auto now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm localTime;
	localtime_s(&localTime, &time);
	char buffer[9];
	strftime(buffer, sizeof(buffer), "%H:%M:%S", &localTime);
	std::cout << "[" + std::string(buffer) + "] " + msg;
}

void SetConsoleTextColor(int colorCode) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, colorCode);
}

void CalculateUnstableRate(Stream& stream) {
	double sumDeviations = 0.0;
	for (double deviation : stream.deviations) {
		sumDeviations += deviation;
	}
	double variance = sumDeviations / (stream.deviations.size() - 1);
	double stdDeviation = std::sqrt(variance);
	double unstableRate = stdDeviation * 10.0;

	//Logger("Unstable Rate: " + std::to_string(unstableRate));
}

double CalculateBPM(Stream& stream, long long totalTime) {
	double avgTimeInterval = static_cast<double>(totalTime) / (stream.clickTimes.size() - 1);
	double bpm = (stream.clickTimes.size() / (static_cast<double>(totalTime) / 60000.0)) / 4.0;

	std::cout << R"(
 _____             _ _
| __  |___ ___ _ _| | |_ ___
|    -| -_|_ -| | | |  _|_ -|
|__|__|___|___|___|_|_| |___|
                             )" << "\n" << std::endl;
	Logger("Average Key Press Interval: " + std::to_string(avgTimeInterval) + " ms\n");
	Logger("BPM: " + std::to_string(bpm) + "\n");

	CalculateUnstableRate(stream);

	return bpm;
}

void ProcessGetch(Stream& stream, char keyPress) {
	if (keyPress != stream.keyOne && keyPress != stream.keyTwo) {
		return;
	}

	auto currentTime = std::chrono::system_clock::now();
	std::time_t epochTime = std::chrono::system_clock::to_time_t(currentTime);
	stream.beginTime = static_cast<int>(epochTime);

	if (stream.clickTimes.size() + 1 <= stream.clickLimit) {
		long long currentTimeMillis = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();

		stream.clickTimes.push_back(currentTimeMillis);

		if (stream.clickTimes.size() > 1) {
			long long timeDiff = stream.clickTimes.back() - stream.clickTimes[stream.clickTimes.size() - 2];
			stream.timeIntervals.push_back(timeDiff);
		}

		if (stream.clickTimes.size() == stream.clickLimit) {
			long long totalTime = stream.clickTimes.back() - stream.clickTimes.front();
			CalculateBPM(stream, totalTime);
		}
	}
}

int main() {
	Stream stream;

	while (true) {
		system("cls");

		SetConsoleTextColor(13);
		std::cout << R"(
 _____ _                     _____               _
|   __| |_ ___ ___ ___ _____|   __|___ ___ ___ _| |
|__   |  _|  _| -_| .'|     |__   | . | -_| -_| . |
|_____|_| |_| |___|__,|_|_|_|_____|  _|___|___|___|
                                  |_|
StreamSpeed by Serialized)" << "\n" << std::endl;

		Logger("Clicks to measure: ");
		std::cin >> stream.clickLimit;

		Logger("Ready to measure. Start streaming now.\n");

		while (stream.clickTimes.size() < stream.clickLimit) {
			char c = _getch();
			ProcessGetch(stream, c);
		}

		std::cout << R"(
             __
            |  |          _____         _     _ _           _
 ___ ___ _ _|  |   ___   |   __|___ ___|_|___| |_|___ ___ _| |
| . |_ -| | |__|  |___|  |__   | -_|  _| | .'| | |- _| -_| . |
|___|___|___|__|         |_____|___|_| |_|__,|_|_|___|___|___|

															)" << std::endl;

		char playAgain;
		Logger("Do you want to try again? (Y/N): ");
		std::cin >> playAgain;

		if (playAgain != 'Y' && playAgain != 'y') {
			break;
		}

		stream.clickTimes.clear();
		stream.timeIntervals.clear();
		stream.deviations.clear();
		stream.beginTime = -1;
	}

	return 0;
}