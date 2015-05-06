//
//  ofxTwitter.h
//  CusineColor
//
//  Created by David Haylock on 09/02/2015.
//
//

#include <stdio.h>
#include "ofMain.h"
#include "ofxOAuth.h"
#include "ofxJSON.h"
#include "json.h"
class ofxTwitter : public ofxOAuth
{
    
public:
    //--------------------------------------------------------------
    ofxTwitter(){   }
    //--------------------------------------------------------------
    ~ofxTwitter(){   }
    //--------------------------------------------------------------

    void setup(const std::string& consumerKey,const std::string& consumerSecret,const std::string& AccessToken,const std::string& AccessTokenSecret);
    
    std::string getQueryMethod(string query, int count = 20);
    std::string getUserTimelineStatuses(string user, int count = 68);
    
    std::string getStatusesMentionsTimeline(int count = 50);
    std::string getStatusesHomeTimeline(int count = 50);
    std::string getStatusesRetweetsOfMe(int count = 50);
    std::string getStatusesFavourites(string user,int count = 50);
    
    std::string updateStatusWithPhoto(const string &path);
    
    deque<string> getQueryImgUrl();
    deque<string> getImgUrl();
    deque<string> getImgID();
    
    deque<string> getQueryImgID();
    ofxJSONElement json;
};


