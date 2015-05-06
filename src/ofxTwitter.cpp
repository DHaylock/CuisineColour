//
//  ofxTwitter.cpp
//  CusineColor
//
//  Created by David Haylock on 09/02/2015.
//
//

#include "ofxTwitter.h"
//--------------------------------------------------------------
void ofxTwitter::setup(const std::string& consumerKey,const std::string& consumerSecret, const std::string& AccessToken, const std::string& AccessTokenSecret)
{
    ofxOAuth::setup("https://api.twitter.com",consumerKey, consumerSecret);
    ofxOAuth::setAccessToken(AccessToken);
    ofxOAuth::setAccessTokenSecret(AccessTokenSecret);
}
//--------------------------------------------------------------
deque<string> ofxTwitter::getImgID()
{
    deque<string>elements;
    for(unsigned int i = 0; i < json.size(); ++i) {
        if (!json[i]["entities"]["media"].empty()) {
            std::string title  = json[i]["entities"]["media"][0]["id"].asString();
            cout << title << endl;
            elements.push_back(title);
        }
    }
    return elements;
}
//--------------------------------------------------------------
deque<string> ofxTwitter::getImgUrl()
{
    deque<string>elements;
    for(unsigned int i = 0; i < json.size(); ++i) {
        if (!json[i]["entities"]["media"].empty()) {
            std::string title  = json[i]["entities"]["media"][0]["media_url"].asString();
            cout << title << endl;
            elements.push_back(title);
        }
    }
    return elements;
}
//--------------------------------------------------------------
deque<string> ofxTwitter::getQueryImgUrl()
{
    deque<string>elements;
    for(unsigned int i = 0; i < json["statuses"].size(); ++i) {
        if (!json["statuses"][i]["entities"]["media"].empty()) {
            std::string title  = json["statuses"][i]["entities"]["media"][0]["media_url"].asString();
            cout << title << endl;
            elements.push_back(title);
        }
    }
    return elements;
}
//--------------------------------------------------------------
deque<string> ofxTwitter::getQueryImgID()
{
    deque<string>elements;
    for(unsigned int i = 0; i < json["statuses"].size(); ++i) {
        if (!json["statuses"][i]["entities"]["media"].empty()) {
            std::string title  = json["statuses"][i]["entities"]["media"][0]["id"].asString();
            cout << title << endl;
            elements.push_back(title);
        }
    }
    return elements;
}
//--------------------------------------------------------------
std::string ofxTwitter::getQueryMethod(string query, int count)
{
    stringstream request;
    request << "/1.1/search/tweets.json?q=" << query <<"&count=" << ofToString(count) << endl;
    std::string returnString = ofxOAuth::get(request.str());
    json.parse(returnString);
    return ofxJSONElement(returnString).toStyledString();
}
#pragma mark - Statuses
//--------------------------------------------------------------
std::string ofxTwitter::getStatusesMentionsTimeline(int count)
{
    stringstream request;
    request << "/1.1/statuses/mentions_timeline.json?";
    request << "count="<< ofToString(count);
    std::string returnString = ofxOAuth::get(request.str());
    return ofxJSONElement(returnString).toStyledString();
}
//--------------------------------------------------------------
std::string ofxTwitter::getUserTimelineStatuses(string user,int count)
{    
    stringstream request;
    request << "/1.1/statuses/user_timeline.json?screen_name=" << user << "&count=" << ofToString(count) << endl;
    std::string returnString = ofxOAuth::get(request.str());
    json.parse(returnString);
    cout << ofxJSONElement(returnString).toStyledString() << endl;
    return ofxJSONElement(returnString).toStyledString();
}
//--------------------------------------------------------------
std::string ofxTwitter::getStatusesHomeTimeline(int count)
{
    stringstream request;
    request << "/1.1/statuses/home_timeline.json?";
    request << "count="<< ofToString(count);
    std::string returnString = ofxOAuth::get(request.str());
    json.parse(returnString);
        cout << ofxJSONElement(returnString).toStyledString() << endl;
    return ofxJSONElement(returnString).toStyledString();
}
//--------------------------------------------------------------
std::string ofxTwitter::getStatusesRetweetsOfMe(int count)
{
    stringstream request;
    request << "/1.1/statuses/retweets_of_me.json?";
    request << "count="<< ofToString(count);
    std::string returnString = ofxOAuth::get(request.str());
    json.parse(returnString);
    return ofxJSONElement(returnString).toStyledString();
}
//--------------------------------------------------------------
std::string ofxTwitter::getStatusesFavourites(string user,int count)
{
    stringstream request;
    request << "/1.1/favorites/list.json?";
    request << "count="<< ofToString(count);
    request << "&screen_name=" << user;
    std::string returnString = ofxOAuth::get(request.str());
    json.parse(returnString);
    return ofxJSONElement(returnString).toStyledString();
}