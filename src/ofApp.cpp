#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::exit()
{
    // Kill all the loaders
    twitterLoader.stopThread();
    instagramLoader.stopThread();
    
    // Clear the Images from the Outputs
    for (int i = 0; i < outputs.size(); i++) {
        outputs[i].deleteImage1();
        outputs[i].deleteImage2();
    }
    
    // Save off the GUI Settings
    gui->saveSettings("GUI/gui.xml");
    animationGui->saveSettings("GUI/animationGui.xml");
    selectionGui->saveSettings("GUI/selectionGui.xml");
    
    // Delete the GUI
    delete gui;
    delete animationGui;
    delete selectionGui;
}
#pragma mark - Credentials
//--------------------------------------------------------------
void ofApp::loadCredentials()
{
    ofxJSONElement data;
    // Open the Credentials File and pull out data
    data.open("Credentials/cred.json");
    
    _instagramOAuth = data["instagramOAuth"].asString();
    _twitterConsumerKey = data["twitterConsumerKey"].asString();
    _twitterConsumerSecret = data["twitterConsumerSecret"].asString();
    _twitterAccessToken = data["twitterAccessToken"].asString();
    _twitterAccessTokenSecret = data["twitterAccessTokenSecret"].asString();
}
#pragma mark - Main
//--------------------------------------------------------------
void ofApp::setup()
{
    
    // Setup the Screen
    ofSetWindowShape(ofGetScreenWidth(),ofGetScreenHeight());
    ofSetWindowPosition(0, 0);
    ofSetFrameRate(60);
    ofSetWindowTitle("Cuisine Color App");
    
    // Don't want to know unless its fatal
    ofSetLogLevel(OF_LOG_ERROR);
    
    // Create the GUI's
    setupGUI();
    
    // Loads the oauth stuff from json file
    loadCredentials();
    
    infoFont.loadFont("GUI/NewMedia Fett.ttf", 20);
    
    // Instagram Setup
    instagram.setup(_instagramOAuth,"self");
    
    // Make sure the crt file is in the data folder otherwise the curl requests wont work
    instagram.setCertFileLocation(ofToDataPath("ca-bundle.crt",false));
    
    // Twitter Stuff
    twitter.setup(_twitterConsumerKey, _twitterConsumerSecret,_twitterAccessToken,_twitterAccessTokenSecret);
    
    // As it Sounds setup the syphon elements
    server.setName("Screen Output");
    
    // File Watcher Setup
    fileWatcher.setup("Photos","Gifs");
    
    // Setup the Image pools
    pool.setup(10,720);
    pool.getListOfFilesCurrentlyInDirectory(fileWatcher.getAllJpgFiles());
    
    // Area for logos loading
    logo.loadImage("arts.jpg");
    logo.resize(logo.getWidth()/2, logo.getHeight()/2);
    
    // Timer Setup
    ofAddListener(autoRunTimer.TIMER_COMPLETE, this, &ofApp::timerComplete);
    ofAddListener(autoRunTimer.TIMER_STARTED, this, &ofApp::timerStarted);
    ofAddListener(autoRunDelay.TIMER_COMPLETE, this, &ofApp::delayTimerComplete);
    ofAddListener(autoRunDelay.TIMER_STARTED, this, &ofApp::delayTimerStarted);
    
    // Set Variables
    bare = 0;
    whichToAddTo = 0;
    canSeq = false;
    latchTimer = true;
    previewSmallImage = false;
    autoRunlatch = false;
    autoRunTimer.setup(10000);
    autoRunDelay.setup(2500);
    
    // Create Output Frames
    int posx = 10;
    int posy = 10;
    int spacing = 330;
    
    ImageController s;
    for (int i = 0; i < 7; i++) {
        s.setControllerID(i);
        s.setup(posx+(i*spacing), 10, 320, 320);
        outputs.push_back(s);
    }
    for (int i = 0; i < 7; i++) {
        s.setControllerID(i+3);
        s.setup(posx+(i*spacing), 10+spacing, 320, 320);
        outputs.push_back(s);
    }
    
    ofImage imgLogo;
    imgLogo.loadImage("TestPattern.jpg");
    
    // Populate with project Image
    for(int i = 0; i < outputs.size(); i++) {
        outputs[i].setSlot1(CUSINE_IMAGE);
        outputs[i].setSlot2(CUSINE_IMAGE);
        outputs[i].addImage1(imgLogo);
        outputs[i].addImage2(imgLogo);
        outputs[i].animate2to1(ANIMATION_TYPE_LINEAR);
    }
    
    // Screen Capturing
    screenCapture.allocate((7*330),(2*330),GL_RGBA);
    screenOutput.allocate((7*340)+10,(2*330)+10,GL_RGBA);
    
    cycleCounter = outputs.size();
    
    // Kill the Threads until ready
    twitterLoader.stopThread();
    instagramLoader.stopThread();
    
    ((ofxUIToggleMatrix *)selectionGui->getWidget("Frame1"))->setColorFill(ofColor::peachPuff);
    ((ofxUIToggleMatrix *)selectionGui->getWidget("Frame1"))->setColorBack(ofColor::black);
    ((ofxUIToggleMatrix *)selectionGui->getWidget("Frame1"))->setColorFillHighlight(ofColor::peachPuff);
    ((ofxUIToggleMatrix *)selectionGui->getWidget("Frame1"))->setDrawBack(true);
    ((ofxUIToggleMatrix *)selectionGui->getWidget("Frame1"))->setDrawOutline(true);
    ((ofxUIToggleMatrix *)selectionGui->getWidget("Frame1"))->setDrawPaddingOutline(true);
    
    ((ofxUIToggleMatrix *)animationGui->getWidget("AnimateSingleFrames"))->setColorBack(ofColor::peachPuff);
    ((ofxUIToggleMatrix *)animationGui->getWidget("AnimateSingleFrames"))->setColorFillHighlight(ofColor::peachPuff);
    ((ofxUIToggleMatrix *)animationGui->getWidget("AnimateSingleFrames"))->setDrawOutline(true);
    ((ofxUIToggleMatrix *)animationGui->getWidget("AnimateSingleFrames"))->setDrawPaddingOutline(true);
    
    gui->loadSettings("GUI/gui.xml");
    animationGui->loadSettings("GUI/animationGui.xml");
    selectionGui->loadSettings("GUI/selectionGui.xml");
}
//--------------------------------------------------------------
void ofApp::update()
{
    if (_AutoRun) {
        autoRun();
        autoRunTimer.update();
        autoRunDelay.update();
        buttonColor.set(20, (int)(60 + 60 * sin(ofGetElapsedTimef()*4)), 0);
        ((ofxUILabelToggle *)gui->getWidget("Auto Run"))->setColorFill(buttonColor);
    }
    else {
        ((ofxUILabelToggle *)gui->getWidget("Auto Run"))->setColorBack(ofColor::red);
    }
    
    // Update the Output Frames
    for (int i = 0; i < outputs.size(); i++) {
        outputs[i].setAnimationLength(animationTime);
        outputs[i].setMoveOrFade(bMoveOrFade);
        outputs[i].update();
    }
    
    // Watch the files for New Images
    if (fileWatcher.isNewJpgFile()) {
        pool.addNewImage(fileWatcher.getNewJpgFileName());
    }
    
    // IMPORTANT: This is the animation hunk
    // Only Active if canSeq is true
    if(canSeq) {
        switch (_animationMethod) {
            case 0:
                animateSequentiallyTopToBottom1to2();
                break;
            case 1:
                animateSequentiallyTopToBottom2to1();
                break;
            case 2:
                animateSequentiallyBottomToTop1to2();
                break;
            case 3:
                animateSequentiallyBottomToTop2to1();
                break;
            case 4:
                animateSequentiallyLeftToRight1to2();
                break;
            case 5:
                animateSequentiallyLeftToRight2to1();
                break;
            case 6:
                animateSequentiallyRightToLeft1to2();
                break;
            case 7:
                animateSequentiallyRightToLeft2to1();
                break;
            case 8:
                animateSelectedSequentially1to2();
                break;
            case 9:
                animateSelectedSequentially2to1();
                break;
            case 10:
                animateSelectedSequentiallyToWhite();
                break;
            case 11:
                animateSelectedSequentiallyToImage();
                break;
            default:
                break;
        }
    }
    
    // Kills the Animation loop
    if ((cycleCounter == outputs.size() || 0)) {
        canSeq = false;
    }
}
//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(0);
    
    // This is where the outputs are Drawn
    for (int i = 0; i < outputs.size(); i++) {
        outputs[i].draw();
    }
    
    // Grab the Screen Data
    screenOutput.loadScreenData(0, 0, (7*330)+10, (2*330)+10);
    
    // Self explanatory
    server.publishTexture(&screenOutput);
    
    // For reference and acknowledgment that the frame is selected
    for(int i = 0; i < outputs.size(); i++) {
        outputs[i].drawOutline();
    }
    
    // Self Explantory draws the separate pools of images
    drawImagePool();
    drawTwitterPool();
    drawInstagramPool();
    drawPreview1();
    drawPreview2();
}
#pragma mark - Helper
//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    switch (key) {
        case OF_KEY_TAB:
            gui->toggleVisible();
            animationGui->toggleVisible();
            selectionGui->toggleVisible();
            break;
        case 'c':
            for(int i = 0; i < outputs.size(); i++) {
                outputs[i].isSelected(false);
                ((ofxUIToggleMatrix *)selectionGui->getWidget("Frame1"))->setAllToggles(false);
                ((ofxUIRadio *)selectionGui->getWidget("AddorNot"))->activateToggle("Can't Add");
            }
            break;
            // Select all of the Frames
        case 'a':
            for(int i = 0; i < outputs.size(); i++) {
                outputs[i].isSelected(true);
                ((ofxUIToggleMatrix *)selectionGui->getWidget("Frame1"))->setAllToggles(true);
            }
            break;
            
            // Select all of the Frames
        case 'w':
            for(int i = 0; i < outputs.size(); i++) {
                outputs[i].fadeWhiteIn(bare);
            }
            break;
        case 'b':
            for(int i = 0; i < outputs.size(); i++) {
                outputs[i].fadeWhiteOut(bare);
            }
            break;
        case '1':
            for(int i = 0; i < outputs.size(); i++) {
                if (outputs[i].isEditable()) {
                    outputs[i].addImage1(pool.getImage(ofRandom(pool.imagesInFolder.size())).getPixelsRef());
                }
            }
            break;
        case '2':
            for(int i = 0; i < outputs.size(); i++) {
                if (outputs[i].isEditable()) {
                    outputs[i].addImage2(pool.getImage(ofRandom(pool.imagesInFolder.size())).getPixelsRef());
                }
            }
            break;
        default:
            break;
    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key) {      }
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) { pool.isMouseOver(x, y); }
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {       }
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
    //-------------------------------------------------
    // *
    // *    Saves the web images onto the Harddrive
    // *
    //-------------------------------------------------
    // Instagram
    for (int i = 0; i < instagramImages.size(); i++) {
        if(instagramImages[i].isClicked(x, y)){
            ofSaveImage(instagramImages[i], ofToDataPath("Photos/"+instagramImages[i].getImageCaption()+".jpg"));
        }
    }
    // Twitter
    for (int i = 0; i < twitterImages.size(); i++) {
        if(twitterImages[i].isClicked(x, y)){
            ofSaveImage(twitterImages[i], ofToDataPath("Photos/"+twitterImages[i].getImageCaption()+".jpg"));
        }
    }
    
    //--------------------------------------------------------------
    // *
    // * IMPORTANT: This gives specific access to the output frames
    // *
    //--------------------------------------------------------------
    switch (whichToAddTo) {
        case 0:
            // Can't do anything in this setting
            break;
        case 1:
            pool.returnClickedImage().clear();
            for (int a = 0; a < outputs.size(); a++) {
                if (outputs[a].isEditable()) {
                    if(pool.isClicked(x, y)) {
                        outputs[a].setSlot1(CUSINE_IMAGE);
                        outputs[a].addImage1(pool.returnClickedImage());
                        outputs[a].setImage1ID(pool.getImageName());
                    }
                }
            }
            whichToAddTo = 0;
            ((ofxUIRadio *)selectionGui->getWidget("AddorNot"))->activateToggle("Can't Add");
            break;
        case 2:
            pool.returnClickedImage().clear();
            for (int a = 0; a < outputs.size(); a++) {
                if (outputs[a].isEditable()) {
                    if(pool.isClicked(x, y)) {
                        outputs[a].setSlot2(CUSINE_IMAGE);
                        outputs[a].addImage2(pool.returnClickedImage());
                        outputs[a].setImage2ID(pool.getImageName());
                    }
                }
            }
            whichToAddTo = 0;
            ((ofxUIRadio *)selectionGui->getWidget("AddorNot"))->activateToggle("Can't Add");
            break;
        default:
            break;
    }
}
#pragma mark - API Calls
//-------------------------------------------------------------
// *
// *                      API Calls
// *
//-------------------------------------------------------------
void ofApp::getInstagramFeed()
{
    int x = 1070;
    int y = 720;
    instagramLoader.startThread();
    instagramImages.clear();
    instagramImages.resize(20);
    instagram.getUserRecentMedia("self");
    if (!instagram.getImageURL().empty()) {
        for ( int i = 0; i < instagram.getImageURL().size(); i++) {
            instagramLoader.loadFromURL(instagramImages[i], instagram.getImageURL()[i]);
            if (i < instagram.getImageURL().size()/2) {
                instagramImages[i].setup(x, y+(i*76),75,75);
            }
            else{
                instagramImages[i].setup(x+75, y+((i-10)*76), 75, 75);
            }
            instagramImages[i].setImageCaption(instagram.getImageID()[i]);
        }
    }
}
//-------------------------------------------------------------
void ofApp::getInstagramLikes()
{
    int x = 1070;
    int y = 720;
    instagramLoader.startThread();
    instagramImages.clear();
    instagramImages.resize(20);
    instagram.getUserLikedMedia(20);
    if (!instagram.getImageURL().empty()) {
        for ( int i = 0; i < instagram.getImageURL().size(); i++) {
            instagramLoader.loadFromURL(instagramImages[i], instagram.getImageURL()[i]);
            if (i < instagram.getImageURL().size()/2) {
                instagramImages[i].setup(x, y+(i*76),75,75);
            }
            else{
                instagramImages[i].setup(x+75, y+((i-10)*76), 75, 75);
            }
            instagramImages[i].setImageCaption(instagram.getImageID()[i]);
        }
    }
}
//-------------------------------------------------------------
void ofApp::getInstagramUserFeed()
{
    int x = 1070;
    int y = 720;
    instagramLoader.startThread();
    instagramImages.clear();
    instagramImages.resize(20);
    instagram.getUserRecentMedia("self");
    if (!instagram.getImageURL().empty()) {
        for ( int i = 0; i < instagram.getImageURL().size(); i++) {
            instagramLoader.loadFromURL(instagramImages[i], instagram.getImageURL()[i]);
            if (i < instagram.getImageURL().size()/2) {
                instagramImages[i].setup(x, y+(i*76),75,75);
            }
            else{
                instagramImages[i].setup(x+75, y+((i-10)*76), 75, 75);
            }
            instagramImages[i].setImageCaption(instagram.getImageID()[i]);
        }
    }
}
//-------------------------------------------------------------
void ofApp::getInstagramQuery()
{
    int x = 1070;
    int y = 720;
    instagramLoader.startThread();
    instagramImages.clear();
    instagramImages.resize(20);
    instagram.getListOfTaggedObjectsNormal(((ofxUILabel * )gui->getWidget("Instagram Term"))->getLabel(), 20);
    if (!instagram.getImageURL().empty()) {
        for ( int i = 0; i < instagram.getImageURL().size(); i++) {
            instagramLoader.loadFromURL(instagramImages[i], instagram.getImageURL()[i]);
            if (i < instagram.getImageURL().size()/2) {
                instagramImages[i].setup(x, y+(i*76),75,75);
            }
            else{
                instagramImages[i].setup(x+75, y+((i-10)*76), 75, 75);
            }
            instagramImages[i].setImageCaption(instagram.getImageID()[i]);
        }
    }
}
//-------------------------------------------------------------
void ofApp::getTwitterFavourites()
{
    int x = 860;
    int y = 720;
    
    twitterLoader.startThread();
    twitterImages.clear();
    twitterImages.resize(20);
    twitter.getStatusesFavourites("@User",20);
    if(!twitter.getImgUrl().empty())
    {
        for (int i = 0; i < twitter.getImgUrl().size();i++) {
            twitterLoader.loadFromURL(twitterImages[i], twitter.getImgUrl()[i]);
            twitterImages[i].setImageCaption(twitter.getImgID()[i]);
            if (i < twitter.getImgUrl().size()/2) {
                twitterImages[i].setup(x, y+(i*76),75,75);
            }
            else{
                twitterImages[i].setup(x+75, y+((i-twitter.getImgUrl().size()/2)*76), 75, 75);
            }
        }
    }
    else {
        cout << "No tags" << endl;
    }
}
//-------------------------------------------------------------
void ofApp::getTwitterRetweets()
{
    int x = 860;
    int y = 720;
    
    twitterLoader.startThread();
    twitterImages.clear();
    twitterImages.resize(20);
    twitterReturn = twitter.getStatusesRetweetsOfMe(20);
    if(!twitter.getImgUrl().empty())
    {
        for (int i = 0; i < twitter.getImgUrl().size();i++) {
            twitterLoader.loadFromURL(twitterImages[i], twitter.getImgUrl()[i]);
            twitterImages[i].setImageCaption(twitter.getImgID()[i]);
            if (i < twitter.getImgUrl().size()/2) {
                twitterImages[i].setup(x, y+(i*76),75,75);
            }
            else{
                twitterImages[i].setup(x+75, y+((i-twitter.getImgUrl().size()/2)*76), 75, 75);
            }
        }
    }
    else {
        cout << "No tags" << endl;
    }
}
//-------------------------------------------------------------
void ofApp::getTwitterQuery()
{
    int x = 860;
    int y = 720;
    
    twitterLoader.startThread();
    twitterImages.clear();
    twitter.getQueryMethod(((ofxUILabel * )gui->getWidget("Twitter Term"))->getLabel());
    
    twitterImages.resize(twitter.getQueryImgUrl().size());
    if(!twitter.getQueryImgUrl().empty())
    {
        for (int i = 0; i < twitter.getQueryImgUrl().size();i++) {
            twitterLoader.loadFromURL(twitterImages[i], twitter.getQueryImgUrl()[i]);
            twitterImages[i].setImageCaption(twitter.getQueryImgID()[i]);
            if (i < twitter.getQueryImgUrl().size()/2) {
                twitterImages[i].setup(x, y+(i*76),75,75);
            }
            else{
                twitterImages[i].setup(x+75, y+((i-twitter.getQueryImgUrl().size()/2)*76), 75, 75);
            }
        }
    }
    else {
        cout << "No tags" << endl;
    }
}
//-------------------------------------------------------------
void ofApp::getTwitterUserStream()
{
    int x = 860;
    int y = 720;
    
    twitterLoader.startThread();
    twitterImages.clear();
    twitterImages.resize(50);
    twitterReturn = twitter.getUserTimelineStatuses("@User",50);
    if(!twitter.getImgUrl().empty())
    {
        for (int i = 0; i < twitter.getImgUrl().size();i++) {
            twitterLoader.loadFromURL(twitterImages[i], twitter.getImgUrl()[i]);
            twitterImages[i].setImageCaption(twitter.getImgID()[i]);
            if (i < twitter.getImgUrl().size()/2) {
                twitterImages[i].setup(x, y+(i*76),75,75);
            }
            else{
                twitterImages[i].setup(x+75, y+((i-twitter.getImgUrl().size()/2)*76), 75, 75);
            }
        }
    }
    else {
        cout << "No tags" << endl;
    }
}
#pragma mark - AutoRun
//-------------------------------------------------------------
// *
// *                      AutoRun
// *
//-------------------------------------------------------------
void ofApp::autoRun()
{
    if (!autoRunlatch) {
        autoRunTimer.start(true);
        autoRunlatch = true;
    }
}
//-------------------------------------------------------------
void ofApp::timerStarted(int &args)
{
    cout << "Timer Starting" << endl;
}
//-------------------------------------------------------------
void ofApp::timerComplete(int &args)
{
    if (latchTimer) {
        for(int i = 0; i < outputs.size(); i++) {
            outputs[i].fadeWhiteIn(bare);
        }
        autoRunDelay.start(false);
        latchTimer = false;
    }
    cout << "Timer Complete" << endl;
}
//-------------------------------------------------------------
void ofApp::delayTimerStarted(int &args)
{
    cout << "Delay Timer Started" << endl;
}
//-------------------------------------------------------------
void ofApp::delayTimerComplete(int &args)
{
    bool latch = false;
    if (!latch) {
        for(int i = 0; i < outputs.size(); i++) {
            if (outputs[i].isEditable()) {
                outputs[i].addImage1(pool.getImage(ofRandom(pool.imagesInFolder.size())).getPixelsRef());
            }
        }
        latch = true;
    }
    for(int i = 0; i < outputs.size(); i++) {
        outputs[i].fadeWhiteOut(bare);
    }
    
    autoRunTimer.start(false);
    latchTimer = true;
    cout << "Delay Timer Complete" << endl;
}
#pragma mark - Animations
//-------------------------------------------------------------
// *
// *                      Animatation
// *
//-------------------------------------------------------------
void ofApp::animateAll1to2()
{
    if (!bMoveOrFade) {
        for (int i = 0; i < outputs.size(); i++) {
            outputs[i].animate1to2(bare);
        }
    }
    else if(bMoveOrFade) {
        for (int i = 0; i < outputs.size(); i++) {
            outputs[i].fade1to2(bare);
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateAll2to1()
{
    if (!bMoveOrFade) {
        for (int i = 0; i < outputs.size(); i++) {
            outputs[i].animate2to1(bare);
        }
    }
    else if(bMoveOrFade) {
        for (int i = 0; i < outputs.size(); i++) {
            outputs[i].fade2to1(bare);
        }
    }
}
//-------------------------------------------------------------
void ofApp::animate1to2(int frame)
{
    if (!bMoveOrFade) {
        outputs[frame].animate1to2(bare);
    }
    else if(bMoveOrFade) {
        outputs[frame].fade1to2(bare);
    }
}
//-------------------------------------------------------------
void ofApp::animate2to1(int frame)
{
    if (!bMoveOrFade) {
        outputs[frame].animate2to1(bare);
    }
    else if(bMoveOrFade) {
        outputs[frame].fade2to1(bare);
    }
}
//--------------------------------------------------------------
void ofApp::animateSequentiallyTopToBottom1to2()
{
    if (cycleCounter == outputs.size()-1) {
        
    }
    else{
        if (outputs[cycleCounter].hasTweenGotHalfway()) {
            if (!bMoveOrFade) {
                outputs[cycleCounter+1].animate1to2(bare);
            }
            else if(bMoveOrFade) {
                outputs[cycleCounter+1].fade1to2(bare);
            }
            cycleCounter++;
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSequentiallyTopToBottom2to1()
{
    if (cycleCounter == outputs.size()-1) {
        
    }
    else {
        if (outputs[cycleCounter].hasTweenGotHalfway()) {
            if (!bMoveOrFade) {
                outputs[cycleCounter+1].animate2to1(bare);
            }
            else if(bMoveOrFade) {
                outputs[cycleCounter+1].fade2to1(bare);
            }
            cycleCounter++;
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSequentiallyBottomToTop1to2()
{
    if (cycleCounter == 0) {
        
    }
    else {
        if (outputs[cycleCounter].hasTweenGotHalfway()) {
            if (!bMoveOrFade) {
                outputs[cycleCounter-1].animate1to2(bare);
            }
            else if(bMoveOrFade) {
                outputs[cycleCounter-1].fade1to2(bare);
            }
            cycleCounter--;
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSequentiallyBottomToTop2to1()
{
    if (cycleCounter == 0) {
        
    }
    else {
        if (outputs[cycleCounter].hasTweenGotHalfway()) {
            if (!bMoveOrFade) {
                outputs[cycleCounter-1].animate2to1(bare);
            }
            else if(bMoveOrFade) {
                outputs[cycleCounter-1].fade2to1(bare);
            }
            cycleCounter--;
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSequentiallyLeftToRight1to2()
{
    if (cycleCounter == 6) {//3) {
        
    }
    else {
        if (outputs[cycleCounter].hasTweenGotHalfway() && outputs[cycleCounter+6].hasTweenGotHalfway()) {
            if (!bMoveOrFade) {
                outputs[cycleCounter+1].animate1to2(bare);
                outputs[cycleCounter+7+1].animate1to2(bare);
            }
            else if (bMoveOrFade) {
                outputs[cycleCounter+1].fade1to2(bare);
                outputs[cycleCounter+7+1].fade1to2(bare);
            }
            cycleCounter++;
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSequentiallyRightToLeft1to2()
{
    if (cycleCounter == 0) {
        
    }
    else {
        if (outputs[cycleCounter].hasTweenGotHalfway() && outputs[cycleCounter+6].hasTweenGotHalfway()) {
            if (!bMoveOrFade) {
                outputs[cycleCounter-1].animate1to2(bare);
                outputs[cycleCounter+7-1].animate1to2(bare);
            }
            else if (bMoveOrFade) {
                outputs[cycleCounter-1].fade1to2(bare);
                outputs[cycleCounter+7-1].fade1to2(bare);
            }
            cycleCounter--;
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSequentiallyLeftToRight2to1()
{
    if (cycleCounter == 6) {
        
    }
    else {
        
        if (outputs[cycleCounter].hasTweenGotHalfway() && outputs[cycleCounter+6].hasTweenGotHalfway()) {
            if (!bMoveOrFade) {
                outputs[cycleCounter+1].animate2to1(bare);
                outputs[cycleCounter+7+1].animate2to1(bare);
            }
            else if(bMoveOrFade) {
                outputs[cycleCounter+1].fade2to1(bare);
                outputs[cycleCounter+7+1].fade2to1(bare);
            }
            cycleCounter++;
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSequentiallyRightToLeft2to1()
{
    if (cycleCounter == 0) {
        
    }
    else {
        if (outputs[cycleCounter].hasTweenGotHalfway() && outputs[cycleCounter+6].hasTweenGotHalfway()) {
            if (!bMoveOrFade) {
                outputs[cycleCounter-1].animate2to1(bare);
                outputs[cycleCounter+7-1].animate2to1(bare);
            }
            else if (bMoveOrFade) {
                outputs[cycleCounter-1].fade2to1(bare);
                outputs[cycleCounter+7-1].fade2to1(bare);
            }
            cycleCounter--;
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSelectedSequentially1to2()
{
    if (!selectedOutputs.empty()) {
        if (cycleCounter == selectedOutputs.size()-1) {
            
        }
        else{
            if (outputs[selectedOutputs[cycleCounter]].hasTweenGotHalfway()) {
                if (!bMoveOrFade) {
                    outputs[selectedOutputs[cycleCounter+1]].animate1to2(bare);
                }
                else if(bMoveOrFade) {
                    outputs[selectedOutputs[cycleCounter+1]].fade1to2(bare);
                }
                cycleCounter++;
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSelectedSequentially2to1()
{
    if (!selectedOutputs.empty()) {
        if (cycleCounter == selectedOutputs.size()-1) {
            
        }
        else{
            if (outputs[selectedOutputs[cycleCounter]].hasTweenGotHalfway()) {
                if (!bMoveOrFade) {
                    outputs[selectedOutputs[cycleCounter+1]].animate2to1(bare);
                }
                else if(bMoveOrFade) {
                    outputs[selectedOutputs[cycleCounter+1]].fade2to1(bare);
                }
                cycleCounter++;
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSelectedSequentiallyToImage()
{
    if (!selectedOutputs.empty()) {
        if (cycleCounter == selectedOutputs.size()-1) {
            
        }
        else{
            if (outputs[selectedOutputs[cycleCounter]].hasFadeToWhiteFinished()) {
                outputs[selectedOutputs[cycleCounter+1]].fadeWhiteOut(bare);
                cycleCounter++;
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::animateSelectedSequentiallyToWhite()
{
    if (!selectedOutputs.empty()) {
        if (cycleCounter == selectedOutputs.size()-1) {
            
        }
        else{
            if (outputs[selectedOutputs[cycleCounter]].hasFadeToWhiteFinished()) {
                outputs[selectedOutputs[cycleCounter+1]].fadeWhiteIn(bare);
                cycleCounter++;
            }
        }
    }
}
#pragma mark - Draw Images
//--------------------------------------------------------------
// *
// *                      Draw Image Pools
// *
//--------------------------------------------------------------
void ofApp::drawImagePool()
{
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofRect(5, 685, 845, ofGetHeight()/2);
    ofPopStyle();
    infoFont.drawString("Image Pool", 10,710);
    pool.draw(0, 0);
}
//--------------------------------------------------------------
void ofApp::drawInstagramPool()
{
    int x = 1070;
    int y = 685;
    for (int i = 0; i < instagramImages.size(); i++) {
        instagramImages[i].draw(instagramImages[i].getOrigin(),75,75);
    }
    ofPushMatrix();
    ofTranslate(x,y);
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofRect(0, 0, 200, ofGetHeight()/2);
    ofPopStyle();
    infoFont.drawString("Instagram Pool", 5,28);
    // Checker for the image loaders
    if (instagramLoader.isThreadRunning()) {
        ofDrawBitmapStringHighlight("Instagram Loading Images", 5,45);
    }
    else {
        ofDrawBitmapStringHighlight("Not Loading Images", 5,45);
    }
    ofPopMatrix();
}
//--------------------------------------------------------------
void ofApp::drawTwitterPool()
{
    int x = 860;
    int y = 685;
    
    for (int i = 0; i < twitterImages.size(); i++) {
        twitterImages[i].draw(twitterImages[i].getOrigin(),75,75);
    }
    
    ofPushMatrix();
    ofTranslate(x,y);
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofRect(0, 0, 200, ofGetHeight()/2);
    ofPopStyle();
    infoFont.drawString("Twitter Pool", 5,28);
    if (twitterLoader.isThreadRunning()) {
        ofDrawBitmapStringHighlight("Twitter Loading Images", 5,45);
    }
    else {
        ofDrawBitmapStringHighlight("Not Loading Images", 5,45);
    }
    ofPopMatrix();
}
//--------------------------------------------------------------
void ofApp::drawPreview1()
{
    int x = 1320;
    int y = 685;
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofRect(x-2, y-2, 7*51, 105);
    ofPopStyle();
    
    for(int i = 0; i < outputs.size(); i++)
    {
        if (i <7) {
            outputs[i].drawImage1(x+(i*51), y);
        }
        else{
            outputs[i].drawImage1(x+((i-7)*51), y+51);
        }
    }
    infoFont.drawString("Preview Window 1", x,y+135);
}
//--------------------------------------------------------------
void ofApp::drawPreview2()
{
    int x = 1320;
    int y = 720+150;
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofRect(x-2, y-2, 7*51, 105);
    ofPopStyle();
    
    for(int i = 0; i < outputs.size(); i++)
    {
        if (i <7) {
            outputs[i].drawImage2(x+(i*51), y);
        }
        else{
            outputs[i].drawImage2(x+((i-7)*51), y+51);
        }
    }
    infoFont.drawString("Preview Window 2", x,y+135);
}
//--------------------------------------------------------------
void ofApp::drawLogoArea()
{
    int x = 10;
    int y = 10+(4*330)+10;
    
    ofPushMatrix();
    ofTranslate(x, y);
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofRect(0, 0, 4*330, 340);
    ofRect(10, 10, 320, 320);
    ofRect(340, 10, 320, 320);
    ofRect(670, 10, 320, 320);
    ofPopStyle();
    ofPopMatrix();
}
//--------------------------------------------------------------
void ofApp::drawInformation()
{
    int x = (7*330);
    int y = 10;
    
    ofPushMatrix();
    ofTranslate(x, y);
    ofPushStyle();
    ofNoFill();
    ofSetColor(255, 255, 255);
    ofRect(0, 0, 500, 340);
    stringstream info;
    info << "Cusine Color" << endl;
    info << endl;
    info << "Number of Images In Pool: " << pool.imagesInFolder.size() << endl;
    info << endl;
    int instagramCount = 0;
    for (int i = 0 ; i < instagramImages.size(); i++) {
        if (instagramImages[i].isAllocated()) {
            instagramCount++;
        }
    }
    info << "Found " << instagram.getImageURL().size()  << " of Instagram Images" << endl;
    info << "Number of Instagram Images: " << instagramCount << endl;
    info << endl;
    
    int twitterCount = 0;
    for (int i = 0 ; i < twitterImages.size(); i++) {
        if (twitterImages[i].isAllocated()) {
            twitterCount++;
        }
    }
    info << "Number of Twitter Images: " << twitterCount << endl;
    
    for (int i = 0; i < selectedOutputs.size(); i++) {
        info << selectedOutputs[i] << " , ";
    }
    info << endl;
    
    ofSetColor(255);
    infoFont.drawString(info.str(), 5,5+20);
    ofPopStyle();
    ofPopMatrix();
}
//--------------------------------------------------------------
// *
// *                      User Interface
// *
//--------------------------------------------------------------
#pragma mark - GUI Main
//--------------------------------------------------------------
void ofApp::setupGUI()
{
    vector<string> animationTypes;
    animationTypes.push_back("ANIMATION_TYPE_BACK");
    animationTypes.push_back("ANIMATION_TYPE_BOUNCE");
    animationTypes.push_back("ANIMATION_TYPE_CIRC");
    animationTypes.push_back("ANIMATION_TYPE_CUBIC");
    animationTypes.push_back("ANIMATION_TYPE_ELASTIC");
    animationTypes.push_back("ANIMATION_TYPE_EXPO");
    animationTypes.push_back("ANIMATION_TYPE_LINEAR");
    animationTypes.push_back("ANIMATION_TYPE_QUAD");
    animationTypes.push_back("ANIMATION_TYPE_QUART");
    animationTypes.push_back("ANIMATION_TYPE_QUINT");
    animationTypes.push_back("ANIMATION_TYPE_SINE");
    
    vector<string> animationMethod;
    animationMethod.push_back("Animate All 1 to 2");
    animationMethod.push_back("Animate All 2 to 1");
    animationMethod.push_back("1to2 Top to Bottom");
    animationMethod.push_back("2to1 Top to Bottom");
    animationMethod.push_back("1to2 Bottom to Top");
    animationMethod.push_back("2to1 Bottom to Top");
    animationMethod.push_back("1to2 Left to Right");
    animationMethod.push_back("2to1 Left to Right");
    animationMethod.push_back("1to2 Right to Left");
    animationMethod.push_back("2to1 Right to Left");
    
    
    vector <string> call;
    call.push_back("Instagram User Feed");
    call.push_back("Instagram User Likes");
    call.push_back("Instagram Feed");
    call.push_back("Instagram Query");
    call.push_back("Twitter Favourites");
    call.push_back("Twitter Retweets");
    call.push_back("Twitter User Stream");
    call.push_back("Twitter Query");
    
    int elementWidth = 350;
    int elementHeight = 30;
    
    gui = new ofxUICanvas((330*6), 660, 600, ofGetHeight());
    gui->setTheme(OFX_UI_THEME_HAYLOCK_BLACK);
    gui->setFontSize(OFX_UI_FONT_SMALL, 5,OFX_UI_FONT_RESOLUTION);
    gui->setRetinaResolution();
    gui->addWidgetDown(new ofxUILabel(elementWidth,"Cusine Color",OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIFPS(OFX_UI_FONT_MEDIUM));
    gui->addSpacer(elementWidth,2);
    gui->addWidgetDown(new ofxUILabelToggle(elementWidth,false,"Auto Run",OFX_UI_FONT_MEDIUM,elementHeight*3));
    gui->addWidgetDown(new ofxUILabel("Change Timer",OFX_UI_FONT_SMALL));
    gui->addWidgetRight(new ofxUINumberDialer(0, 30000, 1000, 0, "Change_Timer", OFX_UI_FONT_SMALL));
    gui->addSpacer(elementWidth,2);
    gui->addWidgetDown(new ofxUILabel("API Calls",OFX_UI_FONT_MEDIUM));
    gui->addSpacer(elementWidth,2);
    gui->addWidgetDown(new ofxUILabelButton(elementWidth, false,"Stop Loaders",OFX_UI_FONT_SMALL,elementHeight));
    gui->addSpacer(elementWidth,2);
    gui->addWidgetDown(new ofxUILabel("Instagram",OFX_UI_FONT_SMALL));
    gui->addSpacer(elementWidth,2);
    gui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Instagram User Feed",OFX_UI_FONT_SMALL,elementHeight));
    gui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Instagram User Likes",OFX_UI_FONT_SMALL,elementHeight));
    gui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Instagram Feed",OFX_UI_FONT_SMALL,elementHeight));
    gui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Instagram Query",OFX_UI_FONT_SMALL,elementHeight));
    gui->addWidgetDown(new ofxUITextInput("Instagram Query Terms", "", elementWidth));
    gui->addWidgetDown(new ofxUILabel("Instagram Term",OFX_UI_FONT_SMALL));
    gui->addSpacer(elementWidth,2);
    gui->addWidgetDown(new ofxUILabel("Twitter",OFX_UI_FONT_SMALL));
    gui->addSpacer(elementWidth,2);
    gui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Twitter Favourites",OFX_UI_FONT_SMALL,elementHeight));
    gui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Twitter Retweets",OFX_UI_FONT_SMALL,elementHeight));
    gui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Twitter User Stream",OFX_UI_FONT_SMALL,elementHeight));
    gui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Twitter Query",OFX_UI_FONT_SMALL,elementHeight));
    gui->addWidgetDown(new ofxUITextInput("Twitter Query Terms", "", elementWidth));
    gui->addWidgetDown(new ofxUILabel("Twitter Term",OFX_UI_FONT_SMALL));
    gui->addSpacer(elementWidth,2);
    gui->autoSizeToFitWidgets();
    
    animationGui = new ofxUICanvas(ofGetWidth()-410,0,400,600);
    animationGui->setTheme(OFX_UI_THEME_HAYLOCK_BLACK);
    animationGui->setRetinaResolution();
    animationGui->addWidgetDown(new ofxUILabel("Animation",OFX_UI_FONT_MEDIUM));
    animationGui->addWidgetDown(new ofxUILabel("Animation Time",OFX_UI_FONT_SMALL));
    animationGui->addWidgetRight(new ofxUINumberDialer(0, 30000, 1000, 0, "Animation_Time", OFX_UI_FONT_SMALL));
    animationGui->addSpacer(elementWidth,2);
    vector<string> moveOrFade;
    moveOrFade.push_back("Move");
    moveOrFade.push_back("Fade");
    vector<string> whiteOrImage;
    whiteOrImage.push_back("White");
    whiteOrImage.push_back("Image");
    animationGui->addWidgetDown(new ofxUIRadio("AnimationType",moveOrFade,OFX_UI_ORIENTATION_HORIZONTAL,50,elementHeight));
    animationGui->addWidgetDown(new ofxUILabelToggle(elementWidth,false, "White",OFX_UI_FONT_SMALL,elementHeight));
    for (int i = 0; i < animationMethod.size(); i++) {
        animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,animationMethod[i],OFX_UI_FONT_SMALL,elementHeight));
    }
    animationGui->addSpacer(elementWidth,2);
    animationGui->addWidgetDown(new ofxUILabel(elementWidth+10,"Animate Single Frames",OFX_UI_FONT_MEDIUM));
    animationGui->addWidgetDown(new ofxUIToggleMatrix(45,45,2,7,"AnimateSingleFrames"));
    animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Animated Selected 1 to 2",OFX_UI_FONT_SMALL,elementHeight));
    animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Animated Selected 2 to 1",OFX_UI_FONT_SMALL,elementHeight));
    animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Animated Selected (Seq) 1 to 2",OFX_UI_FONT_SMALL,elementHeight));
    animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Animated Selected (Seq) 2 to 1",OFX_UI_FONT_SMALL,elementHeight));
    animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Animate Selected to White",OFX_UI_FONT_SMALL,elementHeight));
    animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Animate Selected to Image",OFX_UI_FONT_SMALL,elementHeight));
    animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Animate All Selected to White",OFX_UI_FONT_SMALL,elementHeight));
    animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Animate All Selected to Image",OFX_UI_FONT_SMALL,elementHeight));
    animationGui->addWidgetDown(new ofxUILabelButton(elementWidth,false,"Clear",OFX_UI_FONT_SMALL,elementHeight));
    animationGui->addWidgetDown(new ofxUIRadio("Animations",animationTypes,OFX_UI_ORIENTATION_VERTICAL,50,elementHeight));
    animationGui->autoSizeToFitWidgets();
    
    vector<string> adding;
    adding.push_back("Can't Add");
    adding.push_back("Can Add to 1");
    adding.push_back("Can Add to 2");
    
    selectionGui = new ofxUICanvas(1320,720+400,350,600);
    selectionGui->setTheme(OFX_UI_THEME_HAYLOCK_BLACK);
    selectionGui->setRetinaResolution();
    selectionGui->addWidgetDown(new ofxUILabel("Selection",OFX_UI_FONT_MEDIUM));
    selectionGui->addWidgetDown(new ofxUILabel(elementWidth,"Add to Where",OFX_UI_FONT_MEDIUM));
    selectionGui->addWidgetDown(new ofxUIRadio("AddorNot",adding,OFX_UI_ORIENTATION_VERTICAL,50,elementHeight));
    selectionGui->addWidgetDown(new ofxUILabel("Select Frames",OFX_UI_FONT_MEDIUM));
    selectionGui->addWidgetDown(new ofxUISpacer(elementWidth,2,"SP2"));
    selectionGui->addWidgetDown(new ofxUIToggleMatrix(75,75,2,7,"Frame1"));
    selectionGui->autoSizeToFitWidgets();
    
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
    ofAddListener(selectionGui->newGUIEvent,this,&ofApp::guiEvent);
    ofAddListener(animationGui->newGUIEvent,this,&ofApp::guiEvent);
}
//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    for (int i = 0; i < 7; i++) {
        if (e.getName() == "AnimateSingleFrames("+ofToString(i)+",0)")
        {
            selectedOutputs.push_back(i);
            ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames("+ofToString(i)+",0)"))->setVisible(false);
        }
    }
    
    for (int i = 0; i < 7; i++) {
        if (e.getName() == "AnimateSingleFrames("+ofToString(i)+",1)")
        {
            selectedOutputs.push_back(ofMap(i, 0, 6, 7, 13));
            ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames("+ofToString(i)+",1)"))->setVisible(false);
        }
    }
    if(e.getName() == "Animation_Time")
    {
        ofxUINumberDialer * dialer = (ofxUINumberDialer *) e.widget;
        animationTime = (int)(dialer->getValue());
    }
    else if(e.getName() == "Save Frame 1")
    {
        ofxJSONElement data;
        for (int i = 0;  i < outputs.size(); i++) {
            data[i]["frame"+ofToString(i)];
            data[i]["frame"+ofToString(i)]["image1id"] = outputs[i].getImage1ID();
            data[i]["frame"+ofToString(i)]["image2id"] = outputs[i].getImage2ID();
        }
        ofFile framesInfo(ofToDataPath("json/"+ofGetTimestampString()+".json"),ofFile::WriteOnly);
        framesInfo << data;
    }
    else if(e.getName() == "Auto Run")
    {
        ofxUILabelToggle * tog = (ofxUILabelToggle *) e.widget;
        _AutoRun = tog->getValue();
        autoRunlatch = false;
    }
    else if(e.getName() == "Change_Timer")
    {
        ofxUINumberDialer * dialer = (ofxUINumberDialer *) e.widget;
        _timer = (int)(dialer->getValue());
        autoRunTimer.setup(_timer);
    }
    else if(e.getName() == "Move")
    {
        bMoveOrFade = false;
    }
    else if(e.getName() == "Fade")
    {
        bMoveOrFade = true;
    }
    else if(e.getName() == "White")
    {
        ofxUILabelToggle * tog = (ofxUILabelToggle *) e.widget;
        for (int i = 0;  i < outputs.size(); i++) {
            outputs[i].setFadeToWhite(tog->getValue());
        }
    }
    else if(e.getName() == "Can't Add")
    {
        whichToAddTo = 0;
    }
    else if(e.getName() == "Can Add to 1")
    {
        whichToAddTo = 1;
    }
    else if(e.getName() == "Can Add to 2")
    {
        whichToAddTo = 2;
    }
    else if(e.getName() == "Instagram Query Terms")
    {
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER: ";
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
        {
            cout << "ON FOCUS: ";
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
        }
        string output = textinput->getTextString();
        ((ofxUILabel * )gui->getWidget("Instagram Term"))->setLabel(output);
        
    }
    else if(e.getName() == "Twitter Query Terms")
    {
        ofxUITextInput *textinput = (ofxUITextInput *) e.widget;
        if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_ENTER)
        {
            cout << "ON ENTER: ";
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
        {
            cout << "ON FOCUS: ";
        }
        else if(textinput->getTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
        {
            cout << "ON BLUR: ";
        }
        string output = textinput->getTextString();
        ((ofxUILabel * )gui->getWidget("Twitter Term"))->setLabel(output);
    }
    else if(e.getName() == "Instagram User Feed")
    {
        ofxUILabelButton * but = (ofxUILabelButton *) e.widget;
        if (but->getValue()) {
            getInstagramUserFeed();
            ((ofxUIButton *)gui->getWidget("Instagram User Feed"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Instagram User Feed"))->setVisible(false);
        }
    }
    else if (e.getName() == "Instagram User Likes")
    {
        ofxUILabelButton * but = (ofxUILabelButton *) e.widget;
        if (but->getValue()) {
            getInstagramLikes();
            ((ofxUIButton *)gui->getWidget("Instagram User Likes"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Instagram User Likes"))->setVisible(false);
        }
    }
    else if (e.getName() == "Instagram Feed")
    {
        ofxUILabelButton * but = (ofxUILabelButton *) e.widget;
        if (but->getValue()) {
            getInstagramFeed();
            ((ofxUIButton *)gui->getWidget("Instagram Feed"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Instagram Feed"))->setVisible(false);
        }
    }
    else if (e.getName() == "Instagram Query")
    {
        ofxUILabelButton * but = (ofxUILabelButton *) e.widget;
        if (but->getValue()) {
            getInstagramQuery();
            ((ofxUIButton *)gui->getWidget("Instagram Query"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Instagram Query"))->setVisible(false);
        }
    }
    else if (e.getName() == "Twitter Favourites")
    {
        ofxUILabelButton * but = (ofxUILabelButton *) e.widget;
        if (but->getValue()) {
            getTwitterFavourites();
            ((ofxUIButton *)gui->getWidget("Twitter Favourites"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Twitter Favourites"))->setVisible(false);
        }
    }
    else if (e.getName() == "Twitter Retweets")
    {
        ofxUILabelButton * but = (ofxUILabelButton *) e.widget;
        if (but->getValue()) {
            getTwitterRetweets();
            ((ofxUIButton *)gui->getWidget("Twitter Retweets"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Twitter Retweets"))->setVisible(false);
        }
    }
    else if (e.getName() == "Twitter User Stream")
    {
        ofxUILabelButton * but = (ofxUILabelButton *) e.widget;
        if (but->getValue()) {
            getTwitterUserStream();
            ((ofxUIButton *)gui->getWidget("Twitter User Stream"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Twitter User Stream"))->setVisible(false);
        }
    }
    else if (e.getName() == "Twitter Query")
    {
        ofxUILabelButton * but = (ofxUILabelButton *) e.widget;
        if (but->getValue()) {
            getTwitterQuery();
            ((ofxUIButton *)gui->getWidget("Twitter Query"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Twitter Query"))->setVisible(false);
        }
    }
    else if (e.getName() == "Stop Loaders")
    {
        twitterLoader.stopThread();
        instagramLoader.stopThread();
        tumblrLoader.stopThread();
        if (!((ofxUIButton *)gui->getWidget("Instagram User Feed"))->isVisible()) {
            ((ofxUIButton *)gui->getWidget("Instagram User Feed"))->setLabelVisible(true);
            ((ofxUIButton *)gui->getWidget("Instagram User Feed"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Instagram User Feed"))->setVisible(true);
        }
        else if (!((ofxUIButton *)gui->getWidget("Instagram User Likes"))->isVisible()) {
            ((ofxUIButton *)gui->getWidget("Instagram User Likes"))->setLabelVisible(true);
            ((ofxUIButton *)gui->getWidget("Instagram User Likes"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Instagram User Likes"))->setVisible(true);
        }
        else if (!((ofxUIButton *)gui->getWidget("Instagram Feed"))->isVisible()) {
            ((ofxUIButton *)gui->getWidget("Instagram Feed"))->setLabelVisible(true);
            ((ofxUIButton *)gui->getWidget("Instagram Feed"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Instagram Feed"))->setVisible(true);
        }
        else if (!((ofxUIButton *)gui->getWidget("Instagram Query"))->isVisible()) {
            ((ofxUIButton *)gui->getWidget("Instagram Query"))->setLabelVisible(true);
            ((ofxUIButton *)gui->getWidget("Instagram Query"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Instagram Query"))->setVisible(true);
        }
        else if (!((ofxUIButton *)gui->getWidget("Twitter Favourites"))->isVisible()) {
            ((ofxUIButton *)gui->getWidget("Twitter Favourites"))->setLabelVisible(true);
            ((ofxUIButton *)gui->getWidget("Twitter Favourites"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Twitter Favourites"))->setVisible(true);
        }
        else if (!((ofxUIButton *)gui->getWidget("Twitter Retweets"))->isVisible()) {
            ((ofxUIButton *)gui->getWidget("Twitter Retweets"))->setLabelVisible(true);
            ((ofxUIButton *)gui->getWidget("Twitter Retweets"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Twitter Retweets"))->setVisible(true);
        }
        else if (!((ofxUIButton *)gui->getWidget("Twitter User Stream"))->isVisible()) {
            ((ofxUIButton *)gui->getWidget("Twitter User Stream"))->setLabelVisible(true);
            ((ofxUIButton *)gui->getWidget("Twitter User Stream"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Twitter User Stream"))->setVisible(true);
        }
        else if (!((ofxUIButton *)gui->getWidget("Twitter Query"))->isVisible()) {
            ((ofxUIButton *)gui->getWidget("Twitter Query"))->setLabelVisible(true);
            ((ofxUIButton *)gui->getWidget("Twitter Query"))->setState(false);
            ((ofxUIButton *)gui->getWidget("Twitter Query"))->setVisible(true);
        }
    }
    else if (e.getName() == "1to2 Top to Bottom")
    {
        canSeq = true;
        cycleCounter = 0;
        if (!bMoveOrFade) {
            outputs[cycleCounter].animate1to2(bare);
        }
        else if(bMoveOrFade) {
            outputs[cycleCounter].fade1to2(bare);
        }
        _animationMethod = 0;
    }
    else if (e.getName() == "2to1 Top to Bottom")
    {
        canSeq = true;
        cycleCounter = 0;
        if (!bMoveOrFade) {
            outputs[cycleCounter].animate2to1(bare);
        }
        else if (bMoveOrFade) {
            outputs[cycleCounter].fade2to1(bare);
        }
        _animationMethod = 1;
    }
    else if (e.getName() == "1to2 Bottom to Top")
    {
        canSeq = true;
        cycleCounter = outputs.size()-1;
        if (!bMoveOrFade) {
            outputs[cycleCounter].animate1to2(bare);
        }
        else if (bMoveOrFade) {
            outputs[cycleCounter].fade1to2(bare);
        }
        _animationMethod = 2;
    }
    else if (e.getName() == "2to1 Bottom to Top")
    {
        canSeq = true;
        cycleCounter = outputs.size()-1;
        if (!bMoveOrFade) {
            outputs[cycleCounter].animate2to1(bare);
        }
        else if (bMoveOrFade) {
            outputs[cycleCounter].fade2to1(bare);
        }
        
        _animationMethod = 3;
    }
    else if (e.getName() == "1to2 Left to Right")
    {
        canSeq = true;
        cycleCounter = 0;
        
        if (!bMoveOrFade) {
            outputs[cycleCounter].animate1to2(bare);
            outputs[cycleCounter+7].animate1to2(bare);
        }
        else if (bMoveOrFade) {
            outputs[cycleCounter].fade1to2(bare);
            outputs[cycleCounter+7].fade1to2(bare);
        }
        _animationMethod = 4;
    }
    else if (e.getName() == "2to1 Left to Right")
    {
        canSeq = true;
        cycleCounter = 0;
        if (!bMoveOrFade) {
            outputs[cycleCounter].animate2to1(bare);
            outputs[cycleCounter+7].animate2to1(bare);
        }
        else if (bMoveOrFade) {
            outputs[cycleCounter].fade2to1(bare);
            outputs[cycleCounter+7].fade2to1(bare);
        }
        
        _animationMethod = 5;
    }
    else if (e.getName() == "1to2 Right to Left")
    {
        canSeq = true;
        cycleCounter = 6;
        if (!bMoveOrFade) {
            outputs[cycleCounter].animate1to2(bare);
            outputs[cycleCounter+7].animate1to2(bare);
        }
        else if(bMoveOrFade) {
            outputs[cycleCounter].fade1to2(bare);
            outputs[cycleCounter+7].fade1to2(bare);
        }
        _animationMethod = 6;
    }
    else if (e.getName() == "2to1 Right to Left")
    {
        canSeq = true;
        cycleCounter = 6;
        
        if (!bMoveOrFade) {
            outputs[cycleCounter].animate2to1(bare);
            outputs[cycleCounter+7].animate2to1(bare);
        }
        else if(bMoveOrFade) {
            outputs[cycleCounter].fade2to1(bare);
            outputs[cycleCounter+7].fade2to1(bare);
        }
        _animationMethod = 7;
    }
    else if (e.getName() == "Animated Selected (Seq) 1 to 2")
    {
        if (!selectedOutputs.empty()) {
            canSeq = true;
            cycleCounter = 0;
            if (!bMoveOrFade) {
                outputs[selectedOutputs[cycleCounter]].animate1to2(bare);
            }
            else if (bMoveOrFade) {
                outputs[selectedOutputs[cycleCounter]].fade1to2(bare);
            }
            _animationMethod = 8;
        }
    }
    else if (e.getName() == "Animated Selected (Seq) 2 to 1")
    {
        if (!selectedOutputs.empty()) {
            canSeq = true;
            cycleCounter = 0;
            if (!bMoveOrFade) {
                outputs[selectedOutputs[cycleCounter]].animate2to1(bare);
            }
            else if (bMoveOrFade) {
                outputs[selectedOutputs[cycleCounter]].fade2to1(bare);
            }
            _animationMethod = 9;
        }
    }
    else if (e.getName() == "Animate All 1 to 2")
    {
        animateAll1to2();
    }
    else if (e.getName() == "Animate All 2 to 1")
    {
        animateAll2to1();
    }
    else if (e.getName() == "Animated Selected 1 to 2")
    {
        for (int i = 0; i < selectedOutputs.size(); i++) {
            animate1to2(selectedOutputs[i]);
        }
    }
    else if (e.getName() == "Animated Selected 2 to 1")
    {
        for (int i = 0; i < selectedOutputs.size(); i++) {
            animate2to1(selectedOutputs[i]);
        }
    }
    else if(e.getName() == "Animate Selected to White")
    {
        if (!selectedOutputs.empty()) {
            canSeq = true;
            cycleCounter = 0;
            outputs[selectedOutputs[cycleCounter]].fadeWhiteIn(bare);
            _animationMethod = 10;
        }
    }
    else if(e.getName() == "Animate Selected to Image")
    {
        if (!selectedOutputs.empty()) {
            canSeq = true;
            cycleCounter = 0;
            outputs[selectedOutputs[cycleCounter]].fadeWhiteOut(bare);
            _animationMethod = 11;
        }
    }
    else if(e.getName() == "Animate All Selected to Image")
    {
        for (int i = 0; i < selectedOutputs.size(); i++) {
            outputs[selectedOutputs[i]].fadeWhiteOut(bare);
        }
    }
    else if(e.getName() == "Animate All Selected to White")
    {
        for (int i = 0; i < selectedOutputs.size(); i++) {
            outputs[selectedOutputs[i]].fadeWhiteIn(bare);
        }
    }
    else if (e.getName() == "ANIMATION_TYPE_BACK") {
        bare = ANIMATION_TYPE_BACK;
    }
    else if (e.getName() == "ANIMATION_TYPE_BOUNCE") {
        bare = ANIMATION_TYPE_BOUNCE;
    }
    else if (e.getName() == "ANIMATION_TYPE_CIRC") {
        bare = ANIMATION_TYPE_CIRC;
    }
    else if (e.getName() == "ANIMATION_TYPE_CUBIC") {
        bare = ANIMATION_TYPE_CUBIC;
    }
    else if (e.getName() == "ANIMATION_TYPE_ELASTIC") {
        bare = ANIMATION_TYPE_ELASTIC;
    }
    else if (e.getName() == "ANIMATION_TYPE_EXPO") {
        bare = ANIMATION_TYPE_EXPO;
    }
    else if (e.getName() == "ANIMATION_TYPE_LINEAR") {
        bare = ANIMATION_TYPE_LINEAR;
    }
    else if (e.getName() == "ANIMATION_TYPE_QUAD") {
        bare = ANIMATION_TYPE_QUAD;
    }
    else if (e.getName() == "ANIMATION_TYPE_QUART") {
        bare = ANIMATION_TYPE_QUART;
    }
    else if (e.getName() == "ANIMATION_TYPE_QUINT") {
        bare = ANIMATION_TYPE_QUINT;
    }
    else if (e.getName() == "ANIMATION_TYPE_SINE") {
        bare = ANIMATION_TYPE_SINE;
    }
    else if (e.getName() == "Clear")
    {
        for (int i = 0 ; i < outputs.size(); i++) {
            outputs[i].stopAnimating();
        }
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(0,0)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(1,0)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(2,0)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(3,0)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(4,0)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(5,0)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(6,0)"))->setValue(false);
        
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(0,1)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(1,1)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(2,1)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(3,1)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(4,1)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(5,1)"))->setValue(false);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(6,1)"))->setValue(false);
        
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(0,0)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(1,0)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(2,0)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(3,0)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(4,0)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(5,0)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(6,0)"))->setVisible(true);
        
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(0,1)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(1,1)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(2,1)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(3,1)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(4,1)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(5,1)"))->setVisible(true);
        ((ofxUIToggle *)animationGui->getWidget("AnimateSingleFrames(6,1)"))->setVisible(true);
        selectedOutputs.clear();
    }
#pragma mark - Frameselect
    //------------------------------------------------------------
    // *
    // Selection of Images
    // *
    //------------------------------------------------------------
    for (int i = 0; i < 7; i++) {
        if (e.getName() == "Frame1("+ofToString(i)+",0)")
        {
            ofxUIToggle * toggle = (ofxUIToggle *) e.widget;
            outputs[i].isSelected(toggle->getValue());
        }
    }
    for (int i = 0; i < 7; i++) {
        if (e.getName() == "Frame1("+ofToString(i)+",1)")
        {
            ofxUIToggle * toggle = (ofxUIToggle *) e.widget;
            outputs[ofMap(i, 0, 6, 7, 13)].isSelected(toggle->getValue());
        }
    }
}