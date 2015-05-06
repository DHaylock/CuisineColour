//
//  ImageSelector.h
//  CusineColor
//
//  Created by David Haylock on 06/02/2015.
//
//

#ifndef __CusineColor__ImageSelector__
#define __CusineColor__ImageSelector__

#include <stdio.h>
#include "ofMain.h"
//--------------------------------------------------------------
class ImageSelector : public ofImage {
public:
    bool selected = false;
    bool remove = false;
    ImageSelector(){    }
    //--------------------------------------------------------------
    void setup(int x,int y,int w, int h){
        _x = x;
        _y = y;
        _w = w;
        _h = h;
    }
    //--------------------------------------------------------------
    void setImageCaption(string caption){
        _imageCaption = caption;
    }
    //--------------------------------------------------------------
    ofPoint getOrigin()
    {
        return ofPoint(_x,_y);
    }
    void drawOutline()
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
        ofRect(_x, _y, _w, _h);
        ofPopStyle();
    }
    //--------------------------------------------------------------
    string getImageCaption(){
        return _imageCaption;
    }
    //--------------------------------------------------------------
    bool isClicked(int x,int y)
    {
        if (((x >= _x)&&(x <= _x+_w))&&((y >= _y)&&( y <= _y+_h)))
        {
            selected = true;
            return true;
        }
        else
        {
            selected = false;
            return false;
        }
    }
    //--------------------------------------------------------------
    bool isMouseOver(int x,int y)
    {
        if (((x >= _x)&&(x <= _x+_w))&&((y >= _y)&&( y <= _y+_h)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    bool isEditable()
    {
        return selected;
    }
    //--------------------------------------------------------------
    bool removeElement(int x, int y)
    {

    }
    //--------------------------------------------------------------
    void clear()
    {
        
    }
    
private:
    int _x;
    int _y;
    int _w;
    int _h;
    string _imageCaption;
};

#endif /* defined(__CusineColor__imageSelector__) */
