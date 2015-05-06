//
//  ImagePool.cpp
//  CusineColor
//
//  Created by David Haylock on 11/02/2015.
//
//

#include "ImagePool.h"
#pragma mark - Setup
//--------------------------------------------------------------
void ImagePool::setup(int x, int y)
{
    count = 0;
    _x = x;
    _y = y;
    showPreviewImage = false;
}
#pragma mark - Drawers
//--------------------------------------------------------------
void ImagePool::draw(int x, int y)
{
    if(!imagesInFolder.empty()) {
        for(int i = 0; i < imagesInFolder.size(); i++) {
            if (imagesInFolder[i].isAllocated()) {
                imagesInFolder[i].draw(imagesInFolder[i].getOrigin(), 50,50);
            }
        }
    }
    if (showPreviewImage) {
        ofRect(_mouseX, _mouseY-250, 250,250);
        previewImage.draw(_mouseX, _mouseY-250, 250,250);
    }

}
//--------------------------------------------------------------
void ImagePool::addNewImage(string file)
{
    ImageSelector s;
    s.loadImage(file);
    s.setImageCaption(file);

    if (imagesInFolder.size() % 16 == 0) {
        s.setup(_x,_y+(imagesInFolder.size()/16*50)+1,50,50);
    }
    else{
        s.setup(imagesInFolder.back().getOrigin().x+51, imagesInFolder.back().getOrigin().y, 50, 50);
    }

    imagesInFolder.push_back(s);
}
//--------------------------------------------------------------
void ImagePool::getListOfFilesCurrentlyInDirectory(vector<string> files)
{
    int rx,ry;
    int spacing = 51;
    
    ImageSelector s;
    for (int i = 0; i < files.size(); i++) {
        s.loadImage(files[i]);
        for (int row = 0; row < files.size(); row++) {
            if ((i >= (row*16))&&(i <= ((row*16)+15))) {
                rx = _x+((i-(row*16))*spacing);
                ry = _y+(spacing*row);
            }
        }
        s.setup(rx,ry, 50, 50);
        s.setImageCaption(files[i]);
        imagesInFolder.push_back(s);
    }
}
//--------------------------------------------------------------
bool ImagePool::isMouseOver(int x, int y)
{
    _mouseX = x;
    _mouseY = y;
    
    if (!imagesInFolder.empty()) {
        for(int i = 0; i < imagesInFolder.size(); i++) {
            if (imagesInFolder[i].isMouseOver(x,y)){
                previewImage.clear();
                previewImage = imagesInFolder[i];
                showPreviewImage = true;
                return true;
            }
            else{
                showPreviewImage = false;
            }
        }
    }
}
//--------------------------------------------------------------
bool ImagePool::isClicked(int x, int y)
{
    if (!imagesInFolder.empty()) {
        for(int i = 0; i < imagesInFolder.size(); i++) {
            if (imagesInFolder[i].isClicked(x, y)) {
                img.clear();
                img = imagesInFolder[i];
                _imgID = imagesInFolder[i].getImageCaption();
                return true;
            }
        }
    }
    else{
        return false;
    }
}
//--------------------------------------------------------------
string ImagePool::getImageName()
{
    return _imgID;
}
//--------------------------------------------------------------
ofImage ImagePool::returnClickedImage()
{
    return img;
}
//--------------------------------------------------------------
ofImage ImagePool::getImage(int imgNo)
{
    return imagesInFolder[imgNo];
}