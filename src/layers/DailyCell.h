#pragma once

#include <gd.h>

class DailyCell : public gd::TableViewCell {
        gd::GJGameLevel* level;
        DailyCell(const char* name, cocos2d::CCSize size);
        void draw() override;
        void onView(cocos2d::CCObject* sender);
        void onLeaderboards(cocos2d::CCObject* sender);
        void onInfo(cocos2d::CCObject* sender);
        int getTotalDiamonds();
        int getAwardedDiamonds();
    
    public:
        void loadFromLevel(gd::GJGameLevel* level);
        static DailyCell* create(const char* key, cocos2d::CCSize size);
};