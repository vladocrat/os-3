#include <iostream>
#include <Windows.h>
#include <string>

#include "parser.h"
#include "filecontroller.h"
#include "operationinfo.h"
#include "commands.h"

#define parser Parser::instance()
#define fileController FileController::instance()

namespace Menu {

	void pauseAndClsOnInput()
	{
		system("pause");
		system("cls");
	}

	std::string getCommand()
	{
		std::string userInput = "";
		std::cout << "> ";
		
		while (true)
		{
			std::getline(std::cin, userInput);

			if (std::cin.fail())
			{
				std::cout << "ERROR::FAILED_TO_READ_INPUT" << std::endl;
				pauseAndClsOnInput();
				std::cin.clear();
				continue;
			}

			return userInput;
		}

		return userInput;
	}

} // Menu

namespace FlowControll {

	auto getCorrectInput()
	{
		do
		{
			auto tokens = parser->parse(Menu::getCommand());

			if (!tokens.has_value())
			{
				std::cout << "ERROR::FAILED_TO_PARSE_TOKENS" << std::endl;
				Menu::pauseAndClsOnInput();
				continue;
			}

			if (tokens.value()[0] == Commands::WIN_COMMAND)
			{
				continue;
			}

			if (tokens.value()[0] == Commands::EXIT)
			{
				exit(0);
			}

			return tokens;

		} while (true);
	}

} // FlowControll

int main(int argc, char* argv[])
{
	auto tokens = FlowControll::getCorrectInput();
	
	OperationInfo info;

	fileController->populateOperationInfo(info, tokens.value());

	if (!fileController->append(info))
	{
		std::cout << "ERROR::FAILED_TO_EXECUTE_FUNC" << std::endl;
		Menu::pauseAndClsOnInput();
	}
}