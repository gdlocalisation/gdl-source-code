#pragma once
#include "includes.hpp"

#define CCARRAY_FOREACH_B_BASE(__array__, __obj__, __type__, __index__)                                                                    \
    if (__array__ && __array__->count())                                                                                                   \
        for (auto [__index__, __obj__] = std::tuple<unsigned int, __type__> {0u, nullptr};                                                 \
             (__index__ < __array__->count() && (__obj__ = reinterpret_cast<__type__>(__array__->objectAtIndex(__index__)))); __index__++)

#define CCARRAY_FOREACH_B_TYPE(__array__, __obj__, __type__) CCARRAY_FOREACH_B_BASE(__array__, __obj__, __type__*, ix)

template <typename... Args>
void logD(Args... args) {
#ifdef GDL_INDEV
    std::cout << "[GDL] " << fmt::format(args...) << std::endl;
#endif
}

namespace utils {
    inline void patch(uintptr_t absAddr, uint8_t* bytes, size_t len) {
        DWORD old;
        VirtualProtect((void*)absAddr, len, PAGE_EXECUTE_READWRITE, &old);
        WriteProcessMemory(GetCurrentProcess(), (void*)absAddr, bytes, len, nullptr);
        VirtualProtect((void*)absAddr, len, old, nullptr);
    }

    inline vector<string> splitString(const string& str, char separator) {
        string temp = "";
        vector<string> v;

        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == separator) {
                v.push_back(temp);
                temp = "";
            } else {
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
        if (byte >> 7 == 0b0)
            return 1;
        else if (byte >> 5 == 0b110)
            return 2;
        else if (byte >> 4 == 0b1110)
            return 3;
        else if (byte >> 3 == 0b11110)
            return 4;
        return 0;
    }

    inline string replaceUnicode(const string& str) {
        string ret = "";

        auto ptr = str.c_str();
        while (*ptr) {
            auto len = sequenceLength(*ptr);
            ret += len > 1 ? 'E' : *ptr; // it should be a 1-byte so it doesnt mess up with counting symbols
            ptr += len;
        }

        return ret;
    }

    inline vector<string> splitByWidth(const string& src, float width, const char* fontName) {
        vector<string> ret;

        string str = src;

        while (str.size()) {
            auto lbl = CCLabelBMFont::create("", fontName);

            auto hasNL = str.find("\n") != string::npos;
            auto line = hasNL ? splitString(str, '\n')[0] : str;

            bool overflown = false;
            string current;

            auto ptr = line.c_str();

            while (*ptr) {
                auto len = sequenceLength(*ptr);
                auto letter = string(ptr, len);
                current += letter;
                ptr += len;

                lbl->setString(current.c_str());

                if (lbl->getScaledContentSize().width > width) {
                    overflown = true;
                    break;
                }
            }

            if (overflown) {
                if (current.find(' ') != string::npos) {
                    auto words = splitString(current, ' ');
                    words.pop_back();
                    current = joinStrings(words, " ");
                    current = joinStrings(words, " ") + " ";
                }
            } else if (hasNL) {
                current += " ";
            }

            ret.push_back(current);
            str = str.erase(0, current.size());
        }

        return ret;
    }

    inline json loadJson(const string& name) {
        std::ifstream translationFileStream(name);

        if (translationFileStream.is_open()) {
            try {
                json translationObj;
                translationFileStream >> translationObj;

                return translationObj;
            } catch (...) {
                MessageBoxA(0, fmt::format("Failed to parse json \"{}\". Please check the file for mistakes.", name).c_str(), "GDL Error",
                            0);
                exit(1);
                return nullptr;
            }
        } else {
            MessageBoxA(0, fmt::format("Failed to open json \"{}\".", name).c_str(), "GDL Error", 0);
            exit(1);
            return nullptr;
        }
    }

    inline bool shouldReverseGauntlet(int id) {
        return id == 4 ||  // Shadow
               id == 5 ||  // Lava
               id == 7 ||  // Chaos
               id == 9 ||  // Time
               id == 11 || // Magic
               id == 12 || // Spike
               id == 13 || // Monster
               id == 14 || // Doom
               id == 15;   // Death
    }

    inline void hook(uintptr_t absAddr, void* hk, void** tramp) {
        MH_CreateHook((void*)absAddr, hk, tramp);
    }

    inline void hookCC(const string& symbol, void* hk, void** tramp) {
        hook((uintptr_t)GetProcAddress(GetModuleHandle("libcocos2d.dll"), symbol.c_str()), hk, tramp);
    }
} // namespace utils
