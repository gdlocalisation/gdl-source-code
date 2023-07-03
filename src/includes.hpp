#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <Windows.h>

using std::string, std::vector;

#pragma warning(push, 0)
#include <cocos2d.h>
#include <cocos-ext.h>
#pragma warning(pop)

using namespace cocos2d;

#include "gd.h/include/gd.h"
using namespace gd;

#include "minhook/include/MinHook.h"

#pragma warning(push, 0)
#include "json/json.hpp"
#pragma warning(pop)

using nlohmann::json;

#include <fmt/format.h>

// #define GDL_INDEV