#pragma once

#include "ofMain.h"
#include "ofxUI.h"

class ofApp : public ofBaseApp{
private:
    ofxUICanvas *gui;
    ofxUICanvas *cameraPanel;
    ofxUICanvas *imagePanel;

    ofVideoGrabber* videoGrabber;
    int selectedCameraIndex;

    ofImage screenImage;

    int step;
    int y;
    int x;
    int speed;
    int height;

    int maxSpeed;
    int maxHeight;

    int cameraWidth;
    int cameraHeight;

    int imageWidth;
    int imageHeight;

    float lastTimeImageWasSaved;
    int intervalToSaveImage;

    const static string CAMERA_WIDTH_LABEL;
    const static string CAMERA_HEIGHT_LABEL;
    const static string MAX_SPEED_LABEL;
    const static string MAX_HEIGHT_LABEL;
    const static string SAVE_IMAGE_LABEL;
    const static string SAVE_LABEL;
    const static string CANCEL_LABEL;
    const static string RESET_IMAGE_LABEL;
    const static string SUPPORT_BUTTON_NAME;

    const static string ZERO_DEGREES_LABEL;
    const static string NINETY_DEGREES_LABEL;
    const static string ONE_HUNDRED_EIGHTY_DEGREES_LABEL;
    const static string TWO_HUNDRED_SEVENTY_DEGREES_LABEL;

    ofxUISpacer* titleSpacer;
    ofxUIDropDownList* cameraList;
    ofxUITextInput* cameraWidthTextInput;
    ofxUITextInput* cameraHeightTextInput;
    ofxUITextInput* maxSpeedTextInput;
    ofxUITextInput* maxHeightTextInput;
    ofxUITextInput* intervalToSaveTextInput;

    ofxUIToggle* zeroRotationToggle;
    ofxUIToggle* ninetyRotationToggle;
    ofxUIToggle* oneHundredEightyRotationToggle;
    ofxUIToggle* twoHundredSeventyRotationToggle;
    int rotations;

    ofxUIToggle* showAtStartupToggle;
    bool showAtStartup;

    ofxUIToggle* fullScreenToggle;
    bool fullScreen;

    ofxUIToggle* saveImageToggle;
    bool saveImage;

    std::vector<ofxUITextInput*> textInputs;

    void pickParameters();
    void pickSpeed();
    void paintPixel( int pixelX, int pixelY, ofPixels pixels, float strength );

    void fillImageWithWhite( ofImage* image );
    void saveCurrentImage();

    void reset();
    void cancelConfigurationChanges();
    void applyConfigurationChanges();
    void hideConfigurationPanel();
    void showConfigurationPanel();
    void unfocusAllTextInputs(ofxUITextInput* except);
    void checkTextInputFocus(ofxUIEventArgs &e);

public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    // ofxUI
    void exit();
    void guiEvent(ofxUIEventArgs &e);
    void cameraPanelEvent(ofxUIEventArgs &e);
    void imagePanelEvent(ofxUIEventArgs &e);

};
