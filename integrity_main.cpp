#include "source/ui/checksum/integrity.h"

#pragma comment (linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

int main() {
	std::string error;
	checksum_window checksum_wind;
	if (!checksum_wind.run(error)) {
		return 1;
	}
}

