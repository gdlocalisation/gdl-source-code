#ifndef __ACHIEVEMENTBAR_H__
#define __ACHIEVEMENTBAR_H__

#include <gd.h>

namespace gd {
	#pragma runtime_checks("s", off)
	class GDH_DLL AchievementBar : public cocos2d::CCNodeRGBA {
	public:
		cocos2d::CCLayerColor* m_pBarLayer;
		float m_fUnk;
		cocos2d::CCScene* m_pScene;
		cocos2d::extension::CCScale9Sprite* m_pSquareSprite;
		cocos2d::extension::CCScale9Sprite* m_pIconBG;
		cocos2d::CCSprite* m_pAchGlow;
		cocos2d::CCSprite* m_pIcon;
		cocos2d::CCLabelBMFont* m_pAchTitle;
		cocos2d::CCSprite* m_pAchDesc;

		static AchievementBar* create(const char* title,
			const char* desc, const char* icon, bool quest) {
			auto pRet = reinterpret_cast<AchievementBar* (__fastcall*)(const char*,
				const char*, const char*, bool)>(
					base + 0x3B120
					)(title, desc, icon, quest);
			__asm add esp, 0x8
			return pRet;
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif