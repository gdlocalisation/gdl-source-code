#pragma once

// abusing unsafe functions
#define _CRT_SECURE_NO_WARNINGS
// system
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef _CRT_SECURE_NO_WARNINGS

using std::cout, std::endl, std::tuple, std::vector;

// cocos2d
#pragma warning(push, 0)
#include <cocos2d.h>
#include <cocos-ext.h>
#pragma warning(pop)

using namespace cocos2d;

// gd.h
#include <gd.h>
using namespace gd;

// minhook
#include <MinHook.h>

// utf8 lib
#include "utf8.h"

// json lib
#include <nlohmann/json.hpp>
using namespace nlohmann;

// defines
//#define GDL_INDEV
#define GDL_VERSION "1.0.0"