#pragma once

#include <vector>
#include <string>

#include "operationinfo.h"

class FileController
{
public:
	static FileController* instance()
	{
		static FileController fc;
		return &fc;
	}

	void populateOperationInfo(OperationInfo& ,const std::vector<std::string>& tokens);
	[[nodiscard]] bool append(const OperationInfo&);

private:
	FileController() {};
	~FileController() {};
	FileController(const FileController&) = delete;
	FileController(FileController&&) = delete;
	FileController& operator=(const FileController&) = delete;
};