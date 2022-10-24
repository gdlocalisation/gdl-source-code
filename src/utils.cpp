#include "utils.hpp"

bool patch(uintptr_t address, const uint8_t* bytes) {
    return WriteProcessMemory(
        GetCurrentProcess(),
        reinterpret_cast<LPVOID>(base + address),
        &bytes,
        sizeof(bytes),
        nullptr
    );
}

bool isRusChar(uint32_t cp) {
    if ((0x410 <= cp && cp <= 0x44F) || cp == 0x401 || cp == 0x451)
        return true;

    return false;
}

vector<std::string> SplitString(const std::string& s, char separator) {
    std::string temp;
    vector<std::string> v;

    for (char i : s) {

        if (i == separator) {
            v.push_back(temp);
            temp = "";
        }
        else {
            temp.push_back(i);
        }

    }
    v.push_back(temp);

    return v;
}

std::string join_string(const vector<std::string>& strs, const char* delim) {
    std::string ret;

    for (size_t i = 0; i < strs.size(); i++) {
        auto s = strs[i];
        auto toAdd = (i != strs.size() - 1) ? (s + delim) : (s);
        ret += toAdd;
    }

    return ret;
}

std::string replaceRusCharsWithASCII(const std::string& str) {
    std::string ret;

    auto b = str.begin();
    auto e = str.end();
    while (b != e) {
        auto cp = utf8::next(b, e);

        if (isRusChar(cp))
            ret += "E"; // basically any 1-byte char
        else
            ret += (char)cp;
    }

    return ret;
}

vector<std::string> splitByWidth(const std::string& src, float width, const char* fontName) {
    vector<std::string> ret;

    auto lbl = CCLabelBMFont::create("", fontName);

    auto lines = SplitString(src, '\n');

    for (auto line : lines)
    {
    loop_begin:
        std::string current;
        auto b = line.begin();
        auto e = line.end();
        bool overflowed = false;
        while (b != e) {
            auto cp = utf8::next(b, e);

            auto c = (char)cp;

            if (isRusChar(cp))
                current += ruslettersnums[cp];
            else
                current += c;

            lbl->setString(current.c_str());
            if (lbl->getScaledContentSize().width > width) {
                overflowed = true;
                break;
            }
        }

        if (overflowed && std::string(current).find(' ') != std::string::npos) {
            auto words = SplitString(lbl->getString(), ' ');
            words.pop_back();
            auto a = join_string(words, " ") + " ";
            ret.push_back(a);
            line = line.erase(0, a.length());
            goto loop_begin;
        }
        else {
            ret.push_back(current + " ");
        }
    }

    return ret;
}