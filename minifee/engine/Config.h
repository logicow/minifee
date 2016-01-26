#pragma once
#include <map>

class Config
{
public:
	std::map<std::string, double> vars;
	Config();
private:
	void parseLine(std::string line);
};