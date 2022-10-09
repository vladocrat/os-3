#pragma once

#include <string>
#include <vector>
#include <optional>

class Parser
{
public:
	static Parser* instance()
	{
		static Parser p;
		return &p;
	}

	std::optional<std::vector<std::string>> parse(const std::string&);

private:
	Parser() {};
	~Parser() {};
	Parser(const Parser&) = delete;
	Parser(Parser&&) = delete;
	Parser& operator=(const Parser&) = delete;
};