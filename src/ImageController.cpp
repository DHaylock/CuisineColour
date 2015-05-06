//
//  ImageController.cpp
//  CusineColor
//
//  Created by David Haylock on 10/02/2015.
//
//

#include "ImageController.h"
#pragma mark - Setup
//--------------------------------------------------------------
void ImageController::setup(int x, int y, int w, int h)
{
    selected = false;
    _moveOrFade = false;
    _x = x;
    _y = y;
    _w = w;
    _h = h;
    scene.allocate(320, 320,GL_RGBA);
}
#pragma mark - Update
//--------------------------------------------------------------
void ImageController::update()
{
    scene.begin();
    ofClear(0, 0, 0);
    if (!_moveOrFade) {
        ofSetColor(255, 255, 255);
        
        if (slot1 == CUSINE_IMAGE) {
            if (image1.isAllocated()) {
                image1.draw(tweenImage1.update(),0, 320, 320);
            }
        }
        if (slot2 == CUSINE_IMAGE) {
            if (image2.isAllocated()) {
                image2.draw(tweenImage2.update(),0, 320, 320);
            }
        }
    }
    else if(_moveOrFade)
    {
        if (slot1 == CUSINE_IMAGE) {
            if (image1.isAllocated()) {
                ofSetColor(255,tweenImage1.update());
                image1.draw(0,0, 320, 320);
            }
        }
    
        if (slot2 == CUSINE_IMAGE) {
            if (image2.isAllocated()) {
                ofSetColor(255,tweenImage2.update());
                image2.draw(0,0, 320, 320);
            }
        }
    }
    if (_fadeToWhite) {
        ofSetColor(255,fadeToWhiteTween.update());
        ofRect(0, 0, 320, 320);
    }
    
    
    scene.end();
}

#pragma mark - Drawers
//--------------------------------------------------------------
void ImageController::draw()
{
    ofSetColor(255, 255, 255);
    scene.draw(_x, _y);
}
//--------------------------------------------------------------
void ImageController::drawImage1(int x,int y)
{
    ofSetColor(255, 255, 255);
    if (slot1 == CUSINE_IMAGE) {
        if (image1.isAllocated()) {
            image1.draw(x,y, 50, 50);
        }
    }
}
//--------------------------------------------------------------
void ImageController::drawImage2(int x,int y)
{
    ofSetColor(255, 255, 255);
    if (slot2 == CUSINE_IMAGE) {
        if (image2.isAllocated()) {
            image2.draw(x,y, 50, 50);
        }
    }
}
//--------------------------------------------------------------
void ImageController::drawOutline()
{
    ofPushStyle();
    if(selected)
    {
        ofSetColor(0,200,100);
    }
    else
    {
        ofSetColor(255, 255, 255);
    }
    ofNoFill();
    ofSetLineWidth(3);
    ofRect(_x, _y, _w, _h);
    ofDrawBitmapStringHighlight(ofToString(getCurrentFrame()), _x+5,_y+10);
    ofPopStyle();
}
#pragma mark - Setters
//--------------------------------------------------------------
void ImageController::setSlot1(ImageType type)
{
    if (type == CUSINE_IMAGE) {
        slot1 = CUSINE_IMAGE;
    }
}
//--------------------------------------------------------------
void ImageController::setSlot2(ImageType type)
{
    if (type == CUSINE_IMAGE) {
        slot2 = CUSINE_IMAGE;
    }
}
//--------------------------------------------------------------
void ImageController::setFadeToWhite(bool value)
{
    _fadeToWhite = value;
}
//--------------------------------------------------------------
void ImageController::setControllerID(int id)
{
    _id = id;
}
//--------------------------------------------------------------
void ImageController::setMoveOrFade(bool val)
{
    _moveOrFade = val;
}
//--------------------------------------------------------------
void ImageController::setWhiteOrImage(bool val)
{
    _whiteOrImage = val;
}
//--------------------------------------------------------------
void ImageController::setImageCaption(string caption)
{
    _imageCaption = caption;
}
//--------------------------------------------------------------
void ImageController::setImage1ID(string id)
{
    _image1ID = id;
}
//--------------------------------------------------------------
void ImageController::setImage2ID(string id)
{
    _image2ID = id;
}
//--------------------------------------------------------------
void ImageController::setAnimationLength(int time)
{
    _time = time;
}
#pragma mark - Getters
//--------------------------------------------------------------
int ImageController::getControllerID()
{
    return _id;
}
//--------------------------------------------------------------
string ImageController::getImage1ID()
{
    return _image1ID;
}
//--------------------------------------------------------------
string ImageController::getImage2ID()
{
    return _image2ID;
}
//--------------------------------------------------------------
string ImageController::getImageCaption()
{
    return _imageCaption;
}
//--------------------------------------------------------------
ofPoint ImageController::getOrigin()
{
    return ofPoint(_x,_y);
}
//--------------------------------------------------------------
int ImageController::getCurrentFrame()
{
    
    if (!_moveOrFade) {
        if ((tweenImage1.update() >= 0)&&(tweenImage2.update() >= 320)) {
            _currentFrame = 1;
        }
        else if ((tweenImage1.update() <= -320) && (tweenImage2.update() <= 0)) {
            _currentFrame = 2;
        }
        else {
            
        }
    }
    else if (_moveOrFade) {
        if ((tweenImage1.update() == 255)&&(tweenImage2.update() == 0)) {
            _currentFrame = 1;
        }
        else if ((tweenImage1.update() == 0) && (tweenImage2.update() == 255)) {
            _currentFrame = 2;
        }
        else {
            
        }
    }
    return _currentFrame;
}
//--------------------------------------------------------------
bool ImageController::isClicked(int x,int y)
{
    if (!selected) {
        if (((x >= _x)&&(x <= _x+_w))&&((y >= _y)&&( y <= _y+_h))) {
            selected = true;
            return true;
        }
    }
    else if (selected) {
        if (((x >= _x)&&(x <= _x+_w))&&((y >= _y)&&( y <= _y+_h))) {
            selected = false;
            return false;
        }
    }
}
//--------------------------------------------------------------
void ImageController::isSelected(bool _selected)
{
    selected = _selected;
}
//--------------------------------------------------------------
bool ImageController::isEditable()
{
    return selected;
}
#pragma mark - Add and Delete Image
//--------------------------------------------------------------
void ImageController::addImage1(ofImage img)
{
    deleteImage1();
    
    // If the image is not square
    if (img.getWidth() != img.getHeight()) {
        cout << "Image Not Square" <<endl;
        
        int tdfWidth = img.getWidth();
        int tdfHeight = img.getHeight();
        
        if (tdfWidth > tdfHeight) {
            cout << "W > H" << endl;
            image1.cropFrom(img, tdfWidth/2-(tdfHeight/2),0, tdfHeight,tdfHeight);
        }
        else if(tdfHeight > tdfWidth) {
            cout << "H > W" << endl;
            image1.cropFrom(img, 0, tdfHeight/2-(tdfWidth/2), tdfWidth,tdfWidth);
        }
        else {
            cout << "No" << endl;
        }
    }
    else{
        image1.setFromPixels(img.getPixels(),img.getWidth(),img.getHeight(),OF_IMAGE_COLOR);
    }
}
//--------------------------------------------------------------
void ImageController::addImage2(ofImage img)
{
    deleteImage2();
    // If the image is not square
    if (img.getWidth() != img.getHeight()) {
        cout << "Image Not Square" <<endl;
        
        int tdfWidth = img.getWidth();
        int tdfHeight = img.getHeight();
        
        if (tdfWidth > tdfHeight) {
            cout << "W > H" << endl;
            image2.cropFrom(img, tdfWidth/2-(tdfHeight/2),0, tdfHeight,tdfHeight);
        }
        else if(tdfHeight > tdfWidth) {
            cout << "H > W" << endl;
            image2.cropFrom(img, 0, tdfHeight/2-(tdfWidth/2), tdfWidth,tdfWidth);
        }
        else {
            cout << "No" << endl;
        }
    }
    else{
        image2.setFromPixels(img.getPixels(),img.getWidth(),img.getHeight(),OF_IMAGE_COLOR);
    }
}
//--------------------------------------------------------------
void ImageController::deleteImage1()
{
    image1.clear();
}
//--------------------------------------------------------------
void ImageController::deleteImage2()
{
    image2.clear();
}
#pragma mark - Animations
//--------------------------------------------------------------
bool ImageController::hasTweenFinished()
{
    if (tweenImage1.isCompleted() && tweenImage2.isCompleted()) {
        return true;
    }
    else{
        return false;
    }
}
//--------------------------------------------------------------
bool ImageController::hasTweenGotHalfway()
{
    if (tweenImage1.isHalfway() && tweenImage2.isHalfway()) {
        return true;
    }
    else{
        return false;
    }
}
//--------------------------------------------------------------
bool ImageController::hasFadeToWhiteFinished()
{
    if (fadeToWhiteTween.isCompleted()) {
        return true;
    }
    else{
        return false;
    }
}
//--------------------------------------------------------------
void ImageController::stopAnimating()
{
    tweenImage1.completed = true;
    tweenImage2.completed = true;
}
void ImageController::animate1to2(int type)
{
    switch (type) {
        case ANIMATION_TYPE_BACK:
            tweenImage1.setParameters(easingback, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingback, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_BOUNCE:
            tweenImage1.setParameters(easingbounce, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingbounce, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_CIRC:
            tweenImage1.setParameters(easingcirc, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingcirc, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_CUBIC:
            tweenImage1.setParameters(easingcubic, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingcubic, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_ELASTIC:
            tweenImage1.setParameters(easingelastic, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingelastic, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_EXPO:
            tweenImage1.setParameters(easingexpo, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingexpo, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_LINEAR:
            tweenImage1.setParameters(easinglinear, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easinglinear, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_QUAD:
            tweenImage1.setParameters(easingquad, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingquad, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_QUART:
            tweenImage1.setParameters(easingquart, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingquart, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_QUINT:
            tweenImage1.setParameters(easingquint, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingquint, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        case ANIMATION_TYPE_SINE:
            tweenImage1.setParameters(easingsine, ofxTween::easeInOut, 0, -320, _time, 10);
            tweenImage2.setParameters(easingsine, ofxTween::easeInOut, 320, 0, _time, 10);
            break;
        default:
            break;
    }
}
//--------------------------------------------------------------
void ImageController::animate2to1(int type)
{
    switch (type) {
        case ANIMATION_TYPE_BACK:
            tweenImage1.setParameters(easingback, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingback, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_BOUNCE:
            tweenImage1.setParameters(easingbounce, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingbounce, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_CIRC:
            tweenImage1.setParameters(easingcirc, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingcirc, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_CUBIC:
            tweenImage1.setParameters(easingcubic, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingcubic, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_ELASTIC:
            tweenImage1.setParameters(easingelastic, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingelastic, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_EXPO:
            tweenImage1.setParameters(easingexpo, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingexpo, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_LINEAR:
            tweenImage1.setParameters(easinglinear, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easinglinear, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_QUAD:
            tweenImage1.setParameters(easingquad, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingquad, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_QUART:
            tweenImage1.setParameters(easingquart, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingquart, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_QUINT:
            tweenImage1.setParameters(easingquint, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingquint, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        case ANIMATION_TYPE_SINE:
            tweenImage1.setParameters(easingsine, ofxTween::easeInOut, -320, 0, _time, 10);
            tweenImage2.setParameters(easingsine, ofxTween::easeInOut, 0, 320, _time, 10);
            break;
        default:
            break;
    }
}
//--------------------------------------------------------------
void ImageController::fade1to2(int type)
{
    switch (type) {
        case ANIMATION_TYPE_BACK:
            tweenImage1.setParameters(easingback, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easingback, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        case ANIMATION_TYPE_BOUNCE:
            tweenImage1.setParameters(easingbounce, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easingbounce, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        case ANIMATION_TYPE_CIRC:
            tweenImage1.setParameters(easingcirc, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easingcirc, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        case ANIMATION_TYPE_CUBIC:
            tweenImage1.setParameters(easingcubic, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easingcubic, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        case ANIMATION_TYPE_EXPO:
            tweenImage1.setParameters(easingexpo, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easingexpo, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        case ANIMATION_TYPE_LINEAR:
            tweenImage1.setParameters(easinglinear, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easinglinear, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        case ANIMATION_TYPE_QUAD:
            tweenImage1.setParameters(easingquad, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easingquad, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        case ANIMATION_TYPE_QUART:
            tweenImage1.setParameters(easingquart, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easingquart, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        case ANIMATION_TYPE_QUINT:
            tweenImage1.setParameters(easingquint, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easingquint, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        case ANIMATION_TYPE_SINE:
            tweenImage1.setParameters(easingsine, ofxTween::easeInOut, 255, 0, _time, 10);
            tweenImage2.setParameters(easingsine, ofxTween::easeInOut, 50, 255, _time, 10);
            break;
        default:
        break;
    }
}
//--------------------------------------------------------------
void ImageController::fade2to1(int type)
{
    switch (type) {
        case ANIMATION_TYPE_BACK:
            tweenImage1.setParameters(easingback, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easingback, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_BOUNCE:
            tweenImage1.setParameters(easingbounce, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easingbounce, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_CIRC:
            tweenImage1.setParameters(easingcirc, ofxTween::easeInOut, 0, 255, _time, 10);
            tweenImage2.setParameters(easingcirc, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_CUBIC:
            tweenImage1.setParameters(easingcubic, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easingcubic, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_ELASTIC:
            tweenImage1.setParameters(easingelastic, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easingelastic, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_EXPO:
            tweenImage1.setParameters(easingexpo, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easingexpo, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_LINEAR:
            tweenImage1.setParameters(easinglinear, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easinglinear, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_QUAD:
            tweenImage1.setParameters(easingquad, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easingquad, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_QUART:
            tweenImage1.setParameters(easingquart, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easingquart, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_QUINT:
            tweenImage1.setParameters(easingquint, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easingquint, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_SINE:
            tweenImage1.setParameters(easingsine, ofxTween::easeInOut, 50, 255, _time, 10);
            tweenImage2.setParameters(easingsine, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        default:
            break;
    }
}
//--------------------------------------------------------------
void ImageController::fadeWhiteIn(int type)
{
    switch (type) {
        case ANIMATION_TYPE_BACK:
            fadeToWhiteTween.setParameters(easingback, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_BOUNCE:
            fadeToWhiteTween.setParameters(easingbounce, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_CIRC:
            fadeToWhiteTween.setParameters(easingcirc, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_CUBIC:
            fadeToWhiteTween.setParameters(easingcubic, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_ELASTIC:
            fadeToWhiteTween.setParameters(easingelastic, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_EXPO:
            fadeToWhiteTween.setParameters(easingexpo, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_LINEAR:
            fadeToWhiteTween.setParameters(easinglinear, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_QUAD:
            fadeToWhiteTween.setParameters(easingquad, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_QUART:
            fadeToWhiteTween.setParameters(easingquart, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_QUINT:
            fadeToWhiteTween.setParameters(easingquint, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        case ANIMATION_TYPE_SINE:
            fadeToWhiteTween.setParameters(easingsine, ofxTween::easeInOut, 0, 255, _time, 10);
            break;
        default:
            break;
    }
}
//--------------------------------------------------------------
void ImageController::fadeWhiteOut(int type)
{
    switch (type) {
        case ANIMATION_TYPE_BACK:
            fadeToWhiteTween.setParameters(easingback, ofxTween::easeInOut, 255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_BOUNCE:
            fadeToWhiteTween.setParameters(easingbounce, ofxTween::easeInOut,255,0, _time, 10);
            break;
        case ANIMATION_TYPE_CIRC:
            fadeToWhiteTween.setParameters(easingcirc, ofxTween::easeInOut,255,0, _time, 10);
            break;
        case ANIMATION_TYPE_CUBIC:
            fadeToWhiteTween.setParameters(easingcubic, ofxTween::easeInOut, 255,0, _time, 10);
            break;
        case ANIMATION_TYPE_ELASTIC:
            fadeToWhiteTween.setParameters(easingelastic, ofxTween::easeInOut,255,0, _time, 10);
            break;
        case ANIMATION_TYPE_EXPO:
            fadeToWhiteTween.setParameters(easingexpo, ofxTween::easeInOut, 255,0, _time, 10);
            break;
        case ANIMATION_TYPE_LINEAR:
            fadeToWhiteTween.setParameters(easinglinear, ofxTween::easeInOut, 255,0, _time, 10);
            break;
        case ANIMATION_TYPE_QUAD:
            fadeToWhiteTween.setParameters(easingquad, ofxTween::easeInOut,255, 0, _time, 10);
            break;
        case ANIMATION_TYPE_QUART:
            fadeToWhiteTween.setParameters(easingquart, ofxTween::easeInOut,255,0, _time, 10);
            break;
        case ANIMATION_TYPE_QUINT:
            fadeToWhiteTween.setParameters(easingquint, ofxTween::easeInOut,255,0, _time, 10);
            break;
        case ANIMATION_TYPE_SINE:
            fadeToWhiteTween.setParameters(easingsine, ofxTween::easeInOut,255, 0,_time, 10);
            break;
        default:
            break;
    }
}

