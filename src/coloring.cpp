#include "coloring.h"

namespace Coloring {
	std::string removeTags(std::string str, bool removeColors, bool removeDelay, bool removeInstant) {
		if (removeColors) {
			str = std::regex_replace(str, std::regex("<c.>"), ""); // no starting <cX>
			str = std::regex_replace(str, std::regex("</c>"), ""); // no ending </c>
		}
		if (removeDelay)
			str = std::regex_replace(str, std::regex("<d...>"), ""); // no <dXXX>
		if (removeInstant) {
			str = std::regex_replace(str, std::regex("<i>"), ""); // no starting <i>
			str = std::regex_replace(str, std::regex("</i>"), ""); // no ending </i>
		}

		return str;
	}

	void parseTags(std::string str, CCArray* letters) {
		str = replaceRusCharsWithASCII(str);

		// colors
		std::string modStr = removeTags(str, false);

        size_t pos = 0;
		while (true) {
			auto beginPos = modStr.find("<c", pos);
			if (beginPos == std::string::npos)
				break;

			auto tag = modStr.at(beginPos + 2);

            auto endPos = modStr.find("</c>", pos);

			ccColor3B col;

			// thx to Wylie https://github.com/Wyliemaster/GD-Decompiled/blob/main/GD/code/src/MultilineBitmapFont.cpp#L23
			switch (tag)
			{
			case 'b':
				col = { 0x4A, 0x52, 0xE1 };
				break;
			case 'g':
				col = { 0x40, 0xE3, 0x48 };
				break;
			case 'l':
				col = { 0x60, 0xAB, 0xEF };
				break;
			case 'j':
				col = { 0x32, 0xC8, 0xFF };
				break;
			case 'y':
				col = { 0xFF, 0xFF, 0x00 };
				break;
			case 'o':
				col = { 0xFF, 0xA5, 0x4B };
				break;
			case 'r':
				col = { 0xFF, 0x5A, 0x5A };
				break;
			case 'p':
				col = { 0xFF, 0x00, 0xFF };
				break;
			default:
				col = { 0xFF, 0x00, 0x00 };
			}

			for (auto i = beginPos; i < endPos - 4; i++) {
				if (i >= letters->count())
					break;

				dynamic_cast<CCSprite*>(letters->objectAtIndex(i))->setColor(col);
			}

			modStr.erase(modStr.begin() + beginPos, modStr.begin() + beginPos + 4); // remove the <cX>
			modStr.erase(modStr.begin() + endPos - 4, modStr.begin() + endPos); // remove the </c>

			pos = beginPos + 1;
		}
		
		// delay
		modStr = removeTags(str, true, false);

		pos = 0;
		while (true) {
            auto beginPos = modStr.find("<d", pos);
			if (beginPos == std::string::npos)
				break;

            auto delay = (float)(atoi(modStr.substr(beginPos + 2, 5).c_str())) / 100.0f;

			if (beginPos < letters->count())
				dynamic_cast<CCSprite*>(letters->objectAtIndex(beginPos))->m_nUnknown = *(int*)&delay;

			modStr.erase(modStr.begin() + beginPos, modStr.begin() + beginPos + 6);

			pos = beginPos + 1;
		}

		// instant
		modStr = removeTags(str, true, true, false);

		pos = 0;
		while (true) {
            auto beginPos = modStr.find("<i>", pos);
			if (beginPos == std::string::npos)
				break;

            auto endPos = modStr.find("</i>", pos);

			for (auto i = beginPos; i < endPos - 3; i++) {
				if (i >= letters->count())
					break;

				dynamic_cast<CCSprite*>(letters->objectAtIndex(i))->setVisible(true);
			}

			modStr.erase(modStr.begin() + beginPos, modStr.begin() + beginPos + 3); // remove the <i>
			modStr.erase(modStr.begin() + endPos - 3, modStr.begin() + endPos); // remote the </i>

			pos = beginPos + 1;
		}
	}
}