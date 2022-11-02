#pragma once
#include "includes.h"
#include <iomanip>

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

#define as reinterpret_cast

namespace utils {
    bool patch(const uintptr_t address, const uint8_t* bytes); // credits to hjfod
    bool isUnicodeChar(int cp);
    vector<string> splitString(string str, char separator);
    string joinStrings(vector<string> strings, const char* delim);
    string replaceUnicode(string str);
    vector<string> splitByWidth(string src, float width, string fontName);
    json loadJson(const char* name);
    void getUnicodeChar(unsigned int code, char chars[5]);

    template <typename T>
    T getChildByIndex(CCNode* parent, int index) {
        if (!parent || !parent->getChildrenCount())
            return nullptr;

        return as<T>(parent->getChildren()->objectAtIndex(index));
    }

    // logger
    class Logger : private std::streambuf, public std::ostream {
    public:
        Logger() : std::ostream(this) {
            std::tm tm;
            time_t now = time(0);
            localtime_s(&tm, &now);
            cout << "[" << std::put_time(&tm, "%H:%M:%S") << "] ";
        }

        ~Logger() {
            cout << endl;
        }

        friend inline Logger& operator<<(Logger& l, CCPoint p);

    private:
        int overflow(int c) override {
            std::cout.put(c);
            return 0;
        }
    };

    inline Logger& operator<<(Logger& l, CCPoint p) {
        l << "{" << p.x << "," << p.y << "}";
        return l;
    }
}

#define log utils::Logger()

