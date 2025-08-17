#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelListLayer.hpp>
#include <Geode/modify/GauntletSelectLayer.hpp>
#include <Geode/modify/GauntletLayer.hpp>
#include <Geode/modify/LeaderboardsLayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/GJShopLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/LevelAreaLayer.hpp>
#include <Geode/modify/LevelAreaInnerLayer.hpp>
#include <Geode/modify/SecretLayer4.hpp>
#include <Geode/modify/SecretLayer.hpp>
#include <Geode/modify/SecretLayer5.hpp>
#include <Geode/modify/SecretLayer2.hpp>
#include <Geode/modify/SecretLayer3.hpp>
#include <Geode/modify/SecretRewardsLayer.hpp>
#include <user95401.gif-sprites/include/CCGIFAnimatedSprite.hpp>
#include <random>
#include <filesystem>
#include <fstream>

using namespace geode::prelude;

static auto setupBGPP = [](CCNode* self, const std::string& layerName) {
    auto mod = Mod::get();
    auto bgDir = mod->getSettingValue<std::filesystem::path>("backgrounds");
    if (!std::filesystem::exists(bgDir)) {
        std::filesystem::create_directories(bgDir);
        for (auto& f : {
            "cyber4.gif","debian.png","gray-background.png","lake.png","lines.gif",
            "minimal_gradient.png","odysee.png","plain-wallpaper.png",
            "planet_with_sunrise.png","plant.png","retro_live.gif",
            "shape-abstract.png","shiny-colors.png","smooth_blue.png","wallpaper3-dark.png"
        }) {
            auto src = mod->getResourcesDir() / f, dst = bgDir / f;
            if (std::filesystem::exists(src) && file::readBinary(src)) 
                file::writeBinary(dst, *file::readBinary(src));
        }
        std::ofstream{ bgDir / "DROP IMAGES HERE" }.close();
    }

    auto le_epic = [&](const std::string& path) -> CCNode* {
        std::string lower = path; std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower.ends_with(".png"))
            return CCSprite::create(path.c_str());
        if (auto gif = CCGIFAnimatedSprite::create(path.c_str())) {
            gif->play();
            gif->setLoop(true);
            return gif;
        }
        return nullptr;
    };

    CCNode* bg = nullptr;
    auto manual = mod->getSettingValue<std::filesystem::path>("manual-background").string();
    if (!manual.empty()) bg = le_epic(manual);

    if (!bg) {
        for (auto& ext : {".png", ".gif"}) {
            auto fullPath = bgDir / (layerName + ext);
            if (auto node = le_epic(fullPath.string())) {
                bg = node;
                break;
            }
        }
        if (!bg) {
            std::vector<std::filesystem::path> imgs;
            for (auto& e : std::filesystem::directory_iterator(bgDir))
                if (e.is_regular_file()) {
                    auto ext = e.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    if (ext == ".png" || ext == ".gif")
                        imgs.push_back(e.path());
                }
            if (!imgs.empty()) {
                static std::mt19937_64 rng{ std::random_device{}() }; // mt19937_64 sounds awful jesus christ do better c++
                bg = le_epic(imgs[std::uniform_int_distribution<size_t>(0, imgs.size() - 1)(rng)].string()); // OH MY SWEET JESUS
            }
        }
    }

    if (!bg) {
        if (auto fallback = CCGIFAnimatedSprite::create("static.gif"_spr)) {
    		fallback->setID("oops-gif"_spr);
            fallback->play();
			fallback->setLoop(true);
            bg = fallback;
            auto lbl = CCLabelTTF::create("Backgrounds++ gave up displaying a background because of an error.\nPlease check your settings and make sure your images are not corrupted.", "Arial", 12.f);
            auto win = CCDirector::sharedDirector()->getWinSize();
            lbl->setPosition({ win.width / 2, win.height / 3.5f });
            lbl->setColor(ccc3(255, 0, 0)); lbl->setZOrder(-1);
			lbl->setZOrder(99);
		    lbl->setID("oops"_spr);
            self->addChild(lbl);
        }
    }

    if (bg) {
        auto win = CCDirector::sharedDirector()->getWinSize();
        bg->setPosition({ win.width / 2, win.height / 2 });
        bg->setScaleX(win.width / bg->getContentSize().width);
        bg->setScaleY(win.height / bg->getContentSize().height);
        bg->setZOrder(-2);
		bg->setID("soulja-boy"_spr);
        self->addChild(bg);
    }
};


class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto menu = getChildByID("right-side-menu");
        auto btn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png"),
            this, menu_selector(MyMenuLayer::openFolder)
        );
        btn->setColor(ccc3(255, 140, 0));
		btn->setID("open-folder"_spr);
        menu->addChild(btn);
        menu->updateLayout();

		auto menuBG = this->getChildByID("main-menu-bg");
		if (Mod::get()->getSettingValue<bool>("show-icons") == true) {
		    auto backgroundSprite = menuBG->getChildByID("background");
		    if (backgroundSprite) {
			    backgroundSprite->setVisible(false);
		    } else {
			    geode::log::warn("what");
		    }
		} else {
			if (menuBG) {
				menuBG->setVisible(false);
			} else {
				geode::log::warn("how");
			}
		}

        setupBGPP(this, "MenuLayer");
        return true;
    }
    void openFolder(CCObject*) {
		auto mod = Mod::get();
        file::openFolder(mod->getSettingValue<std::filesystem::path>("backgrounds"));
    }
};

// and now for the index staff's favorite part of this main.cpp!
// (if it works, it works!)

class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
        setupBGPP(this, "CreatorLayer");
        return true;
    }
};

class $modify(MyLevelSearchLayer, LevelSearchLayer) {
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
        setupBGPP(this, "LevelSearchLayer");
        return true;
    }
};

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
	bool init(GJSearchObject* p0) {
		if (!LevelBrowserLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "LevelBrowserLayer");
		return true;
	}
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
	bool init(GJGameLevel* level, bool challenge) {
		if (!LevelInfoLayer::init(level, challenge)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "LevelInfoLayer");
		return true;
	}
};

class $modify(MyEditLevelLayer, EditLevelLayer) {
	bool init(GJGameLevel* level) {
		if (!EditLevelLayer::init(level)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "EditLevelLayer");
		return true;
	}
};

class $modify(MyLevelListLayer, LevelListLayer) {
	bool init(GJLevelList* p0) {
		if (!LevelListLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "LevelListLayer");
		return true;
	}
};

class $modify(MyGauntletSelectLayer, GauntletSelectLayer) {
	bool init(int p0) {
		if (!GauntletSelectLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "GauntletSelectLayer");
		return true;
	}
};

class $modify(MyGauntletLayer, GauntletLayer) {
	bool init(GauntletType p0) {
		if (!GauntletLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "GauntletLayer");
		return true;
	}
};

class $modify(MyLeaderboardsLayer, LeaderboardsLayer) {
	bool init(LeaderboardState p0) {
		if (!LeaderboardsLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "LeaderboardsLayer");
		return true;
	}
};

class $modify(MyGJGarageLayer, GJGarageLayer) {
	bool init() {
		if (!GJGarageLayer::init()) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "GJGarageLayer");
		return true;
	}
};

class $modify(MyGJShopLayer, GJShopLayer) {
	bool init(ShopType p0) {
		if (!GJShopLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "GJShopLayer");
		return true;
	}
};

class $modify(MyLevelSelectLayer, LevelSelectLayer) {
	bool init(int p0) {
		if (!LevelSelectLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "LevelSelectLayer");
		return true;
	}
};

class $modify(MyLevelAreaLayer, LevelAreaLayer) {
	bool init() {
		if (!LevelAreaLayer::init()) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "LevelAreaLayer");
		return true;
	}
};

class $modify(MyLevelAreaInnerLayer, LevelAreaInnerLayer) {
	bool init(int p0) {
		if (!LevelAreaInnerLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "LevelAreaInnerLayer");
		return true;
	}
};

class $modify(MySecretLayer4, SecretLayer4) {
	bool init() {
		if (!SecretLayer4::init()) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "SecretLayer4");
		return true;
	}
};

class $modify(MySecretLayer, SecretLayer) {
	bool init() {
		if (!SecretLayer::init()) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "SecretLayer");
		return true;
	}
};

class $modify(MySecretLayer5, SecretLayer5) {
	bool init() {
		if (!SecretLayer5::init()) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "SecretLayer5");
		return true;
	}
};

class $modify(MySecretLayer2, SecretLayer2) {
	bool init() {
		if (!SecretLayer2::init()) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "SecretLayer2");
		return true;
	}
};

class $modify(MySecretRewardsLayer, SecretRewardsLayer) {
	bool init(bool p0) {
		if (!SecretRewardsLayer::init(p0)) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "SecretRewardsLayer");
		return true;
	}
};

class $modify(MySecretLayer3, SecretLayer3) {
	bool init() {
		if (!SecretLayer3::init()) return false;
		if (Mod::get()->getSettingValue<bool>("only-main-menu") == true) { return true; }
		setupBGPP(this, "SecretLayer3");
		return true;
	}
};