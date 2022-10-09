#pragma once
#include <vector>
#include <string>

struct OperationInfo
{
	int amountOfLines = 0;
	std::vector<int> idsOfLines = { }; //argNumber = (amountOfLines + 3) - 1 //min size =  1 
	std::string fromFilePath = "";
	std::string toFilePath = "";
	int insertPosition = 0;
};
