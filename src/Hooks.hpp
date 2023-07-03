#pragma once
#include "includes.h"
#include "utils.hpp"
#include "coloring.h"

namespace hooks {
    inline json langFile;
    inline json patchFile;
    inline json locationsFile;
    inline vector<string> strings;

	void initPatches();
	void main();
}