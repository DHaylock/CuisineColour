#pragma once

#include "ofMain.h"
#include "ofxInstagram.h"
#include "ofxThreadedImageLoader.h"
#include "ImageSelector.h"
#include "ofxSyphon.h"
#include "ofxTwitter.h"
#include "ImageController.h"
#include "FileWatcher.h"
#include "ImagePool.h"
#include "ofxUI.h"
#include "ofxSimpleTimer.h"

class ofApp : public ofBaseApp {
    
public:
    void setup();
    void update();
    void draw();
    
    void exit();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    
    //-----------------------------------------------
    // *
    // *                  Web Stuff
    // * Will include instagram and twitter collection
    // * elements
    //-----------------------------------------------
    ofxInstagram instagram;
    ofxTwitter twitter;
    
    ofxThreadedImageLoader instagramLoader;
    ofxThreadedImageLoader twitterLoader;
    ofxThreadedImageLoader tumblrLoader;
    
    deque <ImageSelector> twitterImages;
    deque <ImageSelector> instagramImages;
    deque <ImageSelector> tumblrImages;
    deque <string> imgURL;
    
    //-----------------------------------------------
    // *
    // *                  Main Image Stuff
    // *
    //-----------------------------------------------
    deque<ImageController> outputs;
    FileWatcher fileWatcher;
    ImagePool pool;
    bool previewSmallImage;
    ofxSyphonServer server;
    ofTexture screenOutput;
    ofImage logo;
    
    std::string twitterReturn;
    
    deque<string> listOfItemsAlreadyLoaded;
    
    void drawImagePool();
    void drawTwitterPool();
    void drawInstagramPool();
    void drawPreview1();
    void drawPreview2();
    void drawLogoArea();
    void drawResponseData();
    void drawInformation();
    
    ofTrueTypeFont infoFont;
    int whichToAddTo;
    
    ofTexture screenCapture;
    
    //-----------------------------------------------
    // *
    // *                 Credentials
    // *
    //-----------------------------------------------
    void loadCredentials();
    string _instagramOAuth;
    string _twitterConsumerKey;
    string _twitterConsumerSecret;
    string _twitterAccessToken;
    string _twitterAccessTokenSecret;
    
    //-----------------------------------------------
    // *
    // *               User Interface
    // *
    //-----------------------------------------------
    void setupGUI();
    ofxUICanvas *gui;
    ofxUICanvas *animationGui;
    ofxUICanvas *selectionGui;
    
    void guiEvent(ofxUIEventArgs &e);
    bool _activateSyphon;
    
    
    ofColor buttonColor;
    bool _AutoRun;
    int _timer;
    bool latchTimer;
    void autoRun();
    bool autoRunlatch;
    ofxSimpleTimer autoRunTimer;
    ofxSimpleTimer autoRunDelay;
    
    void timerComplete(int &args);
    void timerStarted(int &args);
    
    void delayTimerComplete(int &args);
    void delayTimerStarted(int &args);
    
    //-----------------------------------------------
    // *
    // *                 Animations
    // *
    //-----------------------------------------------
    int cycleCounter;
    int _animationMethod;
    bool canSeq;
    int animationTime;
    int bare;
    void animate1to2(int frame);
    void animate2to1(int frame);
    
    void animateAll1to2();
    void animateAll2to1();
    
    void animateSequentiallyTopToBottom1to2();
    void animateSequentiallyTopToBottom2to1();
    
    void animateSequentiallyBottomToTop1to2();
    void animateSequentiallyBottomToTop2to1();
    
    void animateSequentiallyLeftToRight1to2();
    void animateSequentiallyLeftToRight2to1();
    
    void animateSequentiallyRightToLeft1to2();
    void animateSequentiallyRightToLeft2to1();
    
    void animateSelectedSequentially1to2();
    void animateSelectedSequentially2to1();
    
    void animateSelectedToWhite();
    void animateSelectedToImage();
    
    void animateSelectedSequentiallyToWhite();
    void animateSelectedSequentiallyToImage();
    
    vector <int> selectedOutputs;
    bool bMoveOrFade;
    //-----------------------------------------------
    // *
    // *                 API Calls
    // *
    //-----------------------------------------------
    void getInstagramLikes();
    void getInstagramUserFeed();
    void getInstagramQuery();
    void getInstagramFeed();
    
    void getTwitterFavourites();
    void getTwitterRetweets();
    void getTwitterUserStream();
    void getTwitterQuery();
};
