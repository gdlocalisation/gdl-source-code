#include "Menu.hpp"

namespace Menu {
	CCNode* createCreditsLabels(std::string text, const char* fntName, float scale) {
		auto ret = CCNode::create();

		float yPos = 0;
		for (auto& str : SplitString(text, '\n')) {
			auto lbl = CCLabelBMFont::create(str.c_str(), fntName);
			lbl->setScale(scale);
			lbl->setPositionY(yPos);
			lbl->setAnchorPoint({ 0,0 });
			ret->addChild(lbl);
			yPos -= lbl->getScaledContentSize().height;
		}

		return ret;
	}

#define MENU_PROFILE_MAKE(name, tex, var, smallNick) 							\
	auto var = CCMenuItem::create(); 											\
	var->addChild(CCSprite::createWithSpriteFrameName(tex)); 					\
	auto text_profile_##var = CCLabelBMFont::create(name, "goldFont.fnt"); 		\
	text_profile_##var->setPositionY((smallNick) ? -23.5f : -23.0f); 			\
	text_profile_##var->limitLabelWidth(30.f, 0.5f, 0.35f);						\
	var->addChild(text_profile_##var);

	class GDLMenu : public BrownAlertDelegate {
	protected:
		CCLayer* m_pPage1;
		CCLayer* m_pPage2;
		int m_nPage;
		CCMenuItemSpriteExtra* m_pNextBtn;
		CCMenuItemSpriteExtra* m_pPrevBtn;

		GDLMenu() {
			this->m_pPage1 = CCLayer::create();
			this->m_pPage2 = CCLayer::create();
			this->m_nPage = 1;
			this->m_pNextBtn = nullptr;
			this->m_pPrevBtn = nullptr;
		}

		void switchToPage(int page) {
			this->m_nPage = page;

			switch (page) {
			case 1:
				this->m_pPrevBtn->setEnabled(false);
				this->m_pPrevBtn->setVisible(false);

				this->m_pNextBtn->setEnabled(true);
				this->m_pNextBtn->setVisible(true);

				this->m_pPage1->setVisible(true);
				this->m_pPage2->setVisible(false);
				break;
			case 2:
				this->m_pPrevBtn->setEnabled(true);
				this->m_pPrevBtn->setVisible(true);

				this->m_pNextBtn->setEnabled(false);
				this->m_pNextBtn->setVisible(false);

				this->m_pPage1->setVisible(false);
				this->m_pPage2->setVisible(true);
				break;
			}
		}

		void changePage(CCObject* pObj) {
			this->switchToPage(this->m_nPage + (pObj->getTag() ? 1 : -1));
		}

		void openLink(CCObject*) {
			CCApplication::sharedApplication()->openURL("https://bit.ly/3K4SbF2");
		}

		virtual void setup() override {
			// title

			auto title = CCSprite::createWithSpriteFrameName("gdl_logo_text.png");
			title->setScale(1.1f);
			auto titlespr = CCMenuItemSprite::create(title, title, this, (SEL_MenuHandler)&GDLMenu::openLink);
			titlespr->runAction(
				CCRepeatForever::create(
					CCSequence::create(
						CCEaseInOut::create(CCScaleTo::create(1.5f, 1.2f), 2.0f),
						CCEaseInOut::create(CCScaleTo::create(1.5f, 1.1f), 2.0f),
						nullptr
					)
				)
			);

			titlespr->setPosition({ -10, 84 });
			this->m_buttonMenu->addChild(titlespr);

			// page 1

			auto a1 = CCLabelBMFont::create(u8"������������:", "goldFont.fnt");
			m_pPage1->addChild(a1);
			a1->setScale(0.8f);
			a1->setPositionY(61);

			MENU_PROFILE_MAKE("Jaan", "jaan.png", jaan_pr, false)
			MENU_PROFILE_MAKE("Mye", "mye.png", mye_pr, false)
			MENU_PROFILE_MAKE("Demi Sans", "demisans.png", demi_pr, true)
			MENU_PROFILE_MAKE("LukasRadon", "lukasradon.png", lukas_pr, true)
			MENU_PROFILE_MAKE(u8"�����", "guschin.png", gus_pr, false)

			auto row1 = CCMenu::create(jaan_pr, mye_pr, demi_pr, lukas_pr, gus_pr, nullptr);
			this->m_pPage1->addChild(row1);
			row1->alignItemsHorizontallyWithPadding(55);
			row1->setPosition({ 0, 26 });

			MENU_PROFILE_MAKE(u8"�������", "maximal.png", max_pr, false)
			MENU_PROFILE_MAKE("Raelc", "raelc.png", raelc_pr, false)
			MENU_PROFILE_MAKE("Pixelsuft", "noname.png", noname_pr, false)
			MENU_PROFILE_MAKE("Uulti", "uulti.png", uulti_pr, false)

			auto row2 = CCMenu::create(max_pr, raelc_pr, noname_pr, uulti_pr, nullptr);
			this->m_pPage1->addChild(row2);
			row2->alignItemsHorizontallyWithPadding(55);
			row2->setPosition({ 0, -30 });

			this->m_mainLayer->addChild(this->m_pPage1);
			m_pPage1->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2, 157 });

			// page 2

			auto a2 = CCLabelBMFont::create(u8"��������� �������������:", "goldFont.fnt");
			m_pPage2->addChild(a2);
			a2->setScale(0.7f);
			a2->setPositionY(63);

			const char* thanks1 =
				"- RobTop\n"
				"- QuasarKim\n"
				"- Kolyah35\n"
				"- Geode SDK\n"
				"- GD Programming\n"
				"- Andre\n"
				"- mat\n"
				"- HJFod\n"
				"- Ayrelia"
				;

			const char* thanks2 =
				"- alk1m123\n"
				"- Cvolton\n"
				"- Wylie\n"
				"- sleepyut\n"
				"- Log0\n"
				"- camila314\n"
				"- Eldyj\n"
				"- Rektor\n"
				u8"- �������� GDL"
				;

			auto ta1 = createCreditsLabels(thanks1, "goldFont.fnt", 0.6f);
			ta1->setPosition({ -130, 30 });
			auto ta2 = createCreditsLabels(thanks2, "goldFont.fnt", 0.6f);
			ta2->setPosition({ 40, 30 });
			this->m_pPage2->addChild(ta1);
			this->m_pPage2->addChild(ta2);

			this->m_pPage2->setPosition({ CCDirector::sharedDirector()->getWinSize().width / 2, 157 });
			this->m_mainLayer->addChild(this->m_pPage2);

			// btns

			auto _spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
			_spr->setFlipX(true);
			this->m_pNextBtn = CCMenuItemSpriteExtra::create(
				_spr,
				this,
				(SEL_MenuHandler)&GDLMenu::changePage
			);
			this->m_pNextBtn->setTag(1);
			this->m_pNextBtn->setPositionX(225);
			this->m_buttonMenu->addChild(this->m_pNextBtn);

			this->m_pPrevBtn = CCMenuItemSpriteExtra::create(
				CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
				this,
				(SEL_MenuHandler)&GDLMenu::changePage
			);
			this->m_pPrevBtn->setTag(0);
			this->m_pPrevBtn->setPositionX(-225);
			this->m_buttonMenu->addChild(this->m_pPrevBtn);

			// close btn

			auto closeBtnSpr = ButtonSprite::create("OK", 0, false, "goldFont.fnt", "GJ_button_01.png", 0, 1.);
			auto closeBtn = CCMenuItemSpriteExtra::create(
				dynamic_cast<CCSprite*>(closeBtnSpr),
				this,
				(SEL_MenuHandler)&GDLMenu::onClose
			);
			closeBtn->setUserObject(this);
			closeBtn->setPositionY(-95);
			this->m_buttonMenu->addChild(closeBtn);

			this->switchToPage(1);
		}

		void keyBackClicked() override {
			this->onClose(nullptr);
		}

	public:
		static GDLMenu* create() {
			auto pRet = new GDLMenu();

			if (pRet && pRet->init(400, 230)) {
				pRet->autorelease();
				return pRet;
			}

			CC_SAFE_DELETE(pRet);
			return nullptr;
		}

		void openLayer(CCObject* pObj) {
			CCDirector::sharedDirector()->getRunningScene()->addChild(GDLMenu::create(), 99999);
		}
	};

	bool loadedTextures = false;

	bool(__thiscall* LoadingLayer_init_o)(void* self, bool fromReload);
	bool __fastcall LoadingLayer_init_hk(void* self, void*, bool fromReload) {
		loadedTextures = false;

		return LoadingLayer_init_o(self, fromReload);
	}

	void(__thiscall* loadAssets_o)(void*);
	void __fastcall loadAssets_hk(void* self, void*) {
		if (!loadedTextures) {
			CCTextureCache::sharedTextureCache()->addImage(
				CCFileUtils::sharedFileUtils()->fullPathForFilename("GDL_Plist.png", false).c_str(), false
			);
			CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(
				CCFileUtils::sharedFileUtils()->fullPathForFilename("GDL_Plist.plist", false).c_str()
			);
			loadedTextures = true;
		}

		loadAssets_o(self);
	}

	void(__thiscall* customSetup_o)(void* self);
	void __fastcall customSetup_hk(GJDropDownLayer* self, void*) {
		customSetup_o(self);

		auto spr = cocos2d::CCSprite::createWithSpriteFrameName("gdl_icon.png");
		spr->setScale(1.25f);

		auto btn = CCMenuItemSpriteExtra::create(
			spr,
			self,
			(cocos2d::SEL_MenuHandler)&GDLMenu::openLayer
		);
		btn->setPosition({ 0, 0 });

		auto menu = CCMenu::create();
		menu->addChild(btn, 99999);
		menu->setPosition({ 30, 30 });
		self->m_pLayer->addChild(menu, 99999);
	}

	void main() {
		MH_CreateHook(
			(PVOID)(base + 0x18c080),
			LoadingLayer_init_hk,
			(PVOID*)&LoadingLayer_init_o
		);

		MH_CreateHook(
			(PVOID)(base + 0x18c8e0),
			loadAssets_hk,
			(PVOID*)&loadAssets_o
		);

		MH_CreateHook(
			(PVOID)(base + 0x1dd420),
			customSetup_hk,
			(PVOID*)&customSetup_o
		);
	}
}