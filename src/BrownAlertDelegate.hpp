#pragma once
#include <includes.h>

class FLAlertLayer_ : public cocos2d::CCLayerColor {
public:
    cocos2d::CCMenu* m_buttonMenu;
    int m_controlConnected;
    FLAlertLayerProtocol* m_alertProtocol;
    cocos2d::CCNode* m_scene;
    bool m_reverseKeyBack;
    cocos2d::ccColor3B m_color;
    cocos2d::CCLayer* m_mainLayer;
    int m_ZOrder;
    bool m_noElasticity;
    cocos2d::ccColor3B m_color2;
    ButtonSprite* m_button1;
    ButtonSprite* m_button2;
    cocos2d::CCLayerColor* m_scrollingLayer;
    int m_joystickConnected;
    bool m_containsBorder;
    bool m_noAction;
};

class BrownAlertDelegate : public FLAlertLayer_ {
    protected:
        // layer size is stored here
        cocos2d::CCSize m_pLrSize;

        // override this to add your own
        // layer code
        virtual void setup() = 0;

        // dont override this one
        // the floats are width and height
        // the background sprite may be changed
        inline virtual bool init(float _w, float _h, const char* _spr = "GJ_square01.png") {
            auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
            this->m_pLrSize = cocos2d::CCSize { _w, _h };

            if (!this->initWithColor({ 0, 0, 0, 125 })) return false;
            this->m_mainLayer = cocos2d::CCLayer::create();
            this->addChild(this->m_mainLayer);

            auto bg = cocos2d::extension::CCScale9Sprite::create(_spr, { 0.0f, 0.0f, 80.0f, 80.0f });
            bg->setContentSize(this->m_pLrSize);
            bg->setPosition(winSize.width / 2, winSize.height / 2);
            this->m_mainLayer->addChild(bg);

            this->m_buttonMenu = cocos2d::CCMenu::create();
            this->m_mainLayer->addChild(this->m_buttonMenu);

            setup();

            // auto closeSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
            // closeSpr->setScale(.8f);

            // auto closeBtn = CCMenuItemSpriteExtra::create(
            //     closeSpr,
            //     this,
            //     (cocos2d::SEL_MenuHandler)&BrownAlertDelegate::onClose
            // );
            // closeBtn->setUserData(reinterpret_cast<void*>(this));

            // this->m_buttonMenu->addChild(closeBtn);

            // closeBtn->setPosition( - _w / 2, _h / 2 );

            this->setKeypadEnabled(true);
            this->setTouchEnabled(true);

            this->m_mainLayer->setScale(0.1f);
            this->m_mainLayer->runAction(
                CCEaseElasticOut::create(
                    CCScaleTo::create(0.5f, 1.0),
                    0.6f
                )
            );

            return true;
        }

        inline void onClose(cocos2d::CCObject*) {
            this->setKeyboardEnabled(false);
            this->removeFromParentAndCleanup(true);
        }
};