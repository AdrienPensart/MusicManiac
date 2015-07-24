#include "YoutubeFetcher.hpp"

std::string exec(std::string cmd) {
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe) return "ERROR";
	char buffer[512];
	std::string result = "";
	while(!feof(pipe)) {
		if(fgets(buffer, sizeof(buffer), pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}
