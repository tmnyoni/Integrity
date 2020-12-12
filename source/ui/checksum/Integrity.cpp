#include <chrono>

#include "integrity.h"
#include "../../../resources/resource.h"


void checksum_window::on_run() {
	//disable(page_name_ + "/pause_");
	//disable(page_name_ + "/stop_");

	//set_timer("failsafe", 1000, [this]() {failsafe(); });
}

void checksum_window::on_stop() { stop(); }

//void checksum_window::failsafe() {
//	if (!chksum.is_running()) {
//		enable(page_name_ + "/start_");
//		disable(page_name_ + "/pause_");
//		disable(page_name_ + "/stop_");
//
//		allow_quit();
//	}
//}
//
//void checksum_window::select_file_handler() {
//	open_file_params params;
//	auto filepath = open_file(params);
//
//	std::string error;
//	set_editbox_text(page_name_ + "/file_path_", filepath, error);
//}
//
//void checksum_window::start_handler() {
//	std::string error;
//
//	chksum.file_path_.clear();
//	get_editbox_text(page_name_ + "/file_path_", chksum.file_path_, error);
//
//	if (chksum.file_path_.empty())
//		return;
//
//	prevent_quit();
//	disable(page_name_ + "/start_");
//	enable(page_name_ + "/pause_");
//	enable(page_name_ + "/stop_");
//
//	chksum.start([this]() { progress_track(); },
//		[this]() { on_display_results(); });
//
//}
//
//void checksum_window::pause_handler() {
//	if (chksum.is_running())
//		chksum.pause();
//
//	enable(page_name_ + "/start_");
//	disable(page_name_ + "/pause_");
//	enable(page_name_ + "/stop_");
//}
//
//void checksum_window::stop_handler() {
//	if (chksum.is_running())
//		chksum.stop();
//
//	update_status(0.0);
//
//	enable(page_name_ + "/start_");
//	disable(page_name_ + "/pause_");
//	disable(page_name_ + "/stop_");
//
//	allow_quit();
//}
//
//
//
//void checksum_window::update_status(double perc) {
//	std::string error;
//	set_progress_bar(page_name_ + "/progress_", perc, error);
//
//	if (perc < 100)
//		set_text(page_name_ + "/summary_", std::to_string(int(perc)) + " %", error);
//	else
//		set_text(page_name_ + "/summary_", "Complete", error);
//}	
//
//void checksum_window::progress_track() {
//	update_status(chksum.get_progress());
//
//	if (chksum.get_progress() >= 100.0) {
//		enable(page_name_ + "/start_");
//		disable(page_name_ + "/pause_");
//		disable(page_name_ + "/stop_");
//	}
//}


bool checksum_window::layout(page& persistant_page,
	page& home_page,
	std::string& error) {

	home_page.set_name(page_name_);
	set_width(size_.width);
	set_height(size_.height);
	set_min_width_and_height(width(), height());
	prevent_resizing();

	set_icons(app_icon,
		gui::caption_icon_png{ icon_16, icon_20, icon_24, icon_28, icon_32 });

	widgets::editbox file_path;
	file_path.alias = "file_path_";
	file_path.rect = { long(width() - 400) / 2, long(width() - 400) / 2 + 400, 20, 45 };
	home_page.add_editbox(file_path);

	widgets::image select_path;
	select_path.tight_fit = true;
	select_path.png_resource = select_path_;
	select_path.on_click = [this]() {/* select_file_handler(); */};
	select_path.rect.set_width(20);
	select_path.rect.set_height(20);
	tools::snap_to(file_path.rect, tools::snap::right, 2, select_path.rect);
	home_page.add_image(select_path);

	widgets::progress_bar progress;
	progress.alias = "progress_";
	progress.rect = file_path.rect;
	progress.rect.set_height(15);
	tools::snap_to(file_path.rect, tools::snap::bottom, 10, progress.rect);
	home_page.add_progress_bar(progress);

	widgets::text summary;
	summary.alias = "summary_";
	summary.text_value = "0.0 %";
	summary.alignment = widgets::text_alignment::center;
	summary.rect = file_path.rect;
	tools::snap_to(progress.rect, tools::snap::bottom, 10, summary.rect);
	home_page.add_text(summary);

	rect button_rect;
	button_rect.set_width(300);
	button_rect.set_height(30);
	tools::snap_to(summary.rect, tools::snap::bottom, 10, button_rect);

	widgets::button start;
	start.alias = "start_";
	start.caption = "start";
	start.on_click = [this]() {
		/*start_handler();*/
		std::map<std::string, std::string> rows;
		rows.insert(std::make_pair("size", "24"));
		rows.insert(std::make_pair("size", "2990"));
		on_display_results(rows);
	};
	start.rect.set_width(80);
	start.rect.set_height(30);
	tools::pos_rect(button_rect, start.rect, 0, 0);
	home_page.add_button(start);

	widgets::button pause;
	pause.alias = "pause_";
	pause.caption = "pause";
	pause.on_click = [this]() {/*pause_handler();*/ };
	pause.rect.set_width(80);
	pause.rect.set_height(30);
	tools::pos_rect(button_rect, pause.rect, 50, 0);
	home_page.add_button(pause);

	widgets::button stop;
	stop.alias = "stop_";
	stop.caption = "stop";
	stop.on_click = [this]() {/*stop_handler();*/ };
	stop.rect.set_width(80);
	stop.rect.set_height(30);
	tools::pos_rect(button_rect, stop.rect, 100, 0);
	home_page.add_button(stop);

	return true;
}

void checksum_window::on_display_results(const std::map<std::string, std::string>& data) {
	gui::page display_page("display_page_");

	if (!page_exists("diplay_page_")) {

		auto margin_ = 10;
		size dimens_ = { 30, 30 };

		widgets::image back_img;
		back_img.rect = { margin_, margin_ + dimens_.width, margin_, dimens_.height };
		back_img.alias = "back_btn_";
		back_img.png_resource = back_icon;
		back_img.tight_fit = true;
		back_img.on_click = [&]() { show_previous_page(); };
		display_page.add_image(back_img);

		std::vector<widgets::listview_row> rows;
		if (!data.empty()) 
			for (auto [attribute, value] : data) {
				widgets::listview_row row{ { {attribute, value} } };
				rows.push_back(row);
			}

		widgets::listview display_table;
		display_table.alias = "diplay_table_";
		display_table.border = false;
		display_table.font_size = 11;
		display_table.columns = {
			{"Atrribute", 280, widgets::listview_column_type::string_},
			{"Value",	190, widgets::listview_column_type::string_},
		};
		display_table.data = rows;
		display_table.rect = { 10, 480, 50, 250 };
		display_page.add_listview(display_table);

		add_page(display_page);
	}
	show_page("display_page_");
}