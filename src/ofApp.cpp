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
    
    
    this->startLine();

}

//--------------------------------------------------------------
void ofApp::update(){
    
    this->grabber.update();
    if (this->grabber.isFrameNew()) {
        ofPixels pixels = this->grabber.getPixelsRef();
        
        for (int i = 0; i < this->speed; i++) {
            
            for (int j = 0; j < this->height && (this->y + j) < (this->cameraHeight - 1); j++) {
            
                this->screenImage.setColor( this->x, this->y + j, pixels.getColor(this->x, this->y + j));
            }
            
            if (this->step == 0) {
                --this->x;
                if (this->x < 0) {
                    this->x = 1;
                    this->step = 1;
                }
            } else if (this->step == 1) {
                ++this->x;
                if (this->x >= this->cameraWidth) {
                    this->x = this->cameraWidth - 2;
                    this->step = 2;
                }
            } else {
                --this->x;
                if (this->x <= this->cameraWidth / 2) {
                    this->startLine();
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

void ofApp::startLine() {
    this->x = this->cameraWidth / 2;
    this->y = (int) ofRandom(0, this->cameraHeight);
    
    this->step = 0;
    this->height = (int) ofRandom(1, this->maxHeight);
    this->speed = (int) ofRandom(1, this->maxSpeed);
    
}
