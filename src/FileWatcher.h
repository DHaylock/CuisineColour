//
//  FileWatcher.h
//  CusineColor
//
//  Created by David Haylock on 11/02/2015.
//
//

#include "ofMain.h"
#include "ofxIO.h"

using namespace ofx::IO;
class JpgPathFilter: public AbstractPathFilter
{
public:
    JpgPathFilter()
    {
        
    }
    
    virtual ~JpgPathFilter()
    {
    }
    
    bool accept(const Poco::Path& path) const
    {
        // don't return hidden files
        return !Poco::File(path).isHidden() &&
        ofIsStringInString(path.toString(), ".jpg");
    }
};

class FileWatcher {
    public:

    void setup(std::string jpgPath,std::string gifPath);
    
        vector<string> getAllJpgFiles();
        string getNewJpgFileName();
        bool isNewJpgFile();
        void onJpgDirectoryWatcherItemAdded(const DirectoryWatcherManager::DirectoryEvent& evt);
    
        DirectoryWatcherManager jpgWatcher;
        HiddenFileFilter fileFilter;
        JpgPathFilter jpgFilter;
    private:
        string _jpgPath;
        vector<string> jpgPaths;
        bool pushNewJpgFile;
    protected:
    
    
};