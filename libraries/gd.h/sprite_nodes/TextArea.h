#ifndef __TEXTAREA_H__
#define __TEXTAREA_H__

#include <gd.h>
//#include "../../src/includes.h"

namespace gd {
	#pragma runtime_checks("s", off)

	class GDH_DLL MultilineBitmapFont : public cocos2d::CCSprite
	{
	public:
		float m_fFontWidth[300];
		cocos2d::CCArray* m_pColouredTextArray;  //ColoredSection*
		cocos2d::CCArray* m_pInstantTextArray;   //InstantSection*
		cocos2d::CCArray* m_pDelayedTextArray;   //DelayedSection*
		cocos2d::CCArray* m_pLetterArray;	     //cocos2d::CCSprite*
		float m_fHalfScaledFontWidth;
		bool m_bUnkScaleBool;
		int unk;
		cocos2d::CCSize m_obTextureSize;
		cocos2d::CCPoint m_obPosition;
		float m_fContentSizeScaleMod;
		bool m_bTagsDisabled;

		static MultilineBitmapFont* createWithFont(
			const char* fontName, std::string str, float scale, float width, cocos2d::CCPoint anchorPoint, bool bColourEnabled
		) {
			__asm movss xmm1, scale;
			__asm movss xmm2, width;

			auto pRet = reinterpret_cast
				//<MultilineBitmapFont* (__vectorcall*)(const char*, float, float, float, float, bool, std::string, cocos2d::CCPoint)>
				<MultilineBitmapFont* (__vectorcall*)(const char*, bool, std::string, cocos2d::CCPoint)>
				(base + 0x2A5B0)
				(fontName, bColourEnabled, str, anchorPoint);

			__asm add esp, 0x24;

			return pRet;
		}
	};

	class GDH_DLL TextArea : public cocos2d::CCSprite {
	public:
		//PAD(0x58);

		/*bool m_disableColor;			// 0x1e4 // 0x1E4
		MultilineBitmapFont* m_label;	// 0x1e8 // 0x1E5
		float m_width;					// 0x1ec // 0x1E9
		int m_unknown;					// 0x1f0 // 0x1ED
		std::string m_fontFile;			// 0x1f4 // 
		float m_height;					// 0x20c
		PAD(0x8);
		cocos2d::CCPoint m_eAnchor;*/

		bool m_disableColor;			// 0x1e4 // 0x1E4
		MultilineBitmapFont* m_label;	// 0x1e8 // 0x1E5
		float m_width;					// 0x1ec // 0x1E9
		int m_unknown;					// 0x1f0 // 0x1ED
		std::string m_fontFile;			// 0x1f4 // 
		float m_height;					// 0x20c
		//float f1;
		PAD(0x4);
		cocos2d::CCPoint m_anchorPoint;

		//bool m_disableColor;
		//MultilineBitmapFont* m_label;
		//float m_width;
		//int m_unknown;
		//std::string m_fontFile;
		//float m_height;
		//cocos2d::CCPoint pad1;
		//cocos2d::CCPoint m_eAnchor;

	public:
		static TextArea* create(const char* font, bool disableColor,
			std::string caption, float scale, float width, float height,
			cocos2d::CCPoint const& anchorPos) {
			auto pRet = reinterpret_cast<TextArea* (__vectorcall*)(const char*,
				float, float, float, float,
				bool, std::string, cocos2d::CCPoint)>(
					base + 0x33270
				)(font, 0.f, scale, width, height, disableColor, caption, anchorPos);
			__asm add esp, 0x20
			return pRet;
		}

		void colorAllCharactersTo(cocos2d::ccColor3B color) {
			reinterpret_cast<void(__thiscall*)(
				TextArea*, cocos2d::ccColor3B
			)>(base + 0x33830)(this, color);
		}

		void setString(std::string const& str) {
			reinterpret_cast<void(__thiscall*)(
				TextArea*, std::string
			)>(base + 0x33480)(this, str);
		}
	};
	#pragma runtime_checks("s", restore)
}

#endif