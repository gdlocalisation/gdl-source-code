#include "hooks.hpp"

namespace hooks {
    json locationsFile;

    void initPatches() {
        static vector<string> strings;

        auto langFile = utils::loadJson("ru_ru.json");
        auto patchFile = utils::loadJson("gdl_patches.json");

        strings.clear();
        strings.reserve(langFile.size());

        for (const auto& pair : langFile.items()) {
            if (!patchFile.contains(pair.key()))
                continue;

            strings.push_back(pair.value());

            for (const auto addr : patchFile[pair.key()]) {
                const char* str = strings[strings.size() - 1].c_str();

                utils::patch(base + addr, (uint8_t*)&str, 4);
            }
        }
    }

    bool(__thiscall* dispatchKeyboardMSG_o)(void* self, enumKeyCodes key, bool down);
    bool __fastcall dispatchKeyboardMSG_hk(void* self, void*, enumKeyCodes key, bool down) {
        if (key == KEY_P && down) {
            locationsFile = utils::loadJson("ru_ru_locations.json");
            initPatches();

            return true;
        }

        return dispatchKeyboardMSG_o(self, key, down);
    }

    bool(__thiscall* MenuLayer_init_o)(CCLayer* self);
    bool __fastcall MenuLayer_init_hk(CCLayer* self, void*) {
        if (!MenuLayer_init_o(self))
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        CCLabelBMFont* text = CCLabelBMFont::create("GDL v1.1.2", "goldFont.fnt");
        self->addChild(text);
        text->setScale(0.75);
        text->setPosition({winSize.width / 2.f, winSize.height - 14.f});

        return true;
    }

    void(__thiscall* assign_o)(void*, const char* ptr, int size);
    void __fastcall assign_hk(void* a, void*, const char* ptr, int size) {
        // Why this is required:
        // In some places of gd you can see std::string::assign called with fixed length
        // which limits strings to the length it was in the game but the translated strings may be longer than the original ones.
        // It is also pretty difficult to patch the length as it is not used in all calls and i would somehow need to find places to patch
        // So it is easier to hook it and just set it to the string length instead of the fixed length
        assign_o(a, ptr, strlen(ptr));
    }

    // std::string::operator+(const char* ptr, int size);
    void(__thiscall* operator_plus_o)(void* self, const char* ptr, int size);
    void __fastcall operator_plus_hk(void* self, void*, const char* ptr, int size) {
        operator_plus_o(self, ptr, strlen(ptr));
    }

    void(__thiscall* setString_o)(TextArea*, string);
    void __fastcall setString_hk(TextArea* self, void*, string str) {
        // logD("setString \"{}\"", str);

        auto noTagsStr = coloring::removeTags(str);

        auto lines = utils::splitByWidth(noTagsStr, self->m_width, self->m_fontFile.c_str());

        if (lines.size() == 0)
            return;

        string linesGen(lines.size(), '\n');
        setString_o(self, linesGen);

        CCArray* letterArray = CCArray::create();
        CCARRAY_FOREACH_B_TYPE(self->m_label->getChildren(), lbl, CCLabelBMFont) {
            lbl->setString(lines[ix].c_str());
            lbl->setAnchorPoint({self->m_anchorPoint.x, lbl->getAnchorPoint().y});
            letterArray->addObjectsFromArray(lbl->getChildren());
        }

        self->m_label->m_pLetterArray->removeAllObjects();
        self->m_label->m_pLetterArray->addObjectsFromArray(letterArray);

        if (!self->m_disableColor)
            coloring::parseTags(str, letterArray);
    }

    void(__thiscall* openURL_o)(void*, const char* url);
    void __fastcall openURL_hk(void* self, void*, const char* url) {
        if (string(url) == "http://robtopgames.com/blog/2017/02/01/geometry-dash-newgrounds")
            url = "https://www.gdlocalisation.uk/gd/blog/ru/#newgrounds_start";
        else if (string(url) == "http://www.boomlings.com/files/GJGuide.pdf")
            url = "https://www.gdlocalisation.uk/gd/gjguide/ru/gjguide_ru.pdf";
        else if (string(url) == "http://www.robtopgames.com/gd/faq")
            url = "https://www.gdlocalisation.uk/gd/blog/ru";

        openURL_o(self, url);
    }

    bool(__thiscall* AchievementBar_init_o)(void* self, const char* title, const char* desc, const char* iconSprite, bool quest);
    bool __fastcall AchievementBar_init_hk(AchievementBar* self, void*, const char* title, const char* desc, const char* iconSprite,
                                           bool quest) {
        if (!AchievementBar_init_o(self, title, desc, iconSprite, quest))
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        self->m_pIcon->setPositionX(-110);

        self->m_pAchDesc->setAnchorPoint({0.0, self->m_pAchDesc->getAnchorPoint().y});
        self->m_pAchDesc->setPositionX(0);
        CCARRAY_FOREACH_B_TYPE(self->m_pAchDesc->getChildren(), lbl, CCLabelBMFont) {
            lbl->setAnchorPoint({0.0, lbl->getAnchorPoint().y});
            lbl->setPositionX(self->m_pAchDesc->convertToNodeSpaceAR({winSize.width / 2 - 75, 0.0}).x);
        }

        self->m_pAchTitle->setPosition({self->m_pAchDesc->getParent()->convertToNodeSpaceAR({winSize.width / 2 - 75, 0.0}).x, 22});

        return true;
    }

    bool(__thiscall* GauntletNode_init_o)(CCNode* self, GJMapPack* mapPack);
    bool __fastcall GauntletNode_init_hk(CCNode* self, void*, GJMapPack* mapPack) {
        if (!GauntletNode_init_o(self, mapPack))
            return false;

        auto packID = mapPack->m_nPackID;
        if (utils::shouldReverseGauntlet(packID)) {
            auto nameLabel = reinterpret_cast<CCLabelBMFont*>(self->getChildren()->objectAtIndex(3));
            auto nameShadow = reinterpret_cast<CCLabelBMFont*>(self->getChildren()->objectAtIndex(5));
            auto gauntletLabel = reinterpret_cast<CCLabelBMFont*>(self->getChildren()->objectAtIndex(4));
            auto gauntletShadow = reinterpret_cast<CCLabelBMFont*>(self->getChildren()->objectAtIndex(6));

            nameLabel->setPositionY(75);
            nameShadow->setPositionY(72);
            gauntletLabel->setPositionY(94);
            gauntletShadow->setPositionY(91);

            nameLabel->setScale(0.45f);
            nameShadow->setScale(0.45f);
            gauntletLabel->setScale(0.62f);
            gauntletShadow->setScale(0.62f);
        }

        return true;
    }

    void(__thiscall* CCNode_setPosition_o)(CCNode* self, CCPoint* p);
    void __fastcall CCNode_setPosition_hk(CCNode* self, void*, CCPoint* p) {
        auto lbl = dynamic_cast<CCLabelBMFont*>(self);

        if (!lbl)
            return CCNode_setPosition_o(self, p);

        if (locationsFile.contains(lbl->getString())) {
            auto entry = locationsFile[lbl->getString()];
            if (entry.contains("x"))
                p->x += entry["x"];
            if (entry.contains("y"))
                p->y += entry["y"];
        }

        CCNode_setPosition_o(self, p);
    }

    void(__thiscall* CCLabelBMFont_setScale_o)(CCLabelBMFont* self, float scale);
    void __fastcall CCLabelBMFont_setScale_hk(CCLabelBMFont* self, void*, float scale) {
        if (locationsFile.contains(self->getString())) {
            auto entry = locationsFile[self->getString()];
            if (entry.contains("scale"))
                scale = entry["scale"];
        }

        CCLabelBMFont_setScale_o(self, scale);
    }

    bool(__thiscall* GauntletLayer_init_o)(CCLayer* self, int gauntletType);
    bool __fastcall GauntletLayer_init_hk(CCLayer* self, void*, int gauntletType) {
        if (!GauntletLayer_init_o(self, gauntletType))
            return false;

        CCLabelBMFont* nameLabel = nullptr;
        CCLabelBMFont* shadowLabel = nullptr;

        // you cant do it with class members or just getChildren()->objectAtIndex()
        CCARRAY_FOREACH_B_TYPE(self->getChildren(), node, CCNode) {
            if (dynamic_cast<CCLabelBMFont*>(node)) {
                if (nameLabel == nullptr)
                    nameLabel = reinterpret_cast<CCLabelBMFont*>(node);
                else if (shadowLabel == nullptr)
                    shadowLabel = reinterpret_cast<CCLabelBMFont*>(node);
            }
        }

        auto gauntletName = utils::splitString(nameLabel->getString(), ' ')[0];

        auto newName = fmt::format(utils::shouldReverseGauntlet(gauntletType) ? "Остров {}" : "{} Остров", gauntletName);

        nameLabel->setString(newName.c_str());
        shadowLabel->setString(newName.c_str());

        return true;
    }

    bool(__thiscall* LevelLeaderboard_init_o)(CCNode* self, GJGameLevel* lvl, int type);
    bool __fastcall LevelLeaderboard_init_hk(CCNode* self, void*, GJGameLevel* lvl, int type) {
        if (!LevelLeaderboard_init_o(self, lvl, type))
            return false;

        CCLabelBMFont* lbl = nullptr;

        CCARRAY_FOREACH_B_TYPE(dynamic_cast<CCNode*>(self->getChildren()->objectAtIndex(0))->getChildren(), node, CCNode) {
            if (dynamic_cast<CCLabelBMFont*>(node)) {
                if (lbl == nullptr)
                    lbl = reinterpret_cast<CCLabelBMFont*>(node);
            }
        }

        lbl->setString(fmt::format("Таблица Лидеров для {}", lvl->m_sLevelName).c_str());

        return true;
    }

    // MENU HOOKS

    bool(__thiscall* LoadingLayer_init_o)(void* self, bool fromReload);
    bool __fastcall LoadingLayer_init_hk(void* self, void*, bool fromReload) {
        auto ret = LoadingLayer_init_o(self, fromReload);

        CCTextureCache::sharedTextureCache()->addImage(CCFileUtils::sharedFileUtils()->fullPathForFilename("GDL_Sheet.png", false).c_str(),
                                                       false);
        CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile(
            CCFileUtils::sharedFileUtils()->fullPathForFilename("GDL_Sheet.plist", false).c_str());

        return ret;
    }

    void(__thiscall* customSetup_o)(void* self);
    void __fastcall customSetup_hk(GJDropDownLayer* self, void*) {
        customSetup_o(self);

        auto spr = cocos2d::CCSprite::createWithSpriteFrameName("gdl_icon.png");
        spr->setScale(1.25f);

        auto btn = CCMenuItemSpriteExtra::create(spr, self, menu_selector(GDLMenu::openLayer));
        btn->setPosition({0, 0});

        auto menu = CCMenu::create();
        menu->addChild(btn, 99999);
        menu->setPosition({30, 30});
        self->m_pLayer->addChild(menu, 99999);
    }

    void main() {
        locationsFile = utils::loadJson("ru_ru_locations.json");

        // fix crash because of cyrillic Р, credits to sleepyut
        utils::patch(base + 0x293658, (uint8_t*)"\x01", 1);

        // Change AchievementCell's text width
        float achCellWidth = 230.f;
        utils::patch(base + 0x2E6660, (uint8_t*)&achCellWidth, 4);

        utils::hook(base + 0xF840, assign_hk, (void**)&assign_o);
        utils::hook(base + 0x156F0, operator_plus_hk, (void**)&operator_plus_o);
        utils::hook(base + 0x1907b0, MenuLayer_init_hk, (void**)&MenuLayer_init_o);
        utils::hook(base + 0x33480, setString_hk, (void**)&setString_o);
        utils::hookCC("?openURL@CCApplication@cocos2d@@UAEXPBD@Z", openURL_hk, (void**)&openURL_o);
        utils::hookCC("?setPosition@CCNode@cocos2d@@UAEXABVCCPoint@2@@Z", CCNode_setPosition_hk, (void**)&CCNode_setPosition_o);
        utils::hookCC("?setScale@CCLabelBMFont@cocos2d@@UAEXM@Z", CCLabelBMFont_setScale_hk, (void**)&CCLabelBMFont_setScale_o);
        utils::hook(base + 0x3B200, AchievementBar_init_hk, (void**)&AchievementBar_init_o);
        utils::hook(base + 0x106AE0, GauntletNode_init_hk, (void**)&GauntletNode_init_o);
        utils::hook(base + 0x102E70, GauntletLayer_init_hk, (void**)&GauntletLayer_init_o);
        utils::hook(base + 0x17C4F0, LevelLeaderboard_init_hk, (void**)&LevelLeaderboard_init_o);

        // MENU HOOKS

        utils::hook(base + 0x18C080, LoadingLayer_init_hk, (void**)&LoadingLayer_init_o);
        utils::hook(base + 0x1DD420, customSetup_hk, (void**)&customSetup_o);

#ifdef GDL_INDEV
        utils::hookCC("?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z", dispatchKeyboardMSG_hk,
                      (void**)&dispatchKeyboardMSG_o);
#endif // GDL_INDEV
    }
} // namespace hooks
