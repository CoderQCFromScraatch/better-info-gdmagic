#include "CustomCreatorLayer.h"
#include "CvoltonUpdateLayer.h"
#include "CvoltonOptionsLayer.h"
#include "LevelIDLayer.h"
#include "RewardViewLayer.h"
#include "MoreStatsListView.h"
#include "../managers/CvoltonManager.h"
#include "../utils.hpp"

using namespace gd;
using namespace cocos2d;

CustomCreatorLayer* CustomCreatorLayer::create() {
    auto ret = new CustomCreatorLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

bool CustomCreatorLayer::init() {
    //big part of the code here is stolen from mat's gd mod example because i couldn't be bothered
    //auto backgroundSprite = CCSprite::create("GJ_gradientBG.png");
    /*const int backgrounds[] = {14, 15, 17, 18};
    std::srand(std::time(nullptr));
    int bgNumber = std::rand() % 4;
    std::ostringstream bgStream;
    bgStream << "game_bg_" << backgrounds[bgNumber] << "_001.png";*/

    auto backgroundSprite = CCSprite::create("game_bg_14_001.png"); //stones bg
    //auto backgroundSprite = CCSprite::create("GJ_gradientBG.png"); //rob bg
    //auto backgroundSprite = CCSprite::create(bgStream.str().c_str());
    //auto backgroundSprite = CCSprite::createWithSpriteFrameName("GJ_GameSheet04-uhd.png");
    
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto size = backgroundSprite->getContentSize();
    auto CM = CvoltonManager::sharedState();
    CM->loadTextures();
    
    backgroundSprite->setScale(winSize.width / size.width);
    //backgroundSprite->setScaleX(winSize.width / size.width);
    //backgroundSprite->setScaleY(winSize.height / size.height);
    
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setPosition({0,-75});
    //backgroundSprite->setPosition({0,0});
    
    backgroundSprite->setColor({164, 0, 255}); //purple
    //backgroundSprite->setColor({0, 102, 255}); //rob blue
    
    backgroundSprite->setZOrder(-2);
    addChild(backgroundSprite);

    auto backBtn = gd::CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onBack)
    );

    auto menuBack = CCMenu::create();
    menuBack->addChild(backBtn);
    menuBack->setPosition({25, winSize.height - 25});

    addChild(menuBack);

    setKeypadEnabled(true);

    auto menu = CCMenu::create();
    //menu->setPosition({winSize.width / 2, winSize.height / 2});
    addChild(menu);
    auto label = CCLabelBMFont::create(CM->modName, "goldFont.fnt");

    label->setPosition({winSize.width / 2, winSize.height - 25});

    addChild(label);

    auto featuredBtn = gd::CCMenuItemSpriteExtra::create(
        BetterInfo::createWithBISpriteFrameName("BI_featuredBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onFeatured)
    );
    menu->addChild(featuredBtn);
    featuredBtn->setPosition({-120,55});
    featuredBtn->setSizeMult(1.2f);

    //CCTextureCache::sharedTextureCache()->addImage("mostliked.png", 0);

    auto mostLikedBtn = gd::CCMenuItemSpriteExtra::create(
        BetterInfo::createWithBISpriteFrameName("BI_mostLikedBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onMostLiked)
    );
    menu->addChild(mostLikedBtn);
    mostLikedBtn->setPosition({0,55});
    mostLikedBtn->setSizeMult(1.2f);

    auto searchBtn = gd::CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_searchBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onSearch)
    );
    menu->addChild(searchBtn);
    searchBtn->setPosition({120,57.5f}); //i have no idea why the texture is misaligned theyre the same res
    searchBtn->setSizeMult(1.2f);

    auto dailyBtn = gd::CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_dailyBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onDaily)
    );
    menu->addChild(dailyBtn);
    dailyBtn->setPosition({-122.5,-55});
    dailyBtn->setSizeMult(1.2f);

    //CCTextureCache::sharedTextureCache()->addImage("mostliked.png", 0);

    auto weeklyBtn = gd::CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_weeklyBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onWeekly)
    );
    menu->addChild(weeklyBtn);
    weeklyBtn->setPosition({-2.5,-55});
    weeklyBtn->setSizeMult(1.2f);

    auto updateBtn = gd::CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName(CM->isUpToDate() ? "GJ_getSongInfoBtn_001.png" : "GJ_downloadBtn_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onUpdate)
    );
    menu->addChild(updateBtn);
    updateBtn->setPosition({ 22, - ( (winSize.height / 2) - 25 ) });
    updateBtn->setSizeMult(1.2f);

    /*auto updateArrow = BetterInfo::createBISprite("BI_updateArrow_001.png");
    updateArrow->setAnchorPoint({0,0.1f});
    updateArrow->setPosition({ 33, - ( (winSize.height / 2) - 25 ) });
    if(!CM->isUpToDate()) menu->addChild(updateArrow);*/

    auto settingsSprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSprite->setScale(0.775f);
    auto settingsBtn = gd::CCMenuItemSpriteExtra::create(
        settingsSprite,
        this,
        menu_selector(CustomCreatorLayer::onSettings)
    );
    menu->addChild(settingsBtn);
    settingsBtn->setPosition({ -22, - ( (winSize.height / 2) - 25 ) });
    settingsBtn->setSizeMult(1.2f);

    auto infoBtn = gd::CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
        this,
        menu_selector(CustomCreatorLayer::onInfo)
    );
    menu->addChild(infoBtn);
    infoBtn->setPosition({ (winSize.width / 2) - 25 , (winSize.height / 2) - 25 });
    infoBtn->setSizeMult(1.2f);

    auto cornerBL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBL->setPosition({0,0});
    cornerBL->setAnchorPoint({0,0});
    addChild(cornerBL, -1);

    auto cornerUL = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerUL->setPosition({0,winSize.height});
    cornerUL->setAnchorPoint({0,0});
    cornerUL->setRotation(90);
    addChild(cornerUL, -1);

    auto cornerUR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerUR->setPosition({winSize.width,winSize.height});
    cornerUR->setAnchorPoint({0,0});
    cornerUR->setRotation(180);
    addChild(cornerUR, -1);

    auto cornerBR = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    cornerBR->setPosition({winSize.width,0});
    cornerBR->setAnchorPoint({0,0});
    cornerBR->setRotation(270);
    addChild(cornerBR, -1);

    return true;
}

void CustomCreatorLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}


void CustomCreatorLayer::onBack(CCObject* object) {
    keyBackClicked();
}

void CustomCreatorLayer::onInfo(CCObject* object) {
    gd::FLAlertLayer::create(
        nullptr, 
        CvoltonManager::modName, 
        "OK", 
        nullptr,
        450,
        "This is the main menu for all features related to the mod.\n\n<cy>CoderQC Featured:</c> Featured levels made by @CoderQC.\n<cg>Recently Seen:</c> \"Recently Seen\" recently downloaded levelst.\n<cj>Search:</c> View comments of any level ID.\n\n<cl>Settings:</c> Opens the mod settings\n<cr>Update:</c> Opens the mod update menu"
    )->show();
    /*auto winSize = CCDirector::sharedDirector()->getWinSize();

    CCArray* keys = CCArray::create();
    for(int i = 0; i < 20; i++) keys->addObject(CCString::createWithFormat("%i", i));

    auto listView = MoreStatsListView::create(keys, 356.f, 220.f);
    auto listLayer = GJListLayer::create(listView, "BetterInfo Stats", {191, 114, 62, 255}, 356.f, 220.f);
    listLayer->setPosition(winSize / 2 - listLayer->getScaledContentSize() / 2 - CCPoint(0,5));
    addChild(listLayer);*/
}

void CustomCreatorLayer::onSearch(CCObject* object) {
    LevelIDLayer::create()->show();
}

void CustomCreatorLayer::onFeatured(CCObject* object) {
    auto searchObject = gd::GJSearchObject::create(gd::SearchType::kGJSearchTypeFeaturedGDW);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onMostLiked(CCObject* object) {
    auto searchObject = gd::GJSearchObject::create(gd::SearchType::kGJSearchTypeLikedGDW);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onDaily(CCObject* object) {
    auto searchObject = gd::GJSearchObject::create(gd::SearchType::kSearchTypeDaily);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onWeekly(CCObject* object) {
    auto searchObject = gd::GJSearchObject::create(gd::SearchType::kSearchTypeWeekly);
    auto browserLayer = LevelBrowserLayer::scene(searchObject);

    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void CustomCreatorLayer::onUpdate(CCObject* object) {
    auto layer = CvoltonUpdateLayer::create();
    layer->show();
}

void CustomCreatorLayer::onSettings(CCObject* object) {
    CvoltonOptionsLayer::create()->show();
}

CCScene* CustomCreatorLayer::scene() {
    auto layer = CustomCreatorLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);
    return scene;
}
