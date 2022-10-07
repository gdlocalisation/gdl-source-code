#pragma once
#include <includes.h>
#include <regex>
#include <utils.hpp>

namespace Coloring {
	string removeTags(string str, bool removeColors = true, bool removeDelay = true, bool removeInstant = true);
	void parseTags(string str, CCArray* letters);
}