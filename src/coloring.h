#pragma once
#include <includes.h>
#include <regex>
#include <utils.hpp>

namespace Coloring {
	std::string removeTags(std::string str, bool removeColors = true, bool removeDelay = true, bool removeInstant = true);
	void parseTags(std::string str, CCArray* letters);
}