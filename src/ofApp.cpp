#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    this->grabber.setDeviceID( 1 );

    this->grabber.setDesiredFrameRate(30);
    this->grabber.initGrabber(this->cameraWidth, this->cameraHeight);

    // image to be drawn
    this->screenImage.allocate(this->cameraWidth, this->cameraHeight, OF_IMAGE_COLOR_ALPHA);

    // paints with white
    unsigned char* pixels = this->screenImage.getPixels();
    for (int i =0; i<this->screenImage.getWidth() * this->screenImage.getHeight() * 4; i++) {
        pixels[i] = 255;
    }

    // check if we have a image to load
    ofImage previousImage;
    if (previousImage.loadImage("pendulum_clock.png")) {

        if (previousImage.getWidth() == this->screenImage.getWidth() && previousImage.getHeight() == this->screenImage.getHeight()) {

            this->screenImage = previousImage;

        }

    }

    // where we are going to start copying pixels

    this->step = 0;
    this->x = this->cameraWidth / 2;
    this->pickParameters();
    this->pickSpeed();

}

//--------------------------------------------------------------
void ofApp::update(){

    this->grabber.update();
    if (this->grabber.isFrameNew()) {
        ofPixels pixels = this->grabber.getPixelsRef();

        for (int i = 0; i < this->speed; i++) {

            for (int j = 0; j < this->height && (this->y + j) < (this->cameraHeight - 1); j++) {

                float halfHeight = ((float) this->height) / 2;
                float alpha = 1.0;
                if (j <halfHeight) {
                    alpha = (((float) j) / halfHeight * 0.8) + 0.2;
                } else if (j >halfHeight) {
                    alpha = (((float) height - j) / halfHeight * 0.8) + 0.2;
                }

                this->paintPixel( this->x, this->y + j, pixels, alpha);
            }

            if (this->step == 0) {
                --this->x;
                if (this->x < 0) {
                    this->x = 1;
                    this->step = 1;
                }
            } else if (this->step == 1) {
                ++this->x;
                if (this->x == this->cameraWidth/2) {
                    // changes y and height, but keeps speed
                    this->pickParameters();
                }
                else if (this->x >= this->cameraWidth) {
                    this->x = this->cameraWidth - 2;
                    this->step = 2;
                }
            } else {
                --this->x;
                if (this->x < this->cameraWidth / 2) {
                    this->step = 0;
                    this->pickParameters();
                    this->pickSpeed();
                    break;
                }
            }
        }

        this->screenImage.update();


        if (ofGetElapsedTimef() - this->lastTimeImageWasSaved > this->intervalToSaveImage * 60) {
            // image we will try to load on start up
            this->screenImage.saveImage("pendulum_clock.png");

            // image we will save to have a recording of work
            char filename[64];
            sprintf( filename, "pendulum_clock_%04d_%02d_%02d_%02d_%02d_%02d.png", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds() );

            this->screenImage.saveImage(filename);

            this->lastTimeImageWasSaved = ofGetElapsedTimef();
        }

    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    this->screenImage.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

void ofApp::pickParameters() {
    this->y = (int) ofRandom(0, this->cameraHeight);

    this->height = (int) ofRandom(1, this->maxHeight);
}

void ofApp::pickSpeed() {
    this->speed = (int) ofRandom(1, this->maxSpeed);
}

void ofApp::paintPixel( int pixelX, int pixelY, ofPixels pixels, float strength )
{
    if (pixelX < 0 || pixelX >= pixels.getWidth()) {
        return;
    }
    if (pixelY < 0 || pixelY >= pixels.getHeight()) {
        return;
    }

    ofColor newColor = pixels.getColor( pixelX, pixelY );
    ofColor oldColor = this->screenImage.getPixelsRef().getColor( pixelX, pixelY );
    float diffR = newColor.r - oldColor.r;
    float diffG = newColor.g - oldColor.g;
    float diffB = newColor.b - oldColor.b;

    ofColor resultColor;
    resultColor.r = oldColor.r + (int) (diffR * strength);
    resultColor.g = oldColor.g + (int) (diffG * strength);
    resultColor.b = oldColor.b + (int) (diffB * strength);

    this->screenImage.setColor( pixelX, pixelY, resultColor);
}
