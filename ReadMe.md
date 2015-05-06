Cuisine Color
===

![alt text](CuisineColor.jpg "Logo")

####Introduction

This app has been created for the Sabrina Shirazi of [Cuisine Color](http://cuisineandcolour.com/). The original app may have altered due to the specifics of the project. This version contains the primary features and demonstrates what the app did.

The app does three things.

1. Pulls images from Instagram and Twitter on to the hardrive of the users computer.
2. Allows the user to select,place, display and animate images from the performance.
3. Output the visual data into Madmapper where it is mapped subsequential mapped to a custom surface.

The app is written in c++ using openFrameworks and has the following dependancies.

* [ofxInstagram](https://github.com/DHaylock/ofxInstagram) 
* [ofxOAuth](https://github.com/bakercp/ofxOAuth)
* [ofxJSON](https://github.com/jefftimesten/ofxJSON)
* [ofxSyphon](https://github.com/astellato/ofxSyphon)
* [ofxTween](https://github.com/arturoc/ofxTween) (which has been customised)
* [ofxIO](https://github.com/bakercp/ofxIO)
* [ofxSimpleTimer](https://github.com/HeliosInteractive/ofxSimpleTimer)
* [ofxUI](https://github.com/rezaali/ofxUI)
* ofxThreadedImageLoader (comes as standard in the oF download package)

The interface is spilt into 7 sections.
* Output
* Image Pool
* Twitter Pool
* Instagram Pool
* Preview and Selection
* GUI

####Using the App
- Before launching place you credentials into a cred.json file inside the Credential folder.

<pre>{
    "instagramOAuth": "!",
    "twitterConsumerKey": "!",
    "twitterConsumerSecret": "!",
    "twitterAccessToken":"!",
    "twitterAccessTokenSecret": "!"
}</pre>

- Launch the App

#####General Use
- On launch there should be a selection of images in the Image pool section.
- To add an image into the output frames. Select the appropriate frame from the Select Frame GUI Matrix.
- Choose which sub frame you would like to add it to from the To Which Frame Section
- Then click the image in the pool you want to display. This should add the image, then press 'c' which clears the frame selection.

#####Instagram and Twitter
- Simple click the API Call you would like, the images should start appearing in the desired area. When they have finished, click the stop loaders button.
- Then Simply click the image you want, providing it has not already been added the app will save the image to the drive and it will appear in the Image pool.

