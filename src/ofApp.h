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

    int maxSpeed = 12;
    int maxHeight = 16;

    int cameraWidth = 1920;
    int cameraHeight = 1080;

    float lastTimeImageWasSaved = 0;
    int intervalToSaveImage = 15;

    void startLine();

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
