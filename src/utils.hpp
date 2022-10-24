#pragma once
#include "includes.h"

// thx hjfod
#define CCARRAY_FOREACH_B_BASE(__array__, __obj__, __type__, __index__) \
    if (__array__ && __array__->count()) \
    for (auto [__index__, __obj__] = tuple<unsigned int, __type__> { 0u, nullptr }; \
        (__index__ < __array__->count() && (__obj__ = reinterpret_cast<__type__>(__array__->objectAtIndex(__index__)))); \
        __index__++)

#define CCARRAY_FOREACH_B_TYPE(__array__, __obj__, __type__) \
    CCARRAY_FOREACH_B_BASE(__array__, __obj__, __type__*, ix)

#define COPY_ARRAY(from, to) \
    CCARRAY_FOREACH_B_TYPE(from, obj, CCObject) { \
        to->addObject(obj); \
    }

// THX to HJFod fo that!
bool patch(uintptr_t const address, const uint8_t* bytes);

bool isRusChar(uint32_t cp);

vector<std::string> SplitString(std::string s, char separator);

std::string join_string(vector<std::string> strs, const char* delim);

std::string replaceRusCharsWithASCII(std::string str);

vector<std::string> splitByWidth(std::string src, float width, const char* fontName);

// generated with python =)
static std::map<uint32_t, std::string> ruslettersnums = {
    {1025, u8"\u0401"},
    {1105, u8"\u0451"},
    {1040, u8"\u0410"},
    {1041, u8"\u0411"},
    {1042, u8"\u0412"},
    {1043, u8"\u0413"},
    {1044, u8"\u0414"},
    {1045, u8"\u0415"},
    {1046, u8"\u0416"},
    {1047, u8"\u0417"},
    {1048, u8"\u0418"},
    {1049, u8"\u0419"},
    {1050, u8"\u041A"},
    {1051, u8"\u041B"},
    {1052, u8"\u041C"},
    {1053, u8"\u041D"},
    {1054, u8"\u041E"},
    {1055, u8"\u041F"},
    {1056, u8"\u0420"},
    {1057, u8"\u0421"},
    {1058, u8"\u0422"},
    {1059, u8"\u0423"},
    {1060, u8"\u0424"},
    {1061, u8"\u0425"},
    {1062, u8"\u0426"},
    {1063, u8"\u0427"},
    {1064, u8"\u0428"},
    {1065, u8"\u0429"},
    {1066, u8"\u042A"},
    {1067, u8"\u042B"},
    {1068, u8"\u042C"},
    {1069, u8"\u042D"},
    {1070, u8"\u042E"},
    {1071, u8"\u042F"},
    {1072, u8"\u0430"},
    {1073, u8"\u0431"},
    {1074, u8"\u0432"},
    {1075, u8"\u0433"},
    {1076, u8"\u0434"},
    {1077, u8"\u0435"},
    {1078, u8"\u0436"},
    {1079, u8"\u0437"},
    {1080, u8"\u0438"},
    {1081, u8"\u0439"},
    {1082, u8"\u043A"},
    {1083, u8"\u043B"},
    {1084, u8"\u043C"},
    {1085, u8"\u043D"},
    {1086, u8"\u043E"},
    {1087, u8"\u043F"},
    {1088, u8"\u0440"},
    {1089, u8"\u0441"},
    {1090, u8"\u0442"},
    {1091, u8"\u0443"},
    {1092, u8"\u0444"},
    {1093, u8"\u0445"},
    {1094, u8"\u0446"},
    {1095, u8"\u0447"},
    {1096, u8"\u0448"},
    {1097, u8"\u0449"},
    {1098, u8"\u044A"},
    {1099, u8"\u044B"},
    {1100, u8"\u044C"},
    {1101, u8"\u044D"},
    {1102, u8"\u044E"},
    {1103, u8"\u044F"},
};

// logger
class Logger : private std::streambuf, public std::ostream {
public:
    Logger() : std::ostream(this) {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] ";
    }

    ~Logger() {
        cout << endl;
    }

private:
    int overflow(int c) override {
        std::cout.put(c);
        return 0;
    }
};

#define log Logger()