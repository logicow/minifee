#include "Config.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

Config::Config()
{
	std::ifstream file("config.txt", std::ios::in | std::ios::binary | std::ios::ate);
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (size <= 0) {
		file = std::ifstream("../data/config.txt", std::ios::in | std::ios::binary | std::ios::ate);
		size = 0;
		if (file.seekg(0, std::ios::end).good()) size = file.tellg();
		if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

		if (size <= 0) {
			return;
		}
	}

	std::vector<char> buffer;
	buffer.resize((size_t)size);
	file.read((char*)(&buffer[0]), size);

	std::string fileString(&buffer[0], (unsigned int)size);
	
	bool done = false;
	int startLine = 0;
	while (!done) {
		int nextLine = fileString.find_first_of('\n', startLine);
		if (nextLine <= startLine) {
			nextLine = buffer.size() + 1;
			done = true;
		}
		std::string line = fileString.substr(startLine, nextLine - startLine - 1);
		std::transform(line.begin(), line.end(), line.begin(), ::tolower);
		parseLine(line);
		
		startLine = nextLine + 1;		
	}
}

double parseValue(std::string s)
{
	if (s == "true") {
		return 1;
	}
	else if (s == "false") {
		return 0;
	}

	double ipart = 0;
	double fpart = 0;
	double fpartdiv = 1;
	bool atipart = true;
	for (int i = 0; i < (int)s.size(); i++) {
		if (s[i] >= '0' && s[i] <= '9') {
			if (atipart) {
				ipart = ipart * 10 + s[i] - '0';
			}
			else {
				fpart = fpart * 10 + s[i] - '0';
				fpartdiv *= 10;
			}
		}
		else if (s[i] == ',' || s[i] == '.') {
			atipart = false;
		}
	}

	return ipart + fpart / fpartdiv;
}

void Config::parseLine(std::string line)
{
	int space = line.find_first_of(" \t=");
	if (space <= 0) {
		return;
	}
	int valueStart = line.find_first_not_of(" \t=", space);
	int valueEnd = line.find_first_of(" \t=", valueStart);
	if (valueEnd == valueStart) {
		valueEnd = line.length();
	}
	std::string name = line.substr(0, space);
	std::string valueString = line.substr(valueStart, valueEnd - valueStart);
	double value = parseValue(valueString);

	vars[name] = value;
}

