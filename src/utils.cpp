#include "utils.hpp"

namespace utils {
    bool patch(uintptr_t const address, const uint8_t* bytes) {
        return WriteProcessMemory(
            GetCurrentProcess(),
            reinterpret_cast<LPVOID>(base + address),
            &bytes,
            sizeof(bytes),
            nullptr
        );
    }

    bool isUnicodeChar(int cp) {
        return isascii(cp) ? false : true;
    }

    vector<string> splitString(string str, char separator) {
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

    string joinStrings(vector<string> strings, const char* delim) {
        string ret;

        for (size_t i = 0; i < strings.size(); i++) {
            auto str = strings[i];
            ret += (i != strings.size() - 1) ? str + delim : str;
        }

        return ret;
    }

    string replaceUnicode(string str) {
        string ret = "";

        uint32_t cp = 0;
        auto b = str.begin();
        auto e = str.end();
        while (b != e) {
            cp = utf8::next(b, e);

            ret += isUnicodeChar(cp) ? 'E' : char(cp); // it should be a 1-byte so it doesnt mess up with counting symbols
        }

        return ret;
    }

    void getUnicodeChar(unsigned int code, char chars[5]) {
        if (code <= 0x7F) {
            chars[0] = (code & 0x7F); chars[1] = '\0';
        }
        else if (code <= 0x7FF) {
            // one continuation byte
            chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
            chars[0] = 0xC0 | (code & 0x1F); chars[2] = '\0';
        }
        else if (code <= 0xFFFF) {
            // two continuation bytes
            chars[2] = 0x80 | (code & 0x3F); code = (code >> 6);
            chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
            chars[0] = 0xE0 | (code & 0xF); chars[3] = '\0';
        }
        else if (code <= 0x10FFFF) {
            // three continuation bytes
            chars[3] = 0x80 | (code & 0x3F); code = (code >> 6);
            chars[2] = 0x80 | (code & 0x3F); code = (code >> 6);
            chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
            chars[0] = 0xF0 | (code & 0x7); chars[4] = '\0';
        }
        else {
            // unicode replacement character
            chars[2] = 0xEF; chars[1] = 0xBF; chars[0] = 0xBD;
            chars[3] = '\0';
        }
    }

    vector<string> splitByWidth(string src, float width, string fontName) {
        string str = src;
        vector<string> ret;

        auto lines = splitString(str, '\n');

        for (auto line : lines) {
            auto lbl = CCLabelBMFont::create("", fontName.c_str());            

            while (line.size()) {
                bool overflown = false;
                string current;

                int cp = 0;
                auto b = line.begin();
                auto e = line.end();
                while (b != e) {
                    cp = utf8::next(b, e);

                    char chars[5] = { "\0" };
                    getUnicodeChar(cp, chars);
                    current += chars;

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
                    line = line.erase(0, current.length());
                }
            }
        }

        //log << "- splitByWidth:";
        //for (auto a : ret) {
        //    log << "  - '" << a << "'";
        //}

        return ret;
    }

    json loadJson(const char* name)
    {
        std::ifstream translationFileStream(name);

        if (translationFileStream.is_open())
        {
            try {
                json translationObj;
                translationFileStream >> translationObj;

                return translationObj;
            }
            catch (...) {
                MessageBoxA(nullptr, (string("Failed to parse \"").append(name).append("\"! Check the file for mistakes.")).c_str(), "GDL Error", 0);
                exit(1);
                return nullptr;
            }
        }
        else
        {
            MessageBoxA(nullptr, (string("Failed to open \"").append(name).append("\"!")).c_str(), "GDL Error", 0);
            exit(1);
            return nullptr;
        }
    }
}