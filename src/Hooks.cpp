#include "hooks.hpp"

namespace hooks {
    void initPatches() {
        strings.clear();

        for (auto& pair : hooks::langFile.items()) {
            if (!hooks::patchFile.contains(pair.key()))
                continue;

            strings.push_back(pair.value());

            for (auto& addrStr : hooks::patchFile[pair.key()]) {
                auto addr = std::stoul(string(addrStr), nullptr, 16) + 0xC00;

                const char* str = strings[strings.size() - 1].c_str();

                DWORD old;
                VirtualProtect((void*)(base + addr), 4, PAGE_EXECUTE_READWRITE, &old);
                WriteProcessMemory(
                    GetCurrentProcess(),
                    (void*)(base + addr),
                    &str,
                    4,
                    nullptr
                );
                VirtualProtect((void*)(base + addr), 4, old, nullptr);
            }
        }
    }

    bool(__thiscall* dispatchKeyboardMSG_o)(void* self, enumKeyCodes key, bool down);
    bool __fastcall dispatchKeyboardMSG_hk(void* self, void*, enumKeyCodes key, bool down) {
        if (key == KEY_P && down) {
            langFile = utils::loadJson("ru_ru.json");
            patchFile = utils::loadJson("gdl_patches.json");
            locationsFile = utils::loadJson("ru_ru_locations.json");
            // im going to return to this later
            initPatches();
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

        CCLabelBMFont* text = CCLabelBMFont::create(string("GDL ").append(GDL_VERSION).c_str(), "goldFont.fnt");
        self->addChild(text);
        text->setScale(0.75);
        text->setPosition({ winSize.width / 2, 308.0 });

        return true;
    }

    void(__thiscall* assign_o)(void*, const char* ptr, int size);
    void __fastcall assign_hk(void* a, void*, const char* ptr, int size) {
        // Why this is required:
        // In some places of gd code you can see std::string::assign called with fixed length
        // Which limits strings to the length it was in the game but the translated strings may be longer than the original ones
        // It is also pretty difficult to patch the length as it is not called everywhere and i would somehow need to find places to patch
        // So it is easier to hook it and just set it to the string length instead of the fixed length
        assign_o(a, ptr, strlen(ptr));
    }

    void(__thiscall* setString_o)(TextArea*, string);
    void __fastcall setString_hk(TextArea* self, void*, string str) {
        setString_o(self, ""); // to make self->m_label != nullptr
        
        auto noTagsStr = coloring::removeTags(str);
        
        auto lines = utils::splitByWidth(noTagsStr, self->m_width, self->m_fontFile.c_str());
        
        vector<string> letters(lines.size(), "a");
        setString_o(self, utils::joinStrings(letters, "\n")); // to have enough lines for the text to fit
        
        if (!self->m_label->getChildren()) return;
        
        CCArray* letterArray = CCArray::create();
        float height = 0;
        for (size_t i = 0; i < self->m_label->getChildren()->count();) { // this is because if i remove an item from the array and
                                                                         // increase i, it may go out of the bounds of the array
                                                                         // and to remove item [i+1] i will basically need to remove
                                                                         // item [i] and not increase i for that
            auto lbl = dynamic_cast<CCLabelBMFont*>(self->m_label->getChildren()->objectAtIndex(i));

            if (i < lines.size()) {                
                lbl->setString(lines[i].c_str());
                lbl->setPositionX(0);
                lbl->setAnchorPoint({ self->m_anchorPoint.x, lbl->getAnchorPoint().y });
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
        COPY_ARRAY(letterArray, self->m_label->m_pLetterArray); // if i just set 'self->m_label->m_pLetterArray' to 'letterArray' it will
                                                                // crash bc 'letterArray' will get destroyed after the function ends
                                                                
        if (!self->m_disableColor)
            coloring::parseTags(str, self->m_label->m_pLetterArray);
    }

    void (__thiscall* openURL_o)(void*, const char* url);
    void __fastcall openURL_hk(void* self, void*, const char* url) {
        if (string(url) == "http://robtopgames.com/blog/2017/02/01/geometry-dash-newgrounds")
            url = "https://www.gdlocalisation.gq/gd/blog/ru/#newgrounds_start";
        else if (string(url) == "http://www.boomlings.com/files/GJGuide.pdf")
            url = "https://www.gdlocalisation.gq/gd/gjguide/ru/gjguide_ru.pdf";
        else if (string(url) == "http://www.robtopgames.com/gd/faq")
            url = "https://www.gdlocalisation.gq/gd/blog/ru";

        openURL_o(self, url);
    }

    bool(__thiscall* AchievementBar_init_o)(void* self, const char* title, const char* desc, const char* iconSprite, bool quest);
    bool __fastcall AchievementBar_init_hk(AchievementBar* self, void*, const char* title, const char* desc, const char* iconSprite, bool quest) {
        if (!AchievementBar_init_o(self, title, desc, iconSprite, quest))
            return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        
        self->m_pIcon->setPositionX(-110);

        self->m_pAchDesc->setAnchorPoint({ 0.0, self->m_pAchDesc->getAnchorPoint().y });
        self->m_pAchDesc->setPositionX(0);
        CCARRAY_FOREACH_B_TYPE(self->m_pAchDesc->getChildren(), lbl, CCLabelBMFont) {
            lbl->setAnchorPoint({ 0.0, lbl->getAnchorPoint().y });
            lbl->setPositionX(self->m_pAchDesc->convertToNodeSpaceAR({ winSize.width / 2 - 75, 0.0 }).x);
        }

        self->m_pAchTitle->setPosition({ self->m_pAchDesc->getParent()->convertToNodeSpaceAR({ winSize.width / 2 - 75, 0.0 }).x, 22 });

        return true;
    }

    bool(__thiscall* GauntletNode_init_o)(CCNode* self, GJMapPack* mapPack);
    bool __fastcall GauntletNode_init_hk(CCNode* self, void*, GJMapPack* mapPack) {
        if (!GauntletNode_init_o(self, mapPack))
            return false;

        auto packID = mapPack->m_nPackID;
        if (packID == 4 || // Shadow
            packID == 5 || // Lava
            packID == 7 || // Chaos
            packID == 9 || // Time
            packID == 11 || // Magic
            packID == 12 || // Spike
            packID == 13 || // Monster
            packID == 14 || // Doom
            packID == 15) // Death
        {
            auto nameLabel = as<CCLabelBMFont*>(self->getChildren()->objectAtIndex(3));
            auto nameShadow = as<CCLabelBMFont*>(self->getChildren()->objectAtIndex(5));
            auto gauntletLabel = as<CCLabelBMFont*>(self->getChildren()->objectAtIndex(4));
            auto gauntletShadow = as<CCLabelBMFont*>(self->getChildren()->objectAtIndex(6));

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

    // unfortunately there is no CCLabelBMFont::setPosition
    void(__thiscall* CCNode_setPosition_o)(CCNode* self, CCPoint* p);
    void __fastcall CCNode_setPosition_hk(CCNode* self, void*, CCPoint* p) {
        auto lbl = dynamic_cast<CCLabelBMFont*>(self);

        if (!lbl)
            return CCNode_setPosition_o(self, p);

        if (locationsFile.contains(lbl->getString())) {
            auto entry = locationsFile[lbl->getString()];
            if (entry.contains("x")) *p = *p + ccp(entry["x"], 0);
            if (entry.contains("y")) *p = *p + ccp(0, entry["y"]);
        }

        CCNode_setPosition_o(self, p);
    }

    void(__thiscall* CCLabelBMFont_setScale_o)(CCLabelBMFont* self, float scale);
    void __fastcall CCLabelBMFont_setScale_hk(CCLabelBMFont* self, void*, float scale) {
        if (locationsFile.contains(self->getString())) {
            auto entry = locationsFile[self->getString()];
            if (entry.contains("scale")) scale = entry["scale"];
        }

        CCLabelBMFont_setScale_o(self, scale);
    }

    bool(__thiscall* GauntletLayer_init_o)(CCLayer* self, int gauntletType);
    bool __fastcall GauntletLayer_init_hk(CCLayer* self, void*, int gauntletType) {
        if (!GauntletLayer_init_o(self, gauntletType)) return false;

        CCLabelBMFont* nameLabel = nullptr;
        CCLabelBMFont* shadowLabel = nullptr;

        // you cant do it with class members or just getChildren()->objectAtIndex()
        CCARRAY_FOREACH_B_TYPE(self->getChildren(), node, CCNode) {
            if (dynamic_cast<CCLabelBMFont*>(node)) {
                if (nameLabel == nullptr)
                    nameLabel = as<CCLabelBMFont*>(node);
                else if (shadowLabel == nullptr)
                    shadowLabel = as<CCLabelBMFont*>(node);
            }
        }
        auto gauntletName = utils::splitString(nameLabel->getString(), ' ')[0];

        if (gauntletType == 4 || // Shadow
            gauntletType == 5 || // Lava
            gauntletType == 7 || // Chaos
            gauntletType == 9 || // Time
            gauntletType == 11 || // Magic
            gauntletType == 12 || // Spike
            gauntletType == 13 || // Monster
            gauntletType == 14 || // Doom
            gauntletType == 15) // Death
        {
            nameLabel->setString(string("Остров ").append(gauntletName).c_str());
            shadowLabel->setString(string("Остров ").append(gauntletName).c_str());
        }
        else {
            nameLabel->setString(gauntletName.append(" Остров").c_str());
            shadowLabel->setString(gauntletName.c_str());
        }

        return true;
    }

    bool(__thiscall* LevelLeaderboard_init_o)(CCNode* self, GJGameLevel* lvl, int type);
    bool __fastcall LevelLeaderboard_init_hk(CCNode* self, void*, GJGameLevel* lvl, int type) {
        if (!LevelLeaderboard_init_o(self, lvl, type)) return false;

        CCLabelBMFont* lbl = nullptr;

        CCARRAY_FOREACH_B_TYPE(utils::getChildByIndex<CCNode*>(self, 0)->getChildren(), node, CCNode) {
            if (dynamic_cast<CCLabelBMFont*>(node)) {
                if (lbl == nullptr)
                    lbl = as<CCLabelBMFont*>(node);
            }
        }

        lbl->setString(string("Таблица Лидеров ").append(lvl->m_sLevelName).c_str());

        return true;
    }

    void main() {
        langFile = utils::loadJson("ru_ru.json");
        patchFile = utils::loadJson("gdl_patches.json");
        locationsFile = utils::loadJson("ru_ru_locations.json");

        // im going to return to this later
        initPatches();
        initPatches();

        // fix breaking cyrillic Р, credits to sleepyut
        DWORD old;
        VirtualProtect((void*)(base + 0x293658), 1, PAGE_EXECUTE_READWRITE, &old);
        WriteProcessMemory(GetCurrentProcess(), (void*)(base + 0x293658), "\x01", 1, nullptr);
        VirtualProtect((void*)(base + 0x293658), 1, old, nullptr);
        
        MH_CreateHook(
            (PVOID)(base + 0xF840),
            assign_hk,
            (PVOID*)&assign_o
        );

        MH_CreateHook(
            (PVOID)(base + 0x1907b0),
            MenuLayer_init_hk,
            (PVOID*)&MenuLayer_init_o
        );

        MH_CreateHook(
            (PVOID)(base + 0x33480),
            setString_hk,
            (PVOID*)&setString_o
        );

        MH_CreateHook(
            (PVOID)((int)GetProcAddress(cocosModule, "?openURL@CCApplication@cocos2d@@UAEXPBD@Z")),
            openURL_hk,
            (PVOID*)&openURL_o
        );

        MH_CreateHook(
            (PVOID)((int)GetProcAddress(cocosModule, "?setPosition@CCNode@cocos2d@@UAEXABVCCPoint@2@@Z")),
            CCNode_setPosition_hk,
            (PVOID*)&CCNode_setPosition_o
        );

        MH_CreateHook(
            (PVOID)((int)GetProcAddress(cocosModule, "?setScale@CCLabelBMFont@cocos2d@@UAEXM@Z")),
            CCLabelBMFont_setScale_hk,
            (PVOID*)&CCLabelBMFont_setScale_o
        );

        MH_CreateHook(
            (PVOID)(base + 0x3B200),
            AchievementBar_init_hk,
            (PVOID*)&AchievementBar_init_o
        );

        MH_CreateHook(
            (PVOID)(base + 0x106AE0),
            GauntletNode_init_hk,
            (PVOID*)&GauntletNode_init_o
        );

        MH_CreateHook(
            (PVOID)(base + 0x102E70),
            GauntletLayer_init_hk,
            (PVOID*)&GauntletLayer_init_o
        );

        MH_CreateHook(
            (PVOID)(base + 0x17C4F0),
            LevelLeaderboard_init_hk,
            (PVOID*)&LevelLeaderboard_init_o
        );
        
#ifdef GDL_INDEV
        MH_CreateHook(
            (PVOID)((int)GetProcAddress(cocosModule, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N@Z")),
            dispatchKeyboardMSG_hk,
            (PVOID*)&dispatchKeyboardMSG_o
        );
#endif // GDL_INDEV
    }
}
