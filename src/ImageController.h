//
//  ImageController.h
//  CusineColor
//
//  Created by David Haylock on 10/02/2015.
//
//
enum AnimationType
{
    ANIMATION_TYPE_BACK,
    ANIMATION_TYPE_BOUNCE,
    ANIMATION_TYPE_CIRC,
    ANIMATION_TYPE_CUBIC,
    ANIMATION_TYPE_ELASTIC,
    ANIMATION_TYPE_EXPO,
    ANIMATION_TYPE_LINEAR,
    ANIMATION_TYPE_QUAD,
    ANIMATION_TYPE_QUART,
    ANIMATION_TYPE_QUINT,
    ANIMATION_TYPE_SINE
};

enum ImageType
{
    CUSINE_IMAGE
};

#include <stdio.h>
#include "ImageSelector.h"
#include "ofMain.h"
#include "ofxTween.h"
//--------------------------------------------------------------
class ImageController {
    public:
        ImageController(){  }
        ~ImageController(){  }
    
        void setup(int x, int y, int w, int h);
        void update();
        void draw();
    
        void setSlot1(ImageType type);
        void setSlot2(ImageType type);
        int getControllerType();
    
        void setControllerID(int id);
        int getControllerID();
    
        void deleteImage1();
        void deleteImage2();
    
        void addImage1(ofImage img);
        void addImage2(ofImage img);
    
        // Preview
        void drawImage1(int x,int y);
        void drawImage2(int x,int y);
    
        void stopAnimating();
        bool isClicked(int x,int y);
        void isSelected(bool _selected);
        bool isEditable();
        bool removeElement(int x, int y);
    
        void fade1to2(int type);
        void fade2to1(int type);
        void fadeWhiteIn(int type);
        void fadeWhiteOut(int type);
        void setAnimationLength(int time);
        void animate1to2(int type);
        void animate2to1(int type);
        bool hasTweenFinished();
        bool hasTweenGotHalfway();
        bool hasFadeToWhiteFinished();
        void setMoveOrFade(bool val);
        void setWhiteOrImage(bool val);
    
        void setImage1ID(string id);
        void setImage2ID(string id);
        string getImage1ID();
        string getImage2ID();
    
        int getCurrentFrame();
    
        void setFadeToWhite(bool value);
        void setImageCaption(string caption);
        ofPoint getOrigin();
    
        void drawOutline();
        string getImageCaption();

    private:
        ofImage image1;
        ofImage image2;
        ImageType slot1;
        ImageType slot2;
        ofFbo scene;
        int _x;
        int _y;
        int _w;
        int _h;
        bool _fadeToWhite;
        string _imageCaption;
        string _image1ID;
        string _image2ID;
        bool selected;
        int _id;
        int _time;
        int _currentFrame;
        bool _moveOrFade;
        bool _whiteOrImage;
        ofxTween tweenImage1;
        ofxTween tweenImage2;
        ofxTween fadeToWhiteTween;
        ofxEasingBack 	easingback;
        ofxEasingBounce easingbounce;
        ofxEasingCirc 	easingcirc;
        ofxEasingCubic 	easingcubic;
        ofxEasingElastic easingelastic;
        ofxEasingExpo 	easingexpo;
        ofxEasingLinear easinglinear;
        ofxEasingQuad 	easingquad;
        ofxEasingQuart 	easingquart;
        ofxEasingQuint 	easingquint;
        ofxEasingSine	easingsine;
};