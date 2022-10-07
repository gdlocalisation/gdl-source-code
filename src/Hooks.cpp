#include "Hooks.hpp"

namespace Hooks {
	json langFile;
	json patchFile;

	// MISC FUNCTIONS

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
				MessageBoxA(NULL, (string("Failed to parse \"").append(name).append("\"! Check the file for mistakes.")).c_str(), "GDL Error", NULL);
				exit(1);
				return nullptr;
			}
		}
		else
		{
			MessageBoxA(NULL, (string("Failed to load \"").append(name).append("\"!")).c_str(), "GDL Error", NULL);
			exit(1);
			return nullptr;
		}
	}

	static vector<string> strs;

	void initPatches() {
		int i = 0;

		strs.clear();

		for (const auto& pair : langFile.items()) {
			if (patchFile.find(pair.key()) == patchFile.end())
				continue;

			string value = string(pair.value());

			strs.push_back(value);

			auto addresses = patchFile[pair.key()];

			for (auto addr_string : addresses) {
				auto addr = std::stoul(string(addr_string), nullptr, 16) + 0xC00;

				static const char* a;
				a = strs[i].c_str();
				patch(addr, reinterpret_cast<const uint8_t*>(a));
			}
			i++;
		}
	}

	// HOOKS

	// bool CCKeyboardDispatcher::dispatchKeyboardMSG(enumKeyCodes key, bool);
	bool(__thiscall* dispatchKeyboardMSG_o)(void* self, enumKeyCodes key, bool down);
	bool __fastcall dispatchKeyboardMSG_hk(void* self, void*, enumKeyCodes key, bool down) {
		if (key == KEY_P && down) {
			// i have 0 idea why i have to run this code twice but it only works that way idk lol
			langFile = loadJson("ru_ru.json");
			patchFile = loadJson("gdl_patches.json");
			initPatches();
			initPatches();

			return true;
		}

		return dispatchKeyboardMSG_o(self, key, down);
	}

	// bool MenuLayer::init();
	bool(__thiscall* init_o)(CCLayer* self);
	bool __fastcall init_hk(CCLayer* self, void*) {
		if (!init_o(self)) return false;

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		CCLabelBMFont* text = CCLabelBMFont::create(string("GDL ").append(GDL_VERSION).c_str(), "goldFont.fnt");
		self->addChild(text);
		text->setScale(0.75);
		text->setPosition({ winSize.width / 2, 308.0 });

		return true;
	}

	// std::string::something
	void(__thiscall* assign_o)(void*, const char* ptr, int size);
	void __fastcall assign_hk(void* a, void*, const char* ptr, int size) {
		assign_o(a, ptr, strlen(ptr));
	}

	// void TextArea::setString(std::string);
	void(__thiscall* setString_o)(TextArea*, string);
	void __fastcall setString_hk(TextArea* self, void*, string str) {

		setString_o(self, "");

		auto noTagsStr = Coloring::removeTags(str);

		auto lines = splitByWidth(noTagsStr, self->m_width, self->m_fontFile.c_str());

		vector<string> letters(lines.size(), "a");
		setString_o(self, join_string(letters, "\n"));

		if (!self->m_label->getChildren()) return;

		CCArray* letterArray = CCArray::create();
		float height = 0;
		for (size_t i = 0; i < self->m_label->getChildren()->count();) {
			auto lbl = dynamic_cast<CCLabelBMFont*>(self->m_label->getChildren()->objectAtIndex(i));

			if (i < lines.size()) {				
				lbl->setString(lines[i].c_str());
				lbl->setPositionX(0);
				lbl->setAnchorPoint({ self->f2, lbl->getAnchorPoint().y });
				height += lbl->getScaledContentSize().height;

				CCARRAY_FOREACH_B_TYPE(lbl->getChildren(), letter, CCSprite) {
					letterArray->addObject(letter);
				}
				i++;
				continue;
			}

			lbl->removeFromParent();
		}

		self->setTextureRect({ 0, 0, self->getContentSize().width, height });
		self->setContentSize({ self->getContentSize().width, height });
		self->m_label->setPositionY(height - 6);

		self->m_label->m_pLetterArray->removeAllObjects();
		COPY_ARRAY(letterArray, self->m_label->m_pLetterArray);

		if (!self->m_disableColor)
			Coloring::parseTags(str, self->m_label->m_pLetterArray);
	}

	// bool ButtonSprite::init(char const* p0, int p1, int p2, float p3, bool p4, char const* p5, char const* p6, float p7);
	bool (__thiscall* ButtonSprite_init_o)(ButtonSprite* self, char const* p0, int p1, int p2, float p3, bool p4, char const* p5, char const* p6, float p7);
	bool __fastcall ButtonSprite_init_hk(ButtonSprite* self, void*, char const* p0, int p1, int p2, float p3, bool p4, char const* p5, char const* p6, float p7) {
		if (!ButtonSprite_init_o(self, p0, p1, p2, p3, p4, p5, p6, p7))
			return false;

		if (p7 == 0) {
			self->m_pBGSprite->setContentSize({ self->m_pBGSprite->getContentSize().width, 29.0f * p3});
		}

		return true;
	}

	// void CCApplication::openURL(const char* url);
	void (__thiscall* openURL_o)(void*, const char* url);
	void __fastcall openURL_hk(void* self, void*, const char* url) {
		// no switch for strings be like:
		if (string(url) == "http://robtopgames.com/blog/2017/02/01/geometry-dash-newgrounds")
			url = "https://www.gdlocalisation.gq/gd/blog/ru/#newgrounds_start";
		else if (string(url) == "http://www.boomlings.com/files/GJGuide.pdf")
			url = "https://www.gdlocalisation.gq/gd/gjguide/ru/gjguide_ru.pdf";
		else if (string(url) == "http://www.robtopgames.com/gd/faq")
			url = "https://www.gdlocalisation.gq/gd/blog/ru";

		openURL_o(self, url);
	}

	class AchievementBar : cocos2d::CCNodeRGBA
	{
	public:
		cocos2d::CCLayerColor* m_pBarLayer;
		float m_fUnk;
		cocos2d::CCScene* m_pScene;
		cocos2d::extension::CCScale9Sprite* m_pSquareSprite;
		cocos2d::extension::CCScale9Sprite* m_pIconBG;
		cocos2d::CCSprite* m_pAchGlow;
		cocos2d::CCSprite* m_pIcon;
		cocos2d::CCLabelBMFont* m_pAchTitle;
		TextArea* m_pAchDesc;
	};

	bool(__thiscall* AchievementBar_init_o)(void* self, const char* title, const char* desc, const char* iconSprite, bool quest);
	bool __fastcall AchievementBar_init_hk(AchievementBar* self, void*, const char* title, const char* desc, const char* iconSprite, bool quest) {
		auto ret = AchievementBar_init_o(self, title, desc, iconSprite, quest);
		
		self->m_pAchTitle->setPosition({ -45, 22 }); // idk it gets weirdly ofset
		self->m_pIcon->setPositionX(-110);
		self->m_pAchDesc->setPositionX(-45);
		self->m_pAchDesc->setAnchorPoint({ 0.0, self->m_pAchDesc->getAnchorPoint().y });

		return ret;
	}

	void main() {
		HMODULE cc_handle = GetModuleHandle(L"libcocos2d.dll");

		// i have 0 idea why i have to run this code twice but it only works that way idk lol
		langFile = loadJson("ru_ru.json");
		patchFile = loadJson("gdl_patches.json");
		initPatches();
		initPatches();

		MH_CreateHook(
			(PVOID)(base + 0xF840),
			assign_hk,
			(PVOID*)&assign_o
		);

		MH_CreateHook(
			(PVOID)(base + 0x1907b0),
			init_hk,
			(PVOID*)&init_o
		);

		MH_CreateHook(
			(PVOID)(base + 0x33480),
			setString_hk,
			(PVOID*)&setString_o
		);

		MH_CreateHook(
			(PVOID)(base + 0x138A0),
			ButtonSprite_init_hk,
			(PVOID*)&ButtonSprite_init_o
		);

		MH_CreateHook(
			(PVOID)((int)GetProcAddress(cc_handle, "?openURL@CCApplication@cocos2d@@UAEXPBD@Z")),
			openURL_hk,
			(PVOID*)&openURL_o
		);

		MH_CreateHook(
			(PVOID)(base + 0x3B200),
			AchievementBar_init_hk,
			(PVOID*)&AchievementBar_init_o
		);
		
#ifdef GDL_INDEV
		MH_CreateHook(
			(PVOID)((int)GetProcAddress(cc_handle, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z")),
			dispatchKeyboardMSG_hk,
			(PVOID*)&dispatchKeyboardMSG_o
		);
#endif // GDL_INDEV
	}
}