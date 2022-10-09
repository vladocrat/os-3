#include "parser.h"

#include <Windows.h>
#include <iostream>
#include <sstream>
#include "commands.h"

static const char DELIMETER = ' ';
static const int MIN_PARAMS = 6;

std::optional<std::vector<std::string>> Parser::parse(const std::string& command)
{
	std::vector<std::string> tokens;
	std::string temp = "";
	std::istringstream ss(command);

	while (ss >> temp)
	{
		tokens.push_back(temp);
	}
	
	if (tokens[0] == Commands::EXIT)
	{
		std::vector<std::string> str;
		str.push_back(tokens[0]);
		return str;
	}

	if (tokens[0] != Commands::APPEND) // [0] -> command name
	{
		//let the windows do its thing;
		STARTUPINFOA startInfo;
		PROCESS_INFORMATION processInfo;
		ZeroMemory(&startInfo, sizeof STARTUPINFOA);
		startInfo.cb = sizeof STARTUPINFOA;
		ZeroMemory(&processInfo, sizeof PROCESS_INFORMATION);
		startInfo.dwFlags = STARTF_USESTDHANDLES;
		startInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		startInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		SECURITY_ATTRIBUTES sAttrs;
		sAttrs.nLength = sizeof SECURITY_ATTRIBUTES;
		sAttrs.bInheritHandle = TRUE;
		sAttrs.lpSecurityDescriptor = NULL;
		char* commandStr = &tokens[0][0];

		auto process = CreateProcessA(NULL,
			commandStr,
			&sAttrs,
			NULL,
			true,
			NORMAL_PRIORITY_CLASS,
			NULL,
			NULL,
			&startInfo,
			&processInfo);

		if (!process)
		{
			std::cout << "ERROR::FAILED_TO_START_PROCESS" << std::endl;
			return { };
		}

		auto thread = processInfo.hProcess;
		WaitForSingleObject(thread, INFINITE);
		
		if (!CloseHandle(thread))
		{
			std::cout << "ERROR_FAILED_TO_CLOSE_HANDLE" << std::endl;
			return { };
		}

		return std::vector<std::string>{ Commands::WIN_COMMAND };
	}

	//append 1 1 file.txt newFile.txt 1
	if (tokens.size() < MIN_PARAMS)
	{
		return { };
	}

	if (atoi(tokens[1].c_str()) <= 0) // [1] -> amount of lines
	{
		return { };
	}

	if (atoi(tokens[tokens.size() - 1].c_str()) < 0) // [tokens.size - 1] -> insertPos
	{
		return { };
	}

	for (int i = 2; i < tokens.size() - 4; ++i) // ids start from [2] up to [tokens.size() - 4]
	{
		if (atoi(tokens[i].c_str()) <= 0)
		{
			return { };
		}
	}

	return tokens;
}