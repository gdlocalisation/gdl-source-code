#pragma once
#include "includes.h"

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

    #define log utils::Logger()

    inline vector<string> splitString(string str, char separator) {
        string temp = "";
        vector<string> v;

        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == separator) {
                v.push_back(temp);
                temp = "";
            }
            else {
                temp.push_back(str[i]);
            }

        }
        v.push_back(temp);

        return v;
    }

    inline string joinStrings(vector<string> strings, const char* delim) {
        string ret;

        for (size_t i = 0; i < strings.size(); i++) {
            auto str = strings[i];
            ret += (i != strings.size() - 1) ? str + delim : str;
        }

        return ret;
    }

    // https://en.wikipedia.org/wiki/UTF-8#Encoding
    inline size_t sequenceLength(uint8_t byte) {
        if      (byte >> 7 == 0b0)      return 1;
        else if (byte >> 5 == 0b110)    return 2;
        else if (byte >> 4 == 0b1110)   return 3;
        else if (byte >> 3 == 0b11110)  return 4;
        return 0;
    }

    inline string replaceUnicode(string str) {
        string ret = "";

        auto ptr = str.c_str();
        while (*ptr) {
            auto len = sequenceLength(*ptr);
            ret += len > 1 ? 'E' : *ptr; // it should be a 1-byte so it doesnt mess up with counting symbols
            ptr += len;
        }

        return ret;
    }

    inline vector<string> splitByWidth(string src, float width, string fontName) {
        string str = src;
        vector<string> ret;
        
        auto lines = splitString(str, '\n');
        
        for (auto line : lines) {
            auto lbl = CCLabelBMFont::create("", fontName.c_str());
            
            while (line.size()) {
                bool overflown = false;
                string current;

                const char* ptr = line.c_str();
                
                while (*ptr) {
                    auto len = sequenceLength(*ptr);
                    auto letter = string(ptr, len);
                    ptr += len;

                    current += letter;
                    
                    lbl->setString(current.c_str());
                    
                    if (lbl->getContentSize().width > width) {
                        overflown = true;
                        break;
                    }
                }
                
                if (overflown && current.find(' ') != string::npos) {
                    auto words = splitString(current, ' ');
                    words.pop_back();
                    current = joinStrings(words, " ") + " ";
                    ret.push_back(current);
                    line = line.erase(0, current.length());
                }
                else {
                    current += " ";
                    ret.push_back(current);
                    line = line.erase(0, current.length() - 1);
                }
            }
        }

        return ret;
    }

    inline json loadJson(const char* name) {
        std::ifstream translationFileStream(name);

        if (translationFileStream.is_open())
        {
            try {
                json translationObj;
                translationFileStream >> translationObj;

                return translationObj;
            }
            catch (...) {
                MessageBoxA(0, (string("Failed to parse \"").append(name).append("\"! Check the file for mistakes.")).c_str(), "GDL Error", 0);
                exit(1);
                return nullptr;
            }
        }
        else
        {
            MessageBoxA(0, (string("Failed to open \"").append(name).append("\"!")).c_str(), "GDL Error", 0);
            exit(1);
            return nullptr;
        }
    }

    template <typename T>
    inline T getChildByIndex(CCNode* parent, int index) {
        if (!parent || !parent->getChildrenCount())
            return nullptr;

        return as<T>(parent->getChildren()->objectAtIndex(index));
    }
}