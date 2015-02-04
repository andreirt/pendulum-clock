#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

    ofVideoGrabber grabber;
    int selectedCameraIndex = 0;

    ofImage screenImage;

    int step = 0;
    int y;
    int x;
    int speed;
    int height;

    int maxSpeed = 16;
    int maxHeight = 48;

    int cameraWidth = 1280;
    int cameraHeight = 720;

    float lastTimeImageWasSaved = 0;
    int intervalToSaveImage = 15;

    void pickParameters();
    void pickSpeed();
    void paintPixel( int pixelX, int pixelY, ofPixels pixels, float strength );

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

};
