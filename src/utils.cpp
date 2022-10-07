#include "utils.hpp"

bool patch(uintptr_t const address, const uint8_t* bytes) {
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

vector<string> SplitString(string s, char separator) {
    string temp = "";
    vector<string> v;

    for (size_t i = 0; i < s.length(); ++i) {

        if (s[i] == separator) {
            v.push_back(temp);
            temp = "";
        }
        else {
            temp.push_back(s[i]);
        }

    }
    v.push_back(temp);

    return v;
}

string join_string(vector<string> strs, const char* delim) {
    string ret;

    for (size_t i = 0; i < strs.size(); i++) {
        auto s = strs[i];
        string toAdd = (i != strs.size() - 1) ? (s + delim) : (s);
        ret += toAdd;
    }

    return ret;
}

string replaceRusCharsWithASCII(string str) {
    string ret = "";

    uint32_t cp = 0;
    auto b = str.begin();
    auto e = str.end();
    while (b != e) {
        cp = utf8::next(b, e);

        char c = cp;

        if (isRusChar(cp))
            ret += "E"; // basically any 1-byte char
        else
            ret += c;
    }

    return ret;
}

//string getGDLVersion() {
//    try {
//        // you can pass http::InternetProtocol::V6 to Request to make an IPv6 request
//        http::Request request{ "http://www.gdlocalisation.gq/gd/version" };
//
//        // send a get request
//        const auto response = request.send("GET");
//        return std::string{ response.body.begin(), response.body.end() };
//    }
//    catch (const std::exception& e)
//    {
//        log << "Uh oh: " << e.what();
//        return "-1";
//    }
//}

vector<string> splitByWidth(string src, float width, const char* fontName) {
    string str = src;
    vector<string> ret;

    auto lbl = CCLabelBMFont::create("", fontName);

    auto lines = SplitString(str, '\n');

    for (auto line : lines)
    {
    loop_begin:
        string current;
        uint32_t cp = 0;
        auto b = line.begin();
        auto e = line.end();
        bool overflowed = false;
        while (b != e) {
            cp = utf8::next(b, e);

            char c = cp;

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

        if (overflowed && string(current).find(' ') != string::npos) {
            auto words = SplitString(lbl->getString(), ' ');
            words.pop_back();
            //string a = join_string(words, " ");
            string a = join_string(words, " ") + " ";
            ret.push_back(a);
            //line = line.erase(0, a.length() + 1);
            line = line.erase(0, a.length());
            goto loop_begin;
        }
        else {
            ret.push_back(current + " ");
        }
    }

    //log << "- splitByWidth:";
    //for (auto a : ret) {
    //    log << "  - '" << a << "'";
    //}

    return ret;
}