//
//  FileWatcher.cpp
//  CusineColor
//
//  Created by David Haylock on 11/02/2015.
//
//

#include "FileWatcher.h"

//--------------------------------------------------------------
void FileWatcher::setup(std::string jpgPath,std::string gifPath)
{
    ofAddListener(jpgWatcher.events.onItemAdded, this,&FileWatcher::onJpgDirectoryWatcherItemAdded);
    
    bool listExistingItemsOnStart = false;
    std::string jpgFolderToWatch = ofToDataPath(jpgPath, false);
    
    jpgWatcher.addPath(jpgFolderToWatch,listExistingItemsOnStart,&fileFilter);
    
    DirectoryUtils::listRecursive(jpgFolderToWatch, jpgPaths,false, &jpgFilter,DirectoryUtils::INIFINITE_DEPTH,DirectoryUtils::SIBLINGS_FIRST);
    
    pushNewJpgFile = false;
}
//--------------------------------------------------------------
vector <string> FileWatcher::getAllJpgFiles()
{
    return jpgPaths;
}
//--------------------------------------------------------------
string FileWatcher::getNewJpgFileName()
{
    return _jpgPath;
}
//--------------------------------------------------------------
bool FileWatcher::isNewJpgFile()
{
    if (pushNewJpgFile) {
        pushNewJpgFile = false;
        return true;
    }
    else {
        return false;
    }
}
//--------------------------------------------------------------
void FileWatcher::onJpgDirectoryWatcherItemAdded(const DirectoryWatcherManager::DirectoryEvent& evt)
{
    pushNewJpgFile = true;
    if (ofIsStringInString(evt.item.path(), ".jpg")) {
        _jpgPath = evt.item.path();
    }
}