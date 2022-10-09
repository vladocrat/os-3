#include "filecontroller.h"

#include <iostream>
#include <codecvt>
#include <Windows.h>
#include <sstream>
#include <fstream>
#include <string>
#include <istream>

namespace Utils {

	std::string getLastErrorAsString()
	{
		auto errorMessage = GetLastError();

		if (errorMessage == 0)
		{
			return "NO ERROR CAUGHT";
		}

		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorMessage,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&messageBuffer,
			0,
			NULL);

		std::string message(messageBuffer, size);

		if (LocalFree(messageBuffer) != NULL)
		{
			std::cout << "ERROR::FAILED_TO_FREE_STRING_BUFFER" << std::endl;
		}

		return message;
	}

	std::wstring wstringFromString(const std::string& str)
	{
        std::wstring ws(str.begin(), str.end());
		return ws;
	}

	void closeFile(HANDLE h)
	{
		if (!CloseHandle(h)) {
			std::cout << getLastErrorAsString() << std::endl;
		}
	}

	std::streampos getLinePointer(const std::string& path, int lineId)
	{
		std::ifstream fileStream(path);

		//get to required line
		std::string line = "";

		for (int i = 0; i < lineId; ++i)
		{
			std::getline(fileStream, line);
		}

		auto ret = fileStream.tellg();
		fileStream.close();

		return ret;
	}

	void getLine(const std::string& path, const std::streampos& pos, std::string& line)
	{
		std::ifstream stream(path);
		stream.seekg(pos);
		std::getline(stream, line);
		stream.close();
	}

} // Utils

void FileController::populateOperationInfo(OperationInfo& info, const std::vector<std::string>& tokens)
{
	const int size = tokens.size();

	info.amountOfLines = atoi(tokens[1].c_str());
	
	for (int i = 2; i < size - 4; ++i)	
	{ 
		info.idsOfLines.push_back(atoi(tokens[i].c_str()));
	}

	info.fromFilePath = tokens[size - 3];
	info.toFilePath = tokens[size - 2];
;	info.insertPosition = atoi(tokens[size - 1].c_str());
}

bool FileController::append(const OperationInfo& info)
{
    auto wstrfromFilePath = Utils::wstringFromString(info.fromFilePath);
    LPCWSTR fromFilePath = wstrfromFilePath.c_str();

    auto file = CreateFile(fromFilePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (file == INVALID_HANDLE_VALUE)
    {
        std::cout << Utils::getLastErrorAsString() << std::endl;
        return false;
    }

    wstrfromFilePath = Utils::wstringFromString(info.toFilePath);
    LPCWSTR  writeFilePath = wstrfromFilePath.c_str();

    auto writeFile = CreateFile(writeFilePath,
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        0,
        NULL);

    if (writeFile == INVALID_HANDLE_VALUE)
    {
        std::cout << Utils::getLastErrorAsString() << std::endl;
        return false;
    }

    std::string line = "";
    int correctPosForWrite = info.insertPosition;

    for (int i = 0; i < info.amountOfLines; ++i)
    {
        correctPosForWrite += line.length();
        auto pos = Utils::getLinePointer(info.fromFilePath, i);

        SetFilePointer(file, pos,
            NULL, FILE_BEGIN);

        Utils::getLine(info.fromFilePath, pos, line);
        auto buffer = new LPVOID[line.length() + 1];
        DWORD bytesRead = -1;

        if (!ReadFile(file, buffer,
            line.length(),
            &bytesRead, NULL))
        {
            std::cout << Utils::getLastErrorAsString() << std::endl;
            return false;
        }

        SetFilePointer(writeFile, correctPosForWrite,
            NULL, FILE_CURRENT);

        DWORD bytesWritten = -1;

        if (!WriteFile(writeFile, buffer,
            line.length(),
            &bytesWritten, NULL))
        {
            std::cout << Utils::getLastErrorAsString() << std::endl;
            return false;
        }
    }

    if (!SetFileAttributes(writeFilePath,
        FILE_ATTRIBUTE_READONLY |
        FILE_ATTRIBUTE_HIDDEN |
        FILE_ATTRIBUTE_SYSTEM))
    {
        std::cout << Utils::getLastErrorAsString() << std::endl;
    }

    Utils::closeFile(file);
    Utils::closeFile(writeFile);

    return true;
}