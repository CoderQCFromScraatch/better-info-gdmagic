#define WIN32_LEAN_AND_MEAN
#include "mapped-hooks.hpp"
#include "Windows.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cocos2d.h>
#include <gd.h>
#include <typeinfo>
#include <cstring>
#include <deque>
#include <random>
#include "utils.hpp"

#include "layers/UnregisteredProfileLayer.h"
#include "layers/JumpToPageLayer.h"
#include "layers/ExtendedLevelInfo.h"
#include "layers/CustomCreatorLayer.h"
#include "layers/DailyViewLayer.h"
#include "layers/CvoltonSearchOptions.h"
#include "layers/LevelBrowserEndLayer.h"
#include "layers/LeaderboardViewLayer.h"
#include "layers/ProfileSearchOptions.h"
#include "layers/RewardViewLayer.h"
#include "layers/RewardTypeSelectLayer.h"

#include "managers/CvoltonManager.h"

using namespace cocos2d;
using namespace gd;

const int cvoltonID = 1;
const int commentPageBtnTag = 863390891;
const int questBtnExMarkTag = 863390892;
const int randomBtnTag = 863390893;
const int firstBtnTag = 863390894;
const int filterBtnTag = 863390895;
const int starBtnTag = 863390896;

class CustomLevelSearchLayer : public gd::FLAlertLayer {
    gd::GJGameLevel* level;
public:
    static CustomLevelSearchLayer* create(gd::GJGameLevel* level){
        auto ret = new CustomLevelSearchLayer();
        level->retain();
        ret->level = level;
        if (ret && ret->init()) {
            //robert 1 :D
            ret->autorelease();
        } else {
            //robert -1
            delete ret;
            ret = nullptr;
        }
        return ret;
    }

    void onClose(cocos2d::CCObject* sender)
    {
        level->release();
        setKeypadEnabled(false);
        removeFromParentAndCleanup(true);
    }

    void onSimilar(CCObject* sender) {
        auto searchObject = gd::GJSearchObject::create(gd::SearchType::kSearchTypeSimilar, std::to_string(level->levelID));
        auto browserLayer = LevelBrowserLayer::scene(searchObject);

        auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    void onSong(CCObject* sender) {
        bool customSong = level->songID > 0;
        int songID = customSong ? level->songID : level->audioTrack;

        auto searchObject = gd::GJSearchObject::create(gd::SearchType::kSearchTypeMostLiked);
        searchObject->setSongInfo(customSong, songID);

        auto browserLayer = LevelBrowserLayer::scene(searchObject);

        auto transitionFade = CCTransitionFade::create(0.5, browserLayer);

        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    void onNeighbors(CCObject* sender) {
        std::ostringstream query;
        bool addSeparator = false;
        for(int i = (level->levelID) - 50; i < ((level->levelID) + 50); i++){
            if(addSeparator) query << ",";
            query << i;
            addSeparator = true;
        }

        auto searchObject = gd::GJSearchObject::create(gd::SearchType::kSearchType19, query.str());
        auto browserLayer = LevelBrowserLayer::scene(searchObject);
        auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    bool init(){
        bool init = cocos2d::CCLayerColor::initWithColor({0x00, 0x00, 0x00, 0x4B});
        if(!init) return false;

        cocos2d::CCDirector* director = cocos2d::CCDirector::sharedDirector();
        director->getTouchDispatcher()->incrementForcePrio(2);

        setTouchEnabled(true);
        setKeypadEnabled(true);

        cocos2d::CCSize winSize = director->getWinSize();
        m_pLayer = cocos2d::CCLayer::create();

        this->addChild(m_pLayer);

        cocos2d::extension::CCScale9Sprite* bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
        bg->setContentSize({ 360.0f, 180.0f });
        m_pLayer->addChild(bg, -1);
        bg->setPosition({ winSize.width / 2, winSize.height / 2 });

        auto closeButton = gd::CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this,
            menu_selector(CustomLevelSearchLayer::onClose)
        );

        m_pButtonMenu = CCMenu::create();
        m_pLayer->addChild(m_pButtonMenu, 10);
        m_pButtonMenu->addChild(closeButton);
        closeButton->setPosition({-170.5f, 79});
        closeButton->setSizeMult(1.2f);

        auto searchTitle = CCLabelBMFont::create("Search", "bigFont.fnt");
        searchTitle->setPosition({0,66});

        m_pButtonMenu->addChild(searchTitle);

        auto separator = CCSprite::createWithSpriteFrameName("floorLine_001.png");
        separator->setPosition({285,202});
        separator->setScaleX(0.75f);
        separator->setOpacity(100);
        m_pLayer->addChild(separator);

        /*auto buttonSprite = gd::ButtonSprite::create("Neighbors", (int)(120*0.6), true, "bigFont.fnt", "GJ_button_01.png", 44*0.6f, 0.6f);
        auto buttonButton = gd::CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            menu_selector(CustomLevelSearchLayer::onNeighbors)
        );
        buttonButton->setSizeMult(1.2f);
        buttonButton->setPosition({0,0});
        m_pButtonMenu->addChild(buttonButton);*/

        /*auto neighborButton = createButton(m_pButtonMenu, "Neighbors", menu_selector(CustomLevelSearchLayer::onNeighbors), 0, 4, (int)(120*0.6), 44*0.6f, 0.6f);
        auto similarButton = createButton(m_pButtonMenu, "Similar", menu_selector(CustomLevelSearchLayer::onSimilar), 0, -48, (int)(120*0.6), 44*0.6f, 0.6f);*/

        auto similarButton = createButton(m_pButtonMenu, "Similar", menu_selector(CustomLevelSearchLayer::onSimilar), -75, 8, (int)(120*0.6), 44*0.6f, 0.6f);
        auto neighborButton = createButton(m_pButtonMenu, "Neighbors", menu_selector(CustomLevelSearchLayer::onNeighbors), 75, 8, (int)(120*0.6), 44*0.6f, 0.6f);
        auto songButton = createButton(m_pButtonMenu, "Same song", menu_selector(CustomLevelSearchLayer::onSong), 0, -48, (int)(120*0.6), 44*0.6f, 0.6f);

        return true;
    }

    gd::CCMenuItemSpriteExtra* createButton(CCNode* menu, const char* text, cocos2d::SEL_MenuHandler handler, float x, float y, int width, float height, float scale){
        auto buttonSprite = gd::ButtonSprite::create(text, width, true, "bigFont.fnt", "GJ_button_01.png", height, scale);
        auto buttonButton = gd::CCMenuItemSpriteExtra::create(
            buttonSprite,
            this,
            handler
        );
        buttonButton->setSizeMult(1.2f);
        buttonButton->setPosition({x,y});
        menu->addChild(buttonButton);

        return buttonButton;
    }

};

class StaticStringHelper {
public:
    static std::string getFriendRequestType(int type){
        switch(type){
            case 0: return "Enabled";
            case 1: return "Disabled";
            default: return "Unknown";
        }
    }

    static std::string getMessageType(int type){
        switch(type){
            case 0: return "Enabled";
            case 1: return "Friends Only";
            case 2: return "Disabled";
            default: return "Unknown";
        }
    }

    static std::string getIconType(int type){
        switch(type){
            case 0: return "Cube";
            case 1: return "Ship";
            case 2: return "Ball";
            case 3: return "UFO";
            case 4: return "Wave";
            case 5: return "Robot";
            case 6: return "Spider";
            case 7: return "Swingcopter";
            default: return "None";
        }
    }
};

class GamingButton {
public:
    void onProfilePage(CCObject* sender){
        auto layer = cast<CommentCell*>(this);
        layer->comment->m_pUserScore->setUserID(layer->comment->m_nAuthorPlayerID);
        UnregisteredProfileLayer* profileLayer = UnregisteredProfileLayer::create(layer->comment->m_pUserScore, layer->getParent()->getParent()->getParent()->getParent()->getParent()->getParent());
        profileLayer->show();
    }

    void onProfilePageInfo(CCObject* sender){
        auto layer = cast<ProfilePage*>(this);

        auto score = layer->score;
        auto GM = GameManager::sharedState();

        std::ostringstream contentStream;
        if(score->getUserID() == GM->m_nPlayerUserID) contentStream << "User ID: " << score->getUserID()
            << "\nAccount ID: " << score->getAccountID()
            << "\n\n";//Leaderboard Icon: " << StaticStringHelper::getIconType(score->getIconType()) (is not sent)
        contentStream << "Friend Requests: " << StaticStringHelper::getFriendRequestType(score->getFriendStatus())
            << "\nPrivate Messages: " << StaticStringHelper::getMessageType(score->getMessageState())
            << "\nComment History: " << StaticStringHelper::getMessageType(score->getCommentHistoryStatus());
        if(score->getUserID() == GM->m_nPlayerUserID) contentStream << "\n\nBootups: " << GM->m_nBootups;

        //if(score->getUserID() == cvoltonID) contentStream << "\n\nThis user is epic!";

        gd::FLAlertLayer::create(nullptr, "User Info", "OK", nullptr, contentStream.str())->show();
    }

    void onProfilePageLeaderboard(CCObject* sender){
        auto layer = cast<ProfilePage*>(this);

        auto score = layer->score;

        auto scene = LeaderboardViewLayer::scene(score->getAccountID());
        auto transitionFade = CCTransitionFade::create(0.5, scene);
        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    void onProfilePageReload(CCObject* sender){
        auto self = cast<ProfilePage*>(this);

        auto GLM = GameLevelManager::sharedState();

        auto score = self->score;
        GLM->resetStoredUserInfo(score->getAccountID());
        GLM->resetAccountComments(score->getAccountID());

        GLM->getGJUserInfo(self->something);
    }

    void onProfilePageCopyUserID(CCObject* sender){
        auto self = cast<ProfilePage*>(this);
        BetterInfo::copyToClipboard(std::to_string(self->score->getUserID()).c_str(), self);
    }

    void onProfilePageCopyAccountID(CCObject* sender){
        auto self = cast<ProfilePage*>(this);
        BetterInfo::copyToClipboard(std::to_string(self->score->getAccountID()).c_str(), self);
    }

    void onProfilePageCopyPlayerName(CCObject* sender){
        auto self = cast<ProfilePage*>(this);
        BetterInfo::copyToClipboard(self->score->getPlayerName().c_str(), self);
    }

    /*void onProfilePageStar(CCObject* sender){
        auto self = cast<ProfilePage*>(this);
        auto button = cast<CCMenuItemSpriteExtra*>(sender);
        auto CM = CvoltonManager::sharedState();
        if(CM->toggleOption("profile_search_star")) button->setColor({255, 255, 255});
        else button->setColor({125,125,125});
    }*/

    void onLevelBrowserSavedFilter(CCObject* sender){
        auto self = cast<LevelBrowserLayer*>(this);

        ProfileSearchOptions::create(self)->show();
    }

    void onCustomSearch(CCObject* sender){
        auto self = cast<InfoLayer*>(this);

        auto level = self->m_pLevel;

        CustomLevelSearchLayer* layer = CustomLevelSearchLayer::create(level);
        layer->show();
    }

    void onInfoLayerSchedule(CCObject* sender){
        auto self = cast<InfoLayer*>(this);

        bool prevVisible = self->m_pPrevPageBtn->isVisible();
        bool nextVisible = self->m_pNextPageBtn->isVisible();

        auto GLM = GameLevelManager::sharedState();
        if(self->m_bCommentHistory) GLM->resetCommentTimersForLevelID(self->m_pScore->userID_, self->m_bCommentHistory);
        else GLM->resetCommentTimersForLevelID(self->m_pLevel->levelID, self->m_bCommentHistory);
        self->loadPage(self->m_nPageNumber, true);

        self->m_pLoadingCircle->setVisible(false);

        self->m_pPrevPageBtn->setVisible(prevVisible);
        self->m_pNextPageBtn->setVisible(nextVisible);
    }

    void onInfoLayerToggleSchedule(CCObject* sender){
        auto self = cast<InfoLayer*>(this);
        auto senderBtn = static_cast<CCMenuItemToggler*>(sender);

        bool newState = !(reinterpret_cast<int>(senderBtn->getUserData()));
        senderBtn->setUserData(reinterpret_cast<void*>(newState));

        if(newState) self->getScheduler()->scheduleSelector(schedule_selector(GamingButton::onInfoLayerSchedule), self, 2, false);
        else self->getScheduler()->unscheduleSelector(schedule_selector(GamingButton::onInfoLayerSchedule), self);
    }

    void onLevelInfoNoLoad(CCObject* sender){
        auto layer = cast<CommentCell*>(this);

        auto level = gd::GJGameLevel::create();
        level->levelID_rand = layer->comment->m_nLevelID;
        level->levelID_seed = 0;
        level->levelID = layer->comment->m_nLevelID;

        InfoLayer* infoLayer = InfoLayer::create(level, nullptr);
        infoLayer->setParentFLAlert(layer->getParent()->getParent()->getParent()->getParent()->getParent()->getParent());
        infoLayer->show();

        //gd::FLAlertLayer::create(nullptr, "Level Stats", "OK", nullptr, std::to_string(layer->comment->m_nLevelID))->show();
    }

    void onCustomCreatorLayer(CCObject* sender){
        auto browserLayer = CustomCreatorLayer::scene();
        auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
        CCDirector::sharedDirector()->pushScene(transitionFade);
    }

    void onJumpToPageLayer(CCObject* sender){
        auto layer = cast<InfoLayer*>(this);
        JumpToPageLayer* jumpLayer = JumpToPageLayer::create(layer);
        jumpLayer->show();
    }

    void onLeaderboardDaily(CCObject* sender){
        auto self = cast<LevelLeaderboard*>(this);
        auto level = gd::GJGameLevel::create();
        level->levelID_rand = self->level->levelID_rand;
        level->levelID_seed = self->level->levelID_seed;
        level->levelID = self->level->levelID;
        level->levelName = self->level->levelName;
        level->dailyID_rand = !self->level->dailyID;
        level->dailyID_seed = 0;
        level->dailyID = !self->level->dailyID;
        //GameLevelManager::sharedState()->m_pTimerDict->setObject(CCString::create("0"), "upd_11");
        //GameLevelManager::sharedState()->m_pTimerDict->writeToFile("c:/users/brabe/documents/test.plist");
        GameLevelManager::sharedState()->m_pTimerDict->removeAllObjects(); //this is equivalent to GLM::resetAllTimers() which I suspect is inlined on windows but i didn't bother checking
        //auto obj = dynamic_cast<CCString*>(GameLevelManager::sharedState()->m_pTimerDict->objectForKey("upd_ll"));
        //gd::FLAlertLayer::create(nullptr, "Level Stats", "OK", nullptr, std::to_string(GameLevelManager::sharedState()->m_weeklyTimeLeft))->show();
        auto newLeaderboard = LevelLeaderboard::create(level, self->leaderboardType);
        //newLeaderboard->show();
        newLeaderboard->onUpdate(sender);
        CCDirector::sharedDirector()->getRunningScene()->addChild(newLeaderboard);
        self->onClose(sender);
        /*self->level->release();
        level->retain();
        self->level = level;
        self->onUpdate(sender);*/
    }

    void onLevelBrowserRandom(CCObject* sender){
        auto layer = cast<LevelBrowserLayer*>(this);

        if(layer->searchObject == nullptr) return;
        int pageMax = layer->total / BetterInfo::levelsPerPage(layer->searchObject);
        //int pageToLoad = std::rand() % pageMax;

        int pageToLoad = CvoltonManager::sharedState()->randomNumber(0, pageMax);

        layer->searchObject->m_nPage = pageToLoad;
        layer->loadPage(layer->searchObject);
    }

    void onLevelBrowserFirst(CCObject* sender){
        auto layer = cast<LevelBrowserLayer*>(this);
        layer->searchObject->m_nPage = 0;
        layer->loadPage(layer->searchObject);
    }

    void onLevelBrowserLast(CCObject* sender){
        auto layer = cast<LevelBrowserLayer*>(this);

        if(layer->total == 9999 || BetterInfo::isFalseTotal(layer->searchObject)) {
            LevelBrowserEndLayer::create(layer)->show();
            return;
        }

        layer->searchObject->m_nPage = (layer->total - 1) / BetterInfo::levelsPerPage(layer->searchObject);
        layer->loadPage(layer->searchObject);
    }

    void onLevelBrowserStar(CCObject* sender){
        auto layer = cast<LevelBrowserLayer*>(this);

        if(layer->searchObject == nullptr) return;

        layer->searchObject->m_bStarFilter = !(layer->searchObject->m_bStarFilter);
        layer->loadPage(layer->searchObject);

        auto button = cast<CCMenuItemSpriteExtra*>(sender);
        if(layer->searchObject->m_bStarFilter) button->setColor({255, 255, 255});
        else button->setColor({125,125,125});
    }

    void onDailyHistory(CCObject* sender){
        auto self = cast<DailyLevelPage*>(this);
        auto layer = DailyViewLayer::scene(self->isWeekly);
        auto transitionFade = CCTransitionFade::create(0.5, layer);
        CCDirector::sharedDirector()->pushScene(transitionFade);
        //CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
    }

    void onLeaderboardRefresh(CCObject* sender){
        auto GLM = gd::GameLevelManager::sharedState();
        GLM->updateUserScore();
        GLM->storedLevels->removeObjectForKey("leaderboard_creator");
        GLM->storedLevels->removeObjectForKey("leaderboard_friends");
        GLM->storedLevels->removeObjectForKey("leaderboard_global");
        GLM->storedLevels->removeObjectForKey("leaderboard_top");

        auto layer = LeaderboardsLayer::create(GLM->leaderboardState);
        auto scene = CCScene::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->replaceScene(scene);
    }

    void onMoreSearchNext(CCObject* sender){
        auto self = cast<FLAlertLayer*>(this);
        auto layer = CvoltonSearchOptions::create();
        CCDirector::sharedDirector()->getRunningScene()->addChild(layer);
        self->onClose(sender);
    }

    void onRewardsPageHistory(CCObject* sender){
        RewardTypeSelectLayer::create()->show();
    }

};

bool __fastcall InfoLayer_init(CCLayer* self, void* a, gd::GJGameLevel* level, void* c) {
    if (!MHook::getOriginal(InfoLayer_init)(self, a, level, c)) return false;

    auto layer = cast<CCLayer*>(self->getChildren()->objectAtIndex(0));
    CCMenu* menu = cast<CCMenu*>(layer->getChildren()->objectAtIndex(1));

    gd::CCMenuItemSpriteExtra* playerName = cast<gd::CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(0));
    playerName->setEnabled(true);

    auto buttonSprite = gd::ButtonSprite::create("1", 12, true, "bigFont.fnt", "GJ_button_02.png", 25, 0.4f);
    auto buttonButton = gd::CCMenuItemSpriteExtra::create(
        buttonSprite,
        self,
        menu_selector(GamingButton::onJumpToPageLayer)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({195,34});
    buttonButton->setTag(commentPageBtnTag);
    menu->addChild(buttonButton);

    if(level == nullptr) return true;

    auto searchSprite = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
    searchSprite->setScale(0.8f);
    auto searchButton = gd::CCMenuItemSpriteExtra::create(
        searchSprite,
        self,
        menu_selector(GamingButton::onCustomSearch)
    );
    menu->addChild(searchButton);
    searchButton->setPosition({195, 68});
    searchButton->setSizeMult(1.2f);

    auto scheduleOffSprite = CCSprite::createWithSpriteFrameName("GJ_playEditorBtn_001.png");
    scheduleOffSprite->setScale(.625f);
    auto scheduleOnSprite = CCSprite::createWithSpriteFrameName("GJ_stopEditorBtn_001.png");
    scheduleOnSprite->setScale(.625f);
    auto scheduleBtn = CCMenuItemToggler::create(
        scheduleOffSprite, 
        scheduleOnSprite, 
        self,
        menu_selector(GamingButton::onInfoLayerToggleSchedule)
    );
    scheduleBtn->setUserData(reinterpret_cast<void*>(false));
    scheduleBtn->setPosition({202.5, 100});
    menu->addChild(scheduleBtn);

    if(level->originalLevel != 0) {
        auto originalBtn = dynamic_cast<CCNode*>(menu->getChildren()->objectAtIndex(1));
        if(!originalBtn || originalBtn->getPositionY() != 99) return true;

        if(originalBtn->getPositionX() > 155.4f) originalBtn->setPosition({155.4f, originalBtn->getPositionY()});
    }

    return true;
}

bool __fastcall InfoLayer_onMore(InfoLayer* self, void* a, CCObject* b) {
    if(self->m_pLevel->accountID == 0) {
        UnregisteredProfileLayer::displayProfile(self->m_pLevel->userID, self->m_pLevel->userName);
        return true;
    }

    if (!MHook::getOriginal(InfoLayer_onMore)(self, a, b)) return false;

    return true;
}

bool __fastcall LevelInfoLayer_setupProgressBars(CCLayer* self) {
    if (!MHook::getOriginal(LevelInfoLayer_setupProgressBars)(self)) return false;

    CCMenu* menu = cast<CCMenu*>(self->getChildren()->objectAtIndex(2));
    gd::CCMenuItemSpriteExtra* playerName = cast<gd::CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(0));
    playerName->setEnabled(true);


    return true;
}

bool __fastcall LevelInfoLayer_onViewProfile(LevelInfoLayer* self, void* a, CCObject* b) {

    if(self->level->accountID == 0) {
        UnregisteredProfileLayer::displayProfile(self->level->userID, self->level->userName);
        return true;
    }

    if (!MHook::getOriginal(LevelInfoLayer_onViewProfile)(self, a, b)) return false;

    return true;
}

bool __fastcall LevelCell_onViewProfile(LevelCell* self, void* a, CCObject* b) {

    if(self->level->accountID == 0) {
        UnregisteredProfileLayer::displayProfile(self->level->userID, self->level->userName);
        return true;
    }

    if (!MHook::getOriginal(LevelCell_onViewProfile)(self, a, b)) return false;

    return true;
}

void __fastcall LevelCell_loadCustomLevelCell(LevelCell* self) {
    MHook::getOriginal(LevelCell_loadCustomLevelCell)(self);
    
    auto layer = cast<CCLayer*>(self->getChildren()->objectAtIndex(1));
    for(unsigned int i = 0; i < layer->getChildrenCount(); i++){
        auto menu = dynamic_cast<CCMenu*>(layer->getChildren()->objectAtIndex(i));
        if(menu != nullptr){
            auto playerName = cast<gd::CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(1));
            playerName->setEnabled(true);

            std::ostringstream idText;
            idText << "#" << self->level->levelID;
            auto idTextNode = CCLabelBMFont::create(idText.str().c_str(), "chatFont.fnt");
            idTextNode->setPosition({33,33});
            idTextNode->setAnchorPoint({1,0});
            idTextNode->setScale(0.6f);
            idTextNode->setColor({51,51,51});
            idTextNode->setOpacity(152);
            menu->addChild(idTextNode);
            if(self->level->dailyID > 0 || CvoltonManager::sharedState()->getOption("white_id")){
                idTextNode->setColor({255,255,255});
                idTextNode->setOpacity(200);
            }

            if(self->level->dailyID > 0){

                const int maxDaily = 100000;

                std::ostringstream dailyText;
                dailyText << ((self->level->dailyID >= maxDaily) ? "Weekly" : "Daily") << " #" << (self->level->dailyID % maxDaily);
                auto dailyTextNode = CCLabelBMFont::create(dailyText.str().c_str(), "chatFont.fnt");
                dailyTextNode->setPosition({33,43});
                dailyTextNode->setAnchorPoint({1,0});
                dailyTextNode->setScale(0.6f);
                dailyTextNode->setColor({255,255,255});
                dailyTextNode->setOpacity(200);
                menu->addChild(dailyTextNode);

            }

            break;
        }
    }
}

//this would ideally only be called from init but there are hook conflicts
void fixProfilePagePositions(ProfilePage* self){
    auto layer = cast<CCLayer*>(self->getChildren()->objectAtIndex(0));
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    for(unsigned int i = 0; i < self->m_pButtonMenu->getChildrenCount(); i++){
        CCNode* node = dynamic_cast<CCNode*>(self->m_pButtonMenu->getChildren()->objectAtIndex(i));
        if(node != nullptr && node->getPositionX() == 12 && node->getPositionY() == -258) node->setPosition(16, -224); //node->setPosition({52, -258}); // //
    }

    CCNode* followTxt = dynamic_cast<CCNode*>(layer->getChildren()->objectAtIndex(6));
    if(followTxt->getPositionY() == (winSize.height / 2) - 125) followTxt->setVisible(false); //followTxt->setPositionX(followTxt->getPositionX() + 40);
}

void __fastcall ProfilePage_loadPageFromUserInfo(ProfilePage* self, void* a, gd::GJUserScore* a2){
    //TODO: fix mod badge positioning for empty profiles
    GameLevelManager::sharedState()->storeUserName(a2->getUserID(), a2->getAccountID(), a2->getPlayerName());

    MHook::getOriginal(ProfilePage_loadPageFromUserInfo)(self, a, a2);

    fixProfilePagePositions(self);

    auto layer = cast<CCLayer*>(self->getChildren()->objectAtIndex(0));
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto CM = CvoltonManager::sharedState();

    auto menu = self->m_pButtonMenu;

    auto infoSprite = CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png");
    infoSprite->setScale(0.7f);
    auto infoButton = gd::CCMenuItemSpriteExtra::create(
        infoSprite,
        self,
        menu_selector(GamingButton::onProfilePageInfo)
    );
    menu->addChild(infoButton);
    infoButton->setPosition({16, -135});
    //infoButton->setScale(0.8f);
    infoButton->setSizeMult(1.2f);

    self->objectsInMenu->addObject(infoButton);

    if(a2->getUserID() != GameManager::sharedState()->m_nPlayerUserID){
        for(unsigned int i = 0; i < layer->getChildrenCount(); i++){
            CCNode* node = dynamic_cast<CCNode*>(layer->getChildren()->objectAtIndex(i));
            if(node != nullptr && node->getPositionX() == (winSize.width / 2) - 164 && node->getPositionY() == (winSize.height / 2) + 123) node->setVisible(false);
            if(node != nullptr && node->getPositionX() == (winSize.width / 2) && node->getPositionY() == (winSize.height / 2) + 125) node->setVisible(false);
            //if(node != nullptr && node->getPositionX() == (winSize.width / 2) + 138 && node->getPositionY() == (winSize.height / 2) - 123) node->setVisible(false);
        }

        auto leaderboardButtonSprite = BetterInfo::createBISprite("BI_blankBtn_001.png");
        auto leaderboardSprite = CCSprite::createWithSpriteFrameName(BetterInfo::rankIcon(a2->getGlobalRank()));
        leaderboardSprite->setZOrder(1);
        leaderboardSprite->setScale(1 / 0.6f);
        if(a2->getGlobalRank() > 0) {
            if(a2->getGlobalRank() <= 10) leaderboardSprite->setScale(1.1f);
            else if(a2->getGlobalRank() <= 50) leaderboardSprite->setScale(1.25f);
            else if(a2->getGlobalRank() <= 200) leaderboardSprite->setScale(1.3f);
            else if(a2->getGlobalRank() <= 1000) leaderboardSprite->setScale(1 / 0.7f);
        }
        leaderboardSprite->setPosition({22.5f, 23});
        leaderboardButtonSprite->addChild(leaderboardSprite);
        leaderboardButtonSprite->setScale( (a2->getGlobalRank() <= 1000 && a2->getGlobalRank() > 0) ? 0.7f : 0.6f);
        auto leaderboardButton = gd::CCMenuItemSpriteExtra::create(
            leaderboardButtonSprite,
            self,
            menu_selector(GamingButton::onProfilePageLeaderboard)
        );
        menu->addChild(leaderboardButton);
        leaderboardButton->setPosition({46, -12});
        //leaderboardButton->setScale(0.8f);
        leaderboardButton->setSizeMult(1.2f);
        self->objectsInMenu->addObject(leaderboardButton);

        //auto leaderboardArrow = BetterInfo::createBISprite("BI_viewLeaderboard_001.png");
        //menu->addChild(leaderboardArrow);

        /*auto upArrow = BetterInfo::createBISprite("BI_upArrow_001.png");
        upArrow->setScale(0.8f);
        upArrow->setPosition({30, -18});
        upArrow->setVisible(!(CM->getOption("has_viewed_as")));
        menu->addChild(upArrow);
        self->objectsInMenu->addObject(upArrow);*/

        //set comment to 408 -148
        /*for(unsigned int i = 0; i < self->m_pButtonMenu->getChildrenCount(); i++){
            CCNode* node = dynamic_cast<CCNode*>(self->m_pButtonMenu->getChildren()->objectAtIndex(i));
            if(node != nullptr && node->getPositionX() == 408 && node->getPositionY() == -135) node->setPosition(408, -148); //node->setPosition({52, -258}); // //
        }*/


        auto refreshSprite = CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
        //refreshSprite->setScale(0.7f);
        auto refreshButton = gd::CCMenuItemSpriteExtra::create(
            refreshSprite,
            self,
            menu_selector(GamingButton::onProfilePageReload)
        );
        menu->addChild(refreshButton);
        refreshButton->setPosition({0, -269});
        //refreshButton->setPosition({16,-175});
        //refreshButton->setScale(0.8f);
        refreshButton->setSizeMult(1.2f);

        self->objectsInMenu->addObject(refreshButton);

        auto userIDNode = CCLabelBMFont::create(CCString::createWithFormat("User ID: %i", a2->getUserID())->getCString(), "chatFont.fnt");
        userIDNode->setScale(0.6f);
        userIDNode->setColor({255,255,255});
        userIDNode->setOpacity(220);
        auto userIDBtn = gd::CCMenuItemSpriteExtra::create(
            userIDNode,
            self,
            menu_selector(GamingButton::onProfilePageCopyUserID)
        );
        userIDBtn->setPosition({38,-248});
        userIDBtn->setAnchorPoint({0,1});
        menu->addChild(userIDBtn);
        self->objectsInMenu->addObject(userIDBtn);

        auto accountIDNode = CCLabelBMFont::create(CCString::createWithFormat("Account ID: %i", a2->getAccountID())->getCString(), "chatFont.fnt");
        accountIDNode->setScale(0.6f);
        accountIDNode->setColor({255,255,255});
        accountIDNode->setOpacity(220);
        auto accountIDBtn = gd::CCMenuItemSpriteExtra::create(
            accountIDNode,
            self,
            menu_selector(GamingButton::onProfilePageCopyAccountID)
        );
        accountIDBtn->setPosition({38,-258});
        accountIDBtn->setAnchorPoint({0,1});
        menu->addChild(accountIDBtn);
        self->objectsInMenu->addObject(accountIDBtn);

        auto usernameNode = CCLabelBMFont::create(a2->getPlayerName().c_str(), "bigFont.fnt");
        usernameNode->limitLabelWidth(a2->modBadge_ > 0 ? 140.f : 185.0f, 0.9f, 0.0f);
        auto usernameBtn = gd::CCMenuItemSpriteExtra::create(
            usernameNode,
            self,
            menu_selector(GamingButton::onProfilePageCopyPlayerName)
        );
        usernameBtn->setPosition({210,-10});
        menu->addChild(usernameBtn);
        self->objectsInMenu->addObject(usernameBtn);

        /*auto CM = CvoltonManager::sharedState();
        auto starButton = gd::CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png"),
            self,
            menu_selector(GamingButton::onProfilePageStar)
        );
        menu->addChild(starButton);
        starButton->setPosition({370,-259});
        if(!CM->getOption("profile_search_star")) starButton->setColor({125,125,125});
        starButton->setSizeMult(1.2f);
        self->objectsInMenu->addObject(starButton);*/
    }

}

bool __fastcall ProfilePage_init(ProfilePage* self, void* a, int id, bool a2){
    if(!MHook::getOriginal(ProfilePage_init)(self, a, id, a2)) return false;

    fixProfilePagePositions(self);

    return true;
}

/*void __fastcall ProfilePage_onMyLevels(ProfilePage* self, void* a, CCObject* sender) {
    //gd::FLAlertLayer::create(nullptr, "User Info", "OK", nullptr, "aaegeg")->show();
    //self->m_pLevel->retain();
    auto CM = CvoltonManager::sharedState();
    if(!CM->getOption("profile_search_star")){
        MHook::getOriginal(ProfilePage_onMyLevels)(self, a, sender);
    }

    auto searchObj = GJSearchObject::create(SearchType::kSearchTypeUsersLevels, std::to_string(self->score->getUserID()));
    searchObj->m_bStarFilter = true;

    auto browserLayer = LevelBrowserLayer::scene(searchObj);
    auto transitionFade = CCTransitionFade::create(0.5, browserLayer);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}*/

/*void* __fastcall ProfilePage_getUserInfoFailed(ProfilePage* self, void* a, gd::GJUserScore* a2){
    void* returnValue = MHook::getOriginal(ProfilePage_getUserInfoFailed)(self, a, a2);
    gd::GJUserScore* score = gd::GJUserScore::create();
    score->setPlayerName("amongus");
    score->setPlayerCube(6);
    //self->loadPageFromUserInfo(score);
    return returnValue;
}*/

void __fastcall CommentCell_loadFromComment(CommentCell* self, void* a, GJComment* b) {
    MHook::getOriginal(CommentCell_loadFromComment)(self, a, b);

    //if(b->m_bHasLevelID) return;

    auto layer = cast<CCLayer*>(self->getChildren()->objectAtIndex(1));

    CCMenu* menu = nullptr;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    bool smallCommentsMode = gd::GameManager::sharedState()->getGameVariable("0088");

    for(unsigned int i = 0; i < layer->getChildrenCount(); i++){
        menu = dynamic_cast<CCMenu*>(layer->getChildren()->objectAtIndex(i));
        if(menu != nullptr) break;
    }

    if(menu == nullptr) return;

    if(!(b->m_bHasLevelID)){

        if(b->m_nAuthorAccountID != 0) return;

        auto originalNameNode = cast<CCLabelBMFont*>(layer->getChildren()->objectAtIndex(2));
        //std::string userName = originalNameNode->getString();
        
        if(strlen(originalNameNode->getString()) == 0){
            std::stringstream contentStream;
            contentStream << "- (ID: " << b->m_nAuthorPlayerID << ")";
            originalNameNode->setString(contentStream.str().c_str());
        }
        layer->removeChild(originalNameNode);
        //originalNameNode->setAnchorPoint({0,0});

        //auto newPlayerName = CCLabelBMFont::create(userName.c_str(), "goldFont.fnt");

        auto buttonButton = gd::CCMenuItemSpriteExtra::create(
            originalNameNode,
            self,
            menu_selector(GamingButton::onProfilePage)
        );
        buttonButton->setSizeMult(1.2f);
        buttonButton->setPosition(37 - (winSize.width / 2), smallCommentsMode ? 18.5f - (winSize.height / 2) : 50.5f - (winSize.height / 2) );
        buttonButton->setAnchorPoint({0.1f,0});
        buttonButton->setEnabled(true);
        menu->addChild(buttonButton);

    }else{

        auto commentsSprite = CCSprite::createWithSpriteFrameName("GJ_chatBtn_001.png");
        commentsSprite->setScale(smallCommentsMode ? 0.35f : 0.5f);
        auto commentsButton = gd::CCMenuItemSpriteExtra::create(
            commentsSprite,
            self,
            menu_selector(GamingButton::onLevelInfoNoLoad)
        );
        menu->addChild(commentsButton);
        //commentsButton->setPosition({-79,-136});
        CCPoint smallPosition({206 - (winSize.width / 2), 24.5f - (winSize.height / 2) });
        CCPoint largePosition({154 - (winSize.width / 2), 60 - (winSize.height / 2) });
        commentsButton->setPosition(smallCommentsMode ? smallPosition : largePosition);
        //commentsButton->setScale(0.8f);
        commentsButton->setSizeMult(1.2f);

    }

}

//The following CommentCell functions are required as fixes for crashes caused by refreshing the layer while liking/deleting an item
void __fastcall CommentCell_onConfirmDelete(CommentCell* self, void* a, CCObject* sender) {
    self->retain();
    MHook::getOriginal(CommentCell_onConfirmDelete)(self, a, sender);
}

void __fastcall CommentCell_FLAlert_Clicked(uint8_t* self, void* a, FLAlertLayer* layer, bool btn) {
    MHook::getOriginal(CommentCell_FLAlert_Clicked)(self, a, layer, btn);

    auto cell = reinterpret_cast<CommentCell*>(self - sizeof(LikeItemDelegate) - sizeof(TableViewCell));
    cell->release();
}

//This function is simple enough that it's easier to rewrite it than to figure out how midhooks work
void __fastcall CommentCell_onLike(CommentCell* self, void* a, CCObject* sender) {
    if(!self->comment) return;

    LikeItemType type = kLikeItemTypeComment;
    int special = self->comment->m_nLevelID;

    if (self->accountComment) {
        special = self->comment->m_nAuthorAccountID;
        type = kLikeItemTypeAccountComment;
    }

    bool liked = GameLevelManager::sharedState()->hasLikedItemFullCheck(type, self->comment->m_nCommentID, special);
    if(liked) return;

    auto GM = GameManager::sharedState();
    if (self->comment->m_nAuthorPlayerID == GM->m_nPlayerUserID) return;
    
    auto AM = GJAccountManager::sharedState();
    if (self->comment->m_nAuthorAccountID && self->comment->m_nAuthorAccountID == AM->accountID) return;

    auto layer = LikeItemLayer::create(type, self->comment->m_nCommentID, special);
    layer->m_delegate = reinterpret_cast<LikeItemDelegate*>(reinterpret_cast<uint8_t*>(self) + 376);
    layer->show();
    self->retain();
}

void __fastcall CommentCell_likedItem(uint8_t* self, void* a, LikeItemType* type, int id, int special) {
    MHook::getOriginal(CommentCell_likedItem)(self, a, type, id, special);

    auto cell = reinterpret_cast<CommentCell*>(self - sizeof(TableViewCell));
    cell->release();
}

void __fastcall LevelInfoLayer_onLevelInfo(LevelInfoLayer* self, void* a, CCObject* sender) {
    ExtendedLevelInfo::showProgressDialog(self->level);
}

void __fastcall EditLevelLayer_onLevelInfo(EditLevelLayer* self, void* a, CCObject* sender) {
    ExtendedLevelInfo::showProgressDialog(self->m_pLevel);
}

void __fastcall LevelPage_onInfo(LevelPage* self, void* a, CCObject* sender) {
    //gd::FLAlertLayer::create(nullptr, "User Info", "OK", nullptr, std::to_string((int)self))->show();
    //self->m_pLevel->retain();
    ExtendedLevelInfo::showProgressDialog(self->m_pLevel);
}

void __fastcall InfoLayer_onLevelInfo(InfoLayer* self, void* a, CCObject* sender) {
    auto CM = CvoltonManager::sharedState();
    if(CM->getOption("no_level_info")){
        MHook::getOriginal(InfoLayer_onLevelInfo)(self, a, sender);
        return;
    }

    auto level = self->m_pLevel;

    ExtendedLevelInfo* layer = ExtendedLevelInfo::create(level);
    layer->show();

}

std::string* __fastcall GameLevelManager_userNameForUserID(void* a, void* b, std::string* userName, int userID) {
    auto original = MHook::getOriginal(GameLevelManager_userNameForUserID)(a, b, userName, userID);
    //if i don't clone the string then unmodified strings have a slight chance of crashing the game and i have no idea why
    //auto newString = std::string(*userName);

    auto CM = CvoltonManager::sharedState();

    if(!CM->getOption("no_green_user") && (*userName == "" || *userName == "-")){
        auto CM = CvoltonManager::sharedState();
        *userName = CM->getUserName(userID);

        if(userID == 32471) *userName = "PixelCube"; //previous dataset had an error
    }
    //std::cout << userID << " " << newString << std::endl;

    //*userName = newString;

    /*switch(userID){
        case 248868:
            *userName = "roadbose";
            break;
        case 35:
            *userName = "lich";
            break;
        case 32:
            *userName = "chad";
            break;
        case 65:
            *userName = "Carbonelite";
            break;
        case 90:
            *userName = "angrybirds94";
            break;
        case 1732:
            *userName = "darnoc2";
            break;
        case 46587:
            *userName = "diamond";
            break;
    }*/

    return original;
}

void __fastcall InfoLayer_setupCommentsBrowser(InfoLayer* self, void* a, CCArray* a3) {
    MHook::getOriginal(InfoLayer_setupCommentsBrowser)(self, a, a3);

    if(self->m_nTotalItems >= 999) self->m_pNextPageBtn->setVisible(true);
}

void __fastcall InfoLayer_loadPage(InfoLayer* self, void* a, int page, bool reload) {
    MHook::getOriginal(InfoLayer_loadPage)(self, a, page, reload);

    CCMenu* menu = cast<CCMenu*>(self->m_pCommentsBtn->getParent());

    std::cout << menu->getChildrenCount() << std::endl;

    for(unsigned int i = 0; i < menu->getChildrenCount(); i++){
        auto commentBtn = cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(i));
        std::cout << i << std::endl;
        if(commentBtn == nullptr) continue;

        std::cout << commentBtn->getTag() << std::endl;


        if(commentBtn->getTag() == commentPageBtnTag){

            auto commentBtnSprite = cast<ButtonSprite*>(commentBtn->getChildren()->objectAtIndex(0));
            if(commentBtnSprite == nullptr) continue;

            commentBtnSprite->setString(std::to_string(page+1).c_str());
        }
    }
}

void __fastcall LevelBrowserLayer_updateLevelsLabel(LevelBrowserLayer* self, void* a) {
    MHook::getOriginal(LevelBrowserLayer_updateLevelsLabel)(self, a);

    if(self->total == 9999 || BetterInfo::isFalseTotal(self->searchObject)) self->nextBtn->setVisible(true);

    CCMenu* menu = cast<CCMenu*>(self->nextBtn->getParent());
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    bool isLocal = BetterInfo::isLocal(self->searchObject);

    if((self->searchObject->m_nScreenID == SearchType::kSearchTypeSavedLevels || self->searchObject->m_nScreenID == SearchType::kSearchTypeFavorite) && menu->getChildByTag(filterBtnTag) == nullptr){
        auto filterSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
        filterSprite->setScale(0.7f);
        auto filterButton = gd::CCMenuItemSpriteExtra::create(
            filterSprite,
            self,
            menu_selector(GamingButton::onLevelBrowserSavedFilter)
        );
        menu->addChild(filterButton);
        filterButton->setPosition({- (winSize.width / 2) + 64, 92});
        //filterButton->setScale(0.8f);
        filterButton->setSizeMult(1.2f);
        filterButton->setTag(filterBtnTag);
    }

    if(menu->getChildByTag(starBtnTag)) return;

    if(!BetterInfo::isStarUseless(self->searchObject)) {
        auto starButton = gd::CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png"),
            self,
            menu_selector(GamingButton::onLevelBrowserStar)
        );
        menu->addChild(starButton);
        starButton->setPosition({- (winSize.width / 2) + 30, - (winSize.height / 2) + 58});
        if(!(self->searchObject->m_bStarFilter)) starButton->setColor({125,125,125});
        starButton->setSizeMult(1.2f);
        starButton->setTag(starBtnTag);
    }

    for(unsigned int i = 0; i < menu->getChildrenCount(); i++){
        CCObject* currentObj = menu->getChildren()->objectAtIndex(i);
        if(currentObj != nullptr && currentObj->getTag() == firstBtnTag) {
            CCNode* currentNode = static_cast<CCNode*>(currentObj);
            if(self->searchObject->m_nPage == 0) currentNode->setVisible(false);
            else currentNode->setVisible(true);
            return;
        }
    }

    auto doubleArrowLeft = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    auto arrowLeft = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    arrowLeft->setPosition({0,20});
    doubleArrowLeft->addChild(arrowLeft);
    doubleArrowLeft->setScale(0.5f);
    //doubleArrowLeft->setFlipX(true);
    //arrowLeft->setFlipX(true);
    auto firstBtn = gd::CCMenuItemSpriteExtra::create(
        doubleArrowLeft,
        self,
        menu_selector(GamingButton::onLevelBrowserFirst)
    );
    firstBtn->setTag(firstBtnTag);
    //259 60
    firstBtn->setPosition({ - (winSize.width / 2) + 26, 60});
    if(isLocal) firstBtn->setPosition({ - (winSize.width / 2) + 67, 60});
    if(self->searchObject->m_nPage > 0) menu->addChild(firstBtn);

    if(self->total <= BetterInfo::levelsPerPage(self->searchObject)) return;

    for(unsigned int i = 0; i < menu->getChildrenCount(); i++){
        if(menu->getChildren()->objectAtIndex(i) != nullptr && menu->getChildren()->objectAtIndex(i)->getTag() == randomBtnTag) return;
    }

    CvoltonManager::sharedState()->loadTextures();

    auto randomSprite = BetterInfo::createWithBISpriteFrameName("BI_randomBtn_001.png");
    randomSprite->setScale(0.9f);
    auto randomBtn = gd::CCMenuItemSpriteExtra::create(
        randomSprite,
        self,
        menu_selector(GamingButton::onLevelBrowserRandom)
    );
    randomBtn->setPosition({ (winSize.width / 2) - 23, 88});
    randomBtn->setTag(randomBtnTag);
    if(isLocal) randomBtn->setPosition({(winSize.width / 2) - 58, 122});

    menu->addChild(randomBtn);

    if(!isLocal){
        auto doubleArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        auto arrow = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        arrow->setPosition({31.5,20});
        doubleArrow->addChild(arrow);
        doubleArrow->setScale(0.5f);
        doubleArrow->setFlipX(true);
        arrow->setFlipX(true);
        auto lastBtn = gd::CCMenuItemSpriteExtra::create(
            doubleArrow,
            self,
            menu_selector(GamingButton::onLevelBrowserLast)
        );
        //259 60
        lastBtn->setPosition({ (winSize.width / 2) - 26, 60});
        menu->addChild(lastBtn);
        //lastBtn->setTag(randomBtnTag);
    }
}

/*bool __fastcall LevelBrowserLayer_init(LevelBrowserLayer* self, void* a, GJSearchObject* searchObject) {
    if(!MHook::getOriginal(LevelBrowserLayer_init)(self, a, searchObject)) return false;

    return true;
}*/

void showQuestExclamationMark(CCLayer* creator){
    //if(creator == nullptr || creator->getChildrenCount() < 2) return;
    auto menu = dynamic_cast<CCMenu*>(creator->getChildren()->objectAtIndex(1));
    auto GSM = GameStatsManager::sharedState();

    bool showExclamation = false;
    for(int i = 1; i < 4; i++){
        GJChallengeItem* item = GSM->getChallenge(i);
        if(item != nullptr && item->m_bCanClaim) showExclamation = true;
    }

    if(menu == nullptr || !showExclamation) return;

    auto questBtn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(3));
    if(questBtn == nullptr) return;

    auto existingNode = questBtn->getChildByTag(questBtnExMarkTag);
    if(existingNode != nullptr){
        existingNode->setVisible(true);
        return;
    }

    auto exMark = CCSprite::createWithSpriteFrameName("exMark_001.png");
    exMark->setPosition({16.5,75});
    exMark->setScale(0.7f);
    exMark->setTag(questBtnExMarkTag);
    questBtn->addChild(exMark);
}

bool __fastcall CreatorLayer_init(CCLayer* self) {
    if(!MHook::getOriginal(CreatorLayer_init)(self)) return false;

    //update check
    auto CM = CvoltonManager::sharedState();
    /*CM->doUpdateCheck();*/
    //betterinfo btn
    auto menu = cast<CCMenu*>(self->getChildren()->objectAtIndex(1));

    auto door = cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(12));

    //auto buttonSprite = gd::ButtonSprite::create("Better\nInfo", (int)(100*0.45), true, "bigFont.fnt", "GJ_button_01.png", 120*0.45f, 0.45f);
    auto buttonSprite = BetterInfo::createBISprite("BI_mainButton_001.png");
    buttonSprite->setScale(.9f);
    auto buttonButton = gd::CCMenuItemSpriteExtra::create(
        buttonSprite,
        self,
        menu_selector(GamingButton::onCustomCreatorLayer)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({door->getPositionX() - 1,0});
    buttonButton->setTag(BetterInfo::mainBtnTag);
    menu->addChild(buttonButton);

    //showQuestExclamationMark(self);
    auto alert = CM->updateCompleteDialog();
    if(alert) {
        alert->setZOrder(100);
        self->addChild(alert);
    }

    return true;
}

void __fastcall CreatorLayer_onChallenge(CCLayer* self, void* a, CCMenuItemSpriteExtra* sender) {
    for(unsigned int i = 0; i < sender->getChildrenCount(); i++){
        auto child = dynamic_cast<CCSprite*>(sender->getChildren()->objectAtIndex(i));
        if(child != nullptr && child->getTag() == questBtnExMarkTag) child->setVisible(false);
    }
    MHook::getOriginal(CreatorLayer_onChallenge)(self, a, sender);
}

/*void __fastcall CreatorLayer_onBack(CCLayer* self, void* a, CCMenuItemSpriteExtra* sender) {
    auto CM = CvoltonManager::sharedState();

    MHook::getOriginal(CreatorLayer_onBack)(self, a, sender);
}*/

void _fastcall CreatorLayer_sceneWillResume(uint8_t* self){
    MHook::getOriginal(CreatorLayer_sceneWillResume)(self);

    showQuestExclamationMark(reinterpret_cast<CCLayer*>(self - sizeof(CCLayer)));
    BetterInfo::showBIExclamationMark(reinterpret_cast<CCLayer*>(self - sizeof(CCLayer)));
}

void __fastcall DailyLevelPage_updateTimers(DailyLevelPage* self, void* a, float something) {
    MHook::getOriginal(DailyLevelPage_updateTimers)(self, a, something);

    CCLayer* layer = cast<CCLayer*>(self->getChildren()->objectAtIndex(0));

    if(layer->getChildrenCount() > 11) return;

    auto GM = GameLevelManager::sharedState();
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    std::ostringstream currentDaily;
    currentDaily << "Current: #" << ((self->isWeekly) ? GM->m_weeklyID % 100000 : GM->m_dailyID);
    auto currentDailyNode = CCLabelBMFont::create(currentDaily.str().c_str(), "chatFont.fnt");
    currentDailyNode->setPosition({(winSize.width / 2) + 183, (winSize.height / 2) + 51});
    currentDailyNode->setAnchorPoint({1,0});
    currentDailyNode->setScale(0.6f);
    currentDailyNode->setColor({255,255,255});
    currentDailyNode->setOpacity(200);
    layer->addChild(currentDailyNode);
}

bool __fastcall DailyLevelPage_init(DailyLevelPage* self, void* a, bool isWeekly) {
    if(!MHook::getOriginal(DailyLevelPage_init)(self, a, isWeekly)) return false;

    CvoltonManager::sharedState()->loadTextures();

    CCMenu* menu = self->m_pButtonMenu;

    auto historySprite = BetterInfo::createWithBISpriteFrameName("BI_historyBtn_001.png");
    historySprite->setScale(0.8f);
    auto historyBtn = gd::CCMenuItemSpriteExtra::create(
        historySprite,
        self,
        menu_selector(GamingButton::onDailyHistory)
    );
    historyBtn->setPosition({13, -247});
    menu->addChild(historyBtn);

    return true;
}

bool __fastcall LevelLeaderboard_init(LevelLeaderboard* self, void* a, GJGameLevel* level, int type) { //type is usually an enum but i dont have that rn
    if(!MHook::getOriginal(LevelLeaderboard_init)(self, a, level, type)) return false;

    CCLayer* layer = cast<CCLayer*>(self->getChildren()->objectAtIndex(0));

    //if(layer->getChildrenCount() > 11) return;
    CCMenu* menu = cast<CCMenu*>(layer->getChildren()->objectAtIndex(5));

    auto buttonSprite = gd::ButtonSprite::create((level->dailyID ? "Daily" : "Normal"), 20, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.5);
    auto buttonButton = gd::CCMenuItemSpriteExtra::create(
        buttonSprite,
        self,
        menu_selector(GamingButton::onLeaderboardDaily)
    );
    buttonButton->setSizeMult(1.2f);
    buttonButton->setPosition({196,-87});
    menu->addChild(buttonButton);

    return true;
}

bool __fastcall LeaderboardsLayer_init(ProfilePage* self, void* a, int state){
    if(!MHook::getOriginal(LeaderboardsLayer_init)(self, a, state)) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto layer = cast<CCLayer*>(self->getChildren()->objectAtIndex(0));

    auto refreshBtn = gd::CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"),
        self,
        menu_selector(GamingButton::onLeaderboardRefresh)
    );

    auto menuRefresh = CCMenu::create();
    menuRefresh->addChild(refreshBtn);
    menuRefresh->setPosition({winSize.width - 26.75f, 26.75f});
    menuRefresh->setZOrder(2);

    self->addChild(menuRefresh);

    return true;
}

GJSearchObject* __fastcall LevelSearchLayer_getSearchObject(LevelSearchLayer* self, void* a, SearchType type, std::string str) {

    std::string::const_iterator it = str.begin();
    while (it != str.end() && std::isdigit(*it)) ++it;
    bool isID = !str.empty() && it == str.end();

    //std::ostringstream query;
    //query << "%" << self->input->getString();
    auto CM = CvoltonManager::sharedState();

    if(CM->getOption("search_trim")) {
        str.erase(0, str.find_first_not_of(' '));
        str.erase(str.find_last_not_of(' ') + 1);
    }

    if(CM->getOption("search_no_id") && isID && type == SearchType::kSearchTypeSearch) str = str + "%25";
    if(CM->getOption("search_no_id") && isID && type == SearchType::kSearchTypeFindUsers) str = str + "%20";

    if(CM->getOption("search_surround_percent") && !isID && type == SearchType::kSearchTypeSearch) str = str + "%25%25";
    
    if(CM->getOption("search_contains") && type == SearchType::kSearchTypeSearch && !isID) str = "%25" + str;
    //FLAlertLayer::create(nullptr, "User Info", "OK", nullptr, str)->show();



    return MHook::getOriginal(LevelSearchLayer_getSearchObject)(self, a, type, str);
}

bool __fastcall MoreSearchLayer_init(MoreSearchLayer* self){
    if(!MHook::getOriginal(MoreSearchLayer_init)(self)) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto menu = self->m_pButtonMenu;

    auto sprite = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    sprite->setFlipX(true);

    auto nextBtn = gd::CCMenuItemSpriteExtra::create(
        sprite,
        self,
        menu_selector(GamingButton::onMoreSearchNext)
    );
    nextBtn->setPosition({(winSize.width / 2) + 185, - (winSize.height / 2) + 25});
    menu->addChild(nextBtn);

    return true;
}

/*void __fastcall GameStatsManager_incrementChallenge(void* self, void* a, int challengeType, int count){
    MHook::getOriginal(GameStatsManager_incrementChallenge)(self, a, challengeType, count);

    auto CM = CvoltonManager::sharedState();
    showQuestExclamationMark(CM->getActiveCreator());
}*/

CCArray* __fastcall GameLevelManager_getCompletedLevels(GameLevelManager* self, void* a, bool newFilter){
    auto CM = CvoltonManager::sharedState();
    CompleteMode mode = static_cast<CompleteMode>(CM->getOptionInt("search_completed"));
    if(mode == CompleteMode::modeDefault) return MHook::getOriginal(GameLevelManager_getCompletedLevels)(self, a, newFilter);

    int percentageMin = CM->getOptionInt("search_completed_percentage_min");
    int percentageMax = CM->getOptionInt("search_completed_percentage_max");

    auto coinDict = GameStatsManager::sharedState()->m_verifiedUserCoins;
    auto coinDict2 = GameStatsManager::sharedState()->m_pendingUserCoins;

    CCArray* pRet = CCArray::create();

    auto levels = self->m_onlineLevels;
    CCDictElement* obj;
    CCDICT_FOREACH(levels, obj){
        auto currentLvl = static_cast<GJGameLevel*>(obj->getObject());
        switch(mode){
            case CompleteMode::completed:
                if(currentLvl->normalPercent == 100) pRet->addObject(currentLvl);
                break;
            case CompleteMode::completed21:
                if(currentLvl->orbCompletion == 100) pRet->addObject(currentLvl);
                break;
            case CompleteMode::completed211:
                if(currentLvl->newNormalPercent2 == 100) pRet->addObject(currentLvl);
                break;
            case CompleteMode::allCoins:
            case CompleteMode::noCoins: {
                bool completed = true;
                for(int i = 0; i < currentLvl->coins; i++){
                    bool hasntCoin = coinDict->objectForKey(currentLvl->getCoinKey(i + 1)) == nullptr && coinDict2->objectForKey(currentLvl->getCoinKey(i + 1)) == nullptr;
                    if(hasntCoin) completed = false; else completed = completed && true;
                }
                if(((mode == CompleteMode::noCoins) != completed) && (currentLvl->coins > 0)) pRet->addObject(currentLvl);
                //if(currentLvl->coins > 0) pRet->addObject(currentLvl);
                break;
            }
            case CompleteMode::percentage:
                if((percentageMin == 0 || currentLvl->normalPercent >= percentageMin) && (percentageMax == 0 || currentLvl->normalPercent <= percentageMax)) pRet->addObject(currentLvl);
                break;
        }
    }

    return pRet;
}

CCArray* __fastcall GameLevelManager_getSavedLevels(GameLevelManager* self, void* a, bool filter, int folderID){
    auto CM = CvoltonManager::sharedState();
    CCArray* original = MHook::getOriginal(GameLevelManager_getSavedLevels)(self, a, filter, folderID);

    if(CM->skipSavedFilter) return original;

    CCArray* pRet = CCArray::create();

    //getting config
    std::vector<unsigned int> len;
    for(unsigned int i = 0; i <= 4; i++){
        if(
            CM->getOption(
                CCString::createWithFormat("user_search_len_%02u", i)->getCString()
            )
        ) len.push_back(i);
    }

    std::vector<int> diff;

    if(CM->getOption("user_search_diff_auto")) diff.push_back(-1);
    for(int i = 0; i <= 6; i++){
        if(
            CM->getOption(
                CCString::createWithFormat("user_search_diff_%02i", i)->getCString()
            )
        ) diff.push_back(i);
    }

    std::vector<int> demonDiff;

    for(int i = 0; i <= 5; i++){
        if(
            CM->getOption(
                CCString::createWithFormat("user_search_demon_%02i", i)->getCString()
            )
        ) demonDiff.push_back(i);
    }
    
    //calculating levels
    auto levels = self->m_onlineLevels;
    CCObject* obj;
    CCARRAY_FOREACH(original, obj){
        auto level = static_cast<GJGameLevel*>(obj);

        int difficulty = (level->ratings == 0) ? 0 : (level->ratingsSum / level->ratings);
        if(level->demon != 0) difficulty = 6;
        if(level->autoLevel) difficulty = -1;
        if(!(diff.empty()) && std::find(diff.begin(), diff.end(), difficulty) == diff.end()) continue;

        if(!(len.empty()) && std::find(len.begin(), len.end(), level->levelLength) == len.end()) continue;

        if(CM->getOption("user_search_diff_06") && level->demon != 0) {
            int demonDifficulty = 2;
            if(level->demonDifficulty >= 5) demonDifficulty = level->demonDifficulty - 2;
            else if(level->demonDifficulty >= 3) demonDifficulty = level->demonDifficulty - 3;

            if(!(demonDiff.empty()) && std::find(demonDiff.begin(), demonDiff.end(), demonDifficulty) == demonDiff.end()) continue;
        }

        if(CM->getOption("user_search_star") && level->stars == 0) continue;
        //TODO: respect completed mode
        if(CM->getOption("user_search_uncompleted") && level->normalPercent == 100) continue;
        if(CM->getOption("user_search_completed") && level->normalPercent != 100) continue;
        if(CM->getOption("user_search_featured") && level->featured < 1) continue;
        if(CM->getOption("user_search_original") && level->originalLevel != 0) continue;
        if(CM->getOption("user_search_epic") && !(level->isEpic)) continue;
        if(CM->getOption("user_search_song")) {
            if(CM->getOption("user_search_song_custom") && level->songID != CM->getOptionInt("user_search_song_id")) continue;
            if(!CM->getOption("user_search_song_custom") && (level->audioTrack != CM->getOptionInt("user_search_song_id") || level->songID != 0)) continue;
        }
        //searchObj->m_bSongFilter = CM->getOption("user_search_song");
        if(CM->getOption("user_search_nostar") && level->stars != 0) continue;
        if(CM->getOption("user_search_coins") && (level->coins == 0 || level->coinsVerified == 0)) continue;
        if(CM->getOption("user_search_twoplayer") && !(level->twoPlayerMode)) continue;
        if(CM->getOption("user_search_copied") && level->originalLevel == 0) continue;
        if(CM->getOption("user_search_downloaded") && level->levelString.empty()) continue;
        if(CM->getOption("user_search_ldm") && !(level->lowDetailMode)) continue;
        if(CM->getOption("user_search_idrange")) {
            int min = CM->getOptionInt("user_search_idrange_min");
            int max = CM->getOptionInt("user_search_idrange_max");
            if(min != 0 && level->levelID < min) continue;
            if(max != 0 && level->levelID > max) continue;
        }

        pRet->addObject(level);
    }

    return pRet;
}

void __fastcall GameLevelManager_limitSavedLevels(GameLevelManager* self){
    auto CM = CvoltonManager::sharedState();
    CM->skipSavedFilter = true;

    MHook::getOriginal(GameLevelManager_limitSavedLevels)(self);

    CM->skipSavedFilter = false;
}

bool __fastcall RewardsPage_init(FLAlertLayer* self){
    if(!MHook::getOriginal(RewardsPage_init)(self)) return false;

    CvoltonManager::sharedState()->loadTextures();

    auto historySprite = BetterInfo::createWithBISpriteFrameName("BI_historyBtn_001.png");
    historySprite->setScale(0.8f);
    auto historyBtn = gd::CCMenuItemSpriteExtra::create(
        historySprite,
        self,
        menu_selector(GamingButton::onRewardsPageHistory)
    );
    historyBtn->setPosition({-147,-88});
    self->m_pButtonMenu->addChild(historyBtn);

    return true;
}

/*void LevelBrowserLayer_loadLevelsFinished(LevelBrowserLayer* self, void* a, CCArray* levels, const char* a2){
    MHook::getOriginal(GameLevelManager_getCompletedLevels)(self, a, levels, a2);

    auto CM = CvoltonManager::sharedState();
    if(CM->searchInProgress){
        
    }
}*/

void setupPageLimitBypass(){
    auto proc = GetCurrentProcess();
    auto winapiBase = reinterpret_cast<char*>(base);
    //This patches the maximum number for SetIDPopup to INT_MAX
    unsigned char patch[] = {0x68, 0xFF, 0xFF, 0xFF, 0x7F};
    WriteProcessMemory(proc, winapiBase + 0x1431F6, patch, 5, NULL);
    //This patches the amount of characters allowed in the text input in SetIDPopup to 6 characters
    unsigned char patch2[] = {0x06};
    WriteProcessMemory(proc, winapiBase + 0x14371C, patch2, 1, NULL);
}

void setupDailyNew(){
    auto proc = GetCurrentProcess();
    auto winapiBase = reinterpret_cast<char*>(base);
    //This moves the position of the New! displayed when there's a new daily from 182 (0x43360000) to 130 (0x43020000)
    unsigned char patch[] = {0xC7, 0x04, 0x24, 0x00, 0x00, 0x02, 0x43};
    WriteProcessMemory(proc, winapiBase + 0x6C709, patch, 7, NULL);
}

DWORD WINAPI my_thread(void* hModule) {

    /*AllocConsole();
    std::ofstream conout("CONOUT$", std::ios::out);
    std::ifstream conin("CONIN$", std::ios::in);
    std::cout.rdbuf(conout.rdbuf());
    std::cin.rdbuf(conin.rdbuf());*/


    MH_Initialize();

    /*
        OK so this sleep might seem random and that is because it is.
        The reality is that when you are loading multiple mods at once,
        they all register hooks in their own threads, which leads to
        a race condition where a mod ends up overwriting the hook
        of a different mod instead of adding onto it. This should basically
        make sure that we do not hook at the same time a different mod is
        already attempting to hook.

        The random number generator is added on top of it just as a meme
        and also because I kinda suspect that someone is going to rip off
        my solution and if I were to use a static value, it wouldn't work
        with their mod then.
    */
    /*std::random_device os_seed;
    const unsigned int seed = os_seed();

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribute(1000, 5000);
    int sleepMs = distribute(generator);
    Sleep(sleepMs);*/
    
    /*
        Hook initialization
    */
    MHook::registerHook(base + 0x14F5A0, InfoLayer_init);
    MHook::registerHook(base + 0x151500, InfoLayer_onMore);
    MHook::registerHook(base + 0x151850, InfoLayer_onLevelInfo);
    MHook::registerHook(base + 0x152270, InfoLayer_setupCommentsBrowser);
    MHook::registerHook(base + 0x151E70, InfoLayer_loadPage);
    //setupProgressBars = very bad workaround for interoperability with gdshare lol (help how do i hook something thats already hooked)
    MHook::registerHook(base + 0x15C350, LevelBrowserLayer_updateLevelsLabel);
    //MHook::registerHook(base + 0x15A040, LevelBrowserLayer_init);
    MHook::registerHook(base + 0x177FC0, LevelInfoLayer_setupProgressBars);
    MHook::registerHook(base + 0x17AC90, LevelInfoLayer_onViewProfile);
    MHook::registerHook(base + 0x17ACF0, LevelInfoLayer_onLevelInfo);
    MHook::registerHook(base + 0x70660, EditLevelLayer_onLevelInfo);
    MHook::registerHook(base + 0x189070, LevelPage_onInfo);
    MHook::registerHook(base + 0x5C790, LevelCell_onViewProfile);
    MHook::registerHook(base + 0x5A020, LevelCell_loadCustomLevelCell);
    MHook::registerHook(base + 0x5F3D0, CommentCell_loadFromComment);
    MHook::registerHook(base + 0x61140, CommentCell_onConfirmDelete);
    MHook::registerHook(base + 0x61260, CommentCell_FLAlert_Clicked);
    MHook::registerHook(base + 0x60F90, CommentCell_onLike);
    MHook::registerHook(base + 0x61070, CommentCell_likedItem);
    MHook::registerHook(base + 0x210040, ProfilePage_loadPageFromUserInfo);
    MHook::registerHook(base + 0x20EF00, ProfilePage_init);
    //MHook::registerHook(base + 0x211BB0, ProfilePage_onMyLevels);
    MHook::registerHook(base + 0xA1C20, GameLevelManager_userNameForUserID);
    MHook::registerHook(base + 0x4DE40, CreatorLayer_init);
    MHook::registerHook(base + 0x4F1B0, CreatorLayer_onChallenge);
    //MHook::registerHook(base + 0x4FAE0, CreatorLayer_onBack);
    MHook::registerHook(base + 0x4FB50, CreatorLayer_sceneWillResume); //onLeaderboards 0x4ED20
    MHook::registerHook(base + 0x6BEF0, DailyLevelPage_updateTimers);
    MHook::registerHook(base + 0x6A900, DailyLevelPage_init);
    MHook::registerHook(base + 0x17C4F0, LevelLeaderboard_init); //0x17D090 onChangeType
    MHook::registerHook(base + 0x1587B0, LeaderboardsLayer_init);
    MHook::registerHook(base + 0x1805F0, LevelSearchLayer_getSearchObject); //17F500 onMoreOptions
    MHook::registerHook(base + 0x1825C0, MoreSearchLayer_init); 
    MHook::registerHook(base + 0xA2D20, GameLevelManager_getCompletedLevels); 
    MHook::registerHook(base + 0xA2960, GameLevelManager_getSavedLevels); 
    MHook::registerHook(base + 0xA43B0, GameLevelManager_limitSavedLevels);
    MHook::registerHook(base + 0x2178F0, RewardsPage_init);
    //MHook::registerHook(base + 0x180FC0, LevelSearchLayer_onSearch);
    //MHook::registerHook(base + 0xF9AE0, GameStatsManager_incrementChallenge);
    //MHook::registerHook(base + 0x2133E0, ProfilePage_getUserInfoFailed);

    /*
        Byte patches
    */
    setupPageLimitBypass();
    setupDailyNew();

    MH_EnableHook(MH_ALL_HOOKS);

    /*std::getline(std::cin, std::string());

    MH_Uninitialize();
    conout.close();
    conin.close();
    FreeConsole();
    FreeLibraryAndExitThread(cast<HMODULE>(hModule), 0);*/

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, my_thread, hModule, 0, 0);
    return TRUE;
}
