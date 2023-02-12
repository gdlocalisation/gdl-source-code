#pragma once
#include "includes.h"
#include "utils.hpp"
#include "BrownAlertDelegate.hpp"
#include "hooks.hpp"

namespace menu {
#define MENU_PROFILE_MAKE(name, tex, var, smallNick) 							\
	auto var = CCMenuItem::create(); 											\
	var->addChild(CCSprite::createWithSpriteFrameName(tex)); 					\
	auto text_profile_##var = CCLabelBMFont::create(name, "goldFont.fnt"); 		\
	text_profile_##var->setPositionY((smallNick) ? -23.5f : -23.0f); 			\
	text_profile_##var->limitLabelWidth(30.f, 0.5f, 0.35f);						\
	var->addChild(text_profile_##var);

	inline CCNode* createCreditsLabels(string text, const char* fntName) {
		auto ret = CCNode::create();

		float yPos = 0;
		for (auto& str : utils::splitString(text, '\n')) {
			auto lbl = CCLabelBMFont::create(str.c_str(), fntName);
			lbl->setPositionY(yPos);
			lbl->setAnchorPoint({ 0,0 });
			ret->addChild(lbl);
			yPos -= lbl->getScaledContentSize().height;
		}

		return ret;
	}

	class GDLMenu : public BrownAlertDelegate {
	private:
		CCLayer* m_pPage1;
		CCLayer* m_pPage2;
		int m_nPage;
		CCMenuItemSpriteExtra* m_pNextBtn;
		CCMenuItemSpriteExtra* m_pPrevBtn;

		GDLMenu();
		void switchToPage(int page);
		void keyBackClicked() override;
		virtual void setup() override;
		void openLink(CCObject*);
		void changePage(CCObject* pObj);

	public:
		static GDLMenu* create();
		void openLayer(CCObject*);
	};

	void main();
}