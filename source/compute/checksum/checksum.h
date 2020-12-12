#pragma once

#include <functional>
#include <map>
#include <thread>

class checksum {
public:
	checksum(){}
	~checksum();

	void start(std::function<void()> progress_track,
		std::function<void()> display);
	void stop();
	void pause();

	bool is_running();
	double get_progress();
	bool is_completed();
	std::map<std::string, std::string> get_data();

	std::string file_path_;
private:
	static void crc_calcs(checksum* chksum,
		std::function<void()> progress_track,
		std::function<void()> display);

	std::map<std::string, std::string> data;
	double progress_ = 0.0;
	bool running_ = false;
	bool pause_ = false;
	bool stop_ = false;
	std::thread thread_;
};
