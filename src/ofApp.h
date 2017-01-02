#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include <map>

class ofApp : public ofBaseApp{
private:
    
    ofXml settings;
    ofxImGui*gui;
    GLuint imageButtonID;
    static int maxSpeed;
    static int maxHeight;
    static int intervalToSaveImage;
    static int degreesButtonValue;
    static int currentResolution;
    static bool saveImageToggle;
    static bool showAtStartup;
    static bool fullscreen;
    static bool configurationPanelShow;
    const char* deviceResolution[5];
    
    std::map<string, string> ptStrings;
    std::map<string, string> enStrings;
    std::map<string, string> currentStrings;
    const static string CHANGE_LOCALE_BUTTON_NAME;
    const static string ENGLISH_LABEL;
    const static string PORTUGUESE_LABEL;
    const static int LOCALE_ENGLISH = 0;
    const static int LOCALE_PORTUGUESE = 1;
    int currentLocale;
    string changeLocaleLabel;
    
    const static float MAX_STRENGTH_AROUND_PIXEL;
    
    ofVideoGrabber* videoGrabber;
    int selectedCameraIndex;
    
    ofImage screenImage;
    
    int step;
    int y;
    int x;
    int speed;
    int height;
    
    int cameraWidth;
    int cameraHeight;
    int imageWidth;
    int imageHeight;
    
    float lastTimeImageWasSaved;
    
    int rotations;
    
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
    void changeLocale();
    void saveXmlSettings();
    void loadXmlSettings();
    void setFullscreen();
    void selectResolution();
    
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void mouseReleased(ofMouseEventArgs&);
};
