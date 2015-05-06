//
//  ImagePool.h
//  CusineColor
//
//  Created by David Haylock on 11/02/2015.
//
//

#include "ofMain.h"
#include "ImageSelector.h"

class ImagePool
{
public:
    
    deque <ImageSelector> imagesInFolder;
    void draw(int x, int y);
    
    void addNewImage(string file);
    void getListOfFilesCurrentlyInDirectory(vector<string>files);
    void setup(int x, int y);
    ofImage returnClickedImage();
    bool isClicked(int x, int y);
    string getImageName();
    bool isMouseOver(int x,int y);
    ofImage getImage(int imgNo);
    
private:
    int count;
    ofImage img;
    ofImage previewImage;
    int _x;
    int _y;
    string _imgID;
    bool showPreviewImage;
    int _mouseX;
    int _mouseY;
};