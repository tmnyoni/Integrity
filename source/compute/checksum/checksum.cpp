#include <filesystem>
#include <fstream>
#include <sstream>

#include "boost/crc.hpp"

#include "checksum.h"

checksum::~checksum(){
	if (thread_.joinable()) {
		thread_.join();
	}
}

void checksum::start(std::function<void()> progress_track,
	std::function<void()> display) {
	if (!running_) {
		stop_ = false;

		if (thread_.joinable())
			thread_.join();

		thread_ = std::thread(crc_calcs, this, progress_track, display);
	}
	else
		if (pause_)
			pause_ = false;
}

void checksum::stop() {
	if (running_)
		stop_ = true;

	if (thread_.joinable())
		thread_.join();
}

void checksum::pause() {
	if (running_)
		pause_ = true;
}

bool checksum::is_running() { return running_; }
double checksum::get_progress() { 
	return progress_;
}

bool checksum::is_completed(){
	auto is_complete = false;
	get_progress() >= 100.0  ? is_complete = true : is_complete = false;
	return is_complete;
}

std::map<std::string, std::string> checksum::get_data(){
	return data;
}

void checksum::crc_calcs(checksum* pchksum,
	std::function<void()> progress_track, 
	std::function<void()> display){
	if (!pchksum)
		return;

	checksum& chksum = *pchksum;
	chksum.running_ = true;

	std::filesystem::path file_path_(chksum.file_path_);
	auto temp_file_path = file_path_;
	if (!std::filesystem::is_directory(temp_file_path.remove_filename())) {
		return;
	}

	std::ifstream file_stream(file_path_, std::ios::binary);
	if (!file_stream) {
		return;
	}

	double previous = 0.0;
	bool stop_at_pause = false;

	constexpr std::streamsize buffer_size = 1024;
	const double filesize = file_size(file_path_);
	double processed_bytes = 0.0;
	boost::crc_32_type result;
	do{
		if (chksum.stop_)
			break;
		
		while (chksum.pause_) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (chksum.stop_) {
				stop_at_pause = true;
				break;
			}
		}

		if (stop_at_pause)
			break;

		//calculating the checksum.
		char buffer[buffer_size]; //temporary storage.
		file_stream.read(buffer, buffer_size);
		result.process_bytes(buffer, file_stream.gcount());
		
		//keep track of processing.
		processed_bytes += buffer_size;
		chksum.progress_ = 100 * double(processed_bytes) / double(filesize);

		if (chksum.progress_ - previous >= 1.0 || chksum.progress_ >= 100) {
			if (progress_track)
				progress_track();
			previous = chksum.progress_;
		}
	} while (file_stream);

	if (chksum.is_completed()) {
		chksum.data.clear();

		std::stringstream chksum_ss;
		chksum_ss << std::hex << std::uppercase << result.checksum();
		chksum.data.insert(std::make_pair("checksum", chksum_ss.str()));

		std::stringstream name_ss;
		name_ss << file_path_.filename();
		chksum.data.insert(std::make_pair("filename", name_ss.str()));
		
		//to-do: refactor the code.
		auto format_size = [&](auto size) {
			auto str = [](long size) {return std::to_string(size); };
			std::string formated_size;
			size > (1024 * 1024 ) ? formated_size = str(size / (1024 * 1024)) + "MB":
				formated_size = str(size / 1024) + "KB";
			return formated_size;
		};
		chksum.data.insert(std::make_pair("size", format_size(filesize)));
	}

	chksum.running_ = false;
	if (display) {
		display();
	}
}
