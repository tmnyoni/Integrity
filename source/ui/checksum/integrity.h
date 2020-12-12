#pragma once
#include <string>
#include <chrono>

#include "liblec/cui/gui.h"

#include "../../../source/compute/checksum/checksum.h"

using namespace liblec::cui;

class checksum_window :
	public gui {
	
	//tracking time taken.
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;

	bool layout(page& persistant_page,
		page& home_page,
		std::string& error) override;

	void on_run() override;
	void on_shutdown() override {}
	void on_stop() override;
	void on_caption() override {}

	void failsafe();

	/*Calculating the checksum.*/
	void update_status(double perc);
	void progress_track();

	void select_file_handler();
	void start_handler();
	void pause_handler();
	void stop_handler();

	/*Displaying the results.*/
	void on_display_results(const std::map<std::string, std::string>& data);
private:
	const size size_{ 500, 300 };
	const std::string page_name_{ "Integrity" };

	checksum chksum;
};

