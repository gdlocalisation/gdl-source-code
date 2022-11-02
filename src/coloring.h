#pragma once
#include "includes.h"
#include "utils.hpp"
#include <regex>

namespace coloring {
	string removeTags(string str, bool removeColors = true, bool removeDelay = true, bool removeInstant = true);
	void parseTags(string str, CCArray* letters);
}