#include "ofApp.h"

const string ofApp::ENGLISH_LABEL = "English";
const string ofApp::PORTUGUESE_LABEL = "Português";

const float ofApp::MAX_STRENGTH_AROUND_PIXEL = .15;
const string ofApp::CHANGE_LOCALE_BUTTON_NAME = "changeLocale";

int ofApp::maxSpeed = 16;
int ofApp::maxHeight = 48;
int ofApp::intervalToSaveImage = 15;
int ofApp::degreesButtonValue = 0;
int ofApp::currentResolution = 2;
bool ofApp::saveImageToggle = false;
bool ofApp::showAtStartup = true;
bool ofApp::fullscreen = true;
bool ofApp::configurationPanelShow = true;
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::setup(){
    gui = new ofxImGui;
    gui->setup();
    imageButtonID = gui->loadImage("funarte.png");
    this->step = 0;
    this->imageWidth = 800;
    this->imageHeight = 600;
    this->lastTimeImageWasSaved = 0;
    this->rotations = 0;
    this->selectedCameraIndex = 0;
    this->currentLocale = LOCALE_ENGLISH;
    this->changeLocaleLabel = PORTUGUESE_LABEL;
    this->videoGrabber = new ofVideoGrabber();
    
    ofFile stringsFile;
    stringsFile.open("strings.xml");
    ofBuffer stringsBuffer = stringsFile.readToBuffer();
    ofXml strings;
    strings.loadFromBuffer( stringsBuffer );
    strings.setTo("data");
    int numberOfStrings = strings.getNumChildren();
    for (int i = 0; i < numberOfStrings; i++) {
        strings.setToChild(i);
        
        string tagName = strings.getName();
        
        strings.setTo("pt");
        this->ptStrings[tagName ] = strings.getValue();
        
        strings.setTo("../en");
        this->enStrings[tagName ] = strings.getValue();
        strings.setToParent();
        
        strings.setToParent();
    }
    this->currentStrings = this->enStrings;
    
    this->loadXmlSettings();
    
    this->selectResolution();
    this->applyConfigurationChanges();
    
    if (this->showAtStartup) {
        this->showConfigurationPanel();
    }
    else {
        this->hideConfigurationPanel();
    }
    
    this->reset();
}

//----------------------------------------------------------------------------------------------------------------------------
void ofApp::reset(){
    if (this->videoGrabber != nullptr) {
        if (this->videoGrabber->isInitialized()) {
            this->videoGrabber->close();
            delete this->videoGrabber;
            this->videoGrabber = new ofVideoGrabber();
        }
    }
    
    // set camera
    if (selectedCameraIndex > 0) {
        this->videoGrabber->setDeviceID( selectedCameraIndex );
    }
    else {
        this->videoGrabber->setDeviceID( 0 );
    }
    
    //    this->videoGrabber->setDesiredFrameRate(30);
    this->videoGrabber->initGrabber(this->cameraWidth, this->cameraHeight);
    
    // image to be drawn
    this->screenImage.allocate(this->imageWidth, this->imageHeight, OF_IMAGE_COLOR_ALPHA);
    this->fillImageWithWhite( &this->screenImage );
    
    // check if we have a image to load
    ofImage previousImage;
    if (previousImage.load("pendulum_clock.png")) {
        
        if (previousImage.getWidth() == this->screenImage.getWidth() && previousImage.getHeight() == this->screenImage.getHeight()) {
            
            this->screenImage = previousImage;
        }
    }
    
    this->lastTimeImageWasSaved = 0;
    
    // where we are going to start copying pixels
    this->step = 0;
    this->x = this->cameraWidth / 2;
    this->pickParameters();
    this->pickSpeed();
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::update(){
    this->videoGrabber->update();
    
    if (this->configurationPanelShow == true) {
        ofShowCursor();
        return;
    }
    
    if (this->videoGrabber->isFrameNew()) {
        ofPixels pixels = this->videoGrabber->getPixels();
        pixels.rotate90( this->rotations );
        
        for (int i = 0; i < this->speed; i++) {
            
            for (int j = 0; j < this->height && (this->y + j) < (this->imageHeight - 1); j++) {
                
                float halfHeight = ((float) this->height) / 2;
                float alpha = 1.0;
                if (j <halfHeight) {
                    alpha = ((float) j) / halfHeight;
                } else if (j >halfHeight) {
                    alpha = ((float) height - j) / halfHeight;
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
                if (this->x == this->imageWidth/2) {
                    // changes y and height, but keeps speed
                    this->pickParameters();
                }
                else if (this->x >= this->imageWidth) {
                    this->x = this->imageWidth - 2;
                    this->step = 2;
                }
            } else {
                --this->x;
                if (this->x < this->imageWidth / 2) {
                    this->step = 0;
                    this->pickParameters();
                    this->pickSpeed();
                    break;
                }
            }
        }
        
        this->screenImage.update();
        
        if (this->saveImageToggle)
        {
            if (ofGetElapsedTimef() - this->lastTimeImageWasSaved > this->intervalToSaveImage * 60) {
                // image we will try to load on start up
                this->screenImage.save("pendulum_clock.png");
                
                // image we will save to have a recording of work
                this->saveCurrentImage();
                this->lastTimeImageWasSaved = ofGetElapsedTimef();
            }
        }
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::draw(){
    // We scale the image so it covers all the screen.
    // The image is croppred if necessary.
    float hScale = (float) ofGetWidth() / (float) this->screenImage.getWidth();
    float vScale = (float) ofGetHeight() / (float) this->screenImage.getHeight();
    
    float scale = max(hScale, vScale);
    
    int scaledWidth = (int) roundf((float) this->screenImage.getWidth() * scale);
    int scaledHeight = (int) roundf((float) this->screenImage.getHeight() * scale);
    
    int x = (ofGetWidth() - scaledWidth) / 2;
    int y = (ofGetHeight() - scaledHeight) / 2;
    
    this->screenImage.draw(x, y, scaledWidth, scaledHeight);
    
    //Interface
    if(configurationPanelShow == true){
        gui->begin();
        ImGui::SetNextWindowSize(ofVec2f(800,500));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin(this->currentStrings["pendulumclock"].c_str());
        //ImGui::Text(this->currentStrings["pendulumclock"].c_str());
        if(ImGui::Button(this->changeLocaleLabel.c_str())){
            this->changeLocale();
        }
        
        vector< ofVideoDevice > devices = this->videoGrabber->listDevices();
        vector<string> *cameras = new vector<string>();
        vector<ofVideoDevice>::iterator it;
        
        for (it = devices.begin(); it != devices.end(); ++it) {
            ofVideoDevice device = *it;
            cameras->push_back(device.deviceName);
        }
        
        char* nameDevices = (char*) malloc(512 * cameras->size());
        char* current = nameDevices;
        for (int i = 0; i < cameras->size(); i++) {
            strcpy(current, cameras->at(i).c_str());
            current += strlen(cameras->at(i).c_str());
            *current = '\0';
            ++current;
        }
        ImGui::PushItemWidth(200);
        ImGui::Combo(this->currentStrings["pickCamera"].c_str(), &selectedCameraIndex, nameDevices, cameras->size());
        free(nameDevices);
        ImGui::PushItemWidth(100);
        ImGui::Combo(this->currentStrings["deviceResolution"].c_str(), &currentResolution, deviceResolution, 3);
        ImGui::Text(this->currentStrings["imageRotation"].c_str());
        ImGui::RadioButton(this->currentStrings["zeroDegress"].c_str(), &degreesButtonValue, 0); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["ninetyDegress"].c_str(), &degreesButtonValue, 1); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["oneHundredEightyDegress"].c_str(), &degreesButtonValue, 2); ImGui::SameLine();
        ImGui::RadioButton(this->currentStrings["twoHundredSeventyDegress"].c_str(), &degreesButtonValue, 3);
        ImGui::PushItemWidth(100);
        ImGui::SliderInt(this->currentStrings["maxSpeed"].c_str(), &maxSpeed, 2, 16);
        ImGui::SliderInt(this->currentStrings["maxHeight"].c_str(), &maxHeight, 4, 48);
        ImGui::Checkbox(this->currentStrings["saveImage"].c_str(), &saveImageToggle); ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::SliderInt(this->currentStrings["minutes"].c_str(), &intervalToSaveImage, 1, 60);
        
        if(ImGui::Button(this->currentStrings["resetImage"].c_str())){
            this->fillImageWithWhite(&this->screenImage);
            this->screenImage.save("pendulum_clock.png");
            this->x = 0;
            this->y = 0;
        }
        
        ImGui::Checkbox(this->currentStrings["showAtStartup"].c_str(), &showAtStartup);
        ImGui::Checkbox(this->currentStrings["fullScreen"].c_str(), &fullscreen);
        
        if(ImGui::Button(this->currentStrings["save"].c_str())){
            this->saveXmlSettings();
            this->applyConfigurationChanges();
            this->reset();
            this->hideConfigurationPanel();
        }
        ImGui::SameLine();
        if(ImGui::Button(this->currentStrings["cancel"].c_str())){
            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();
        }
        
        ImGui::Text(this->currentStrings["credits1"].c_str());
        ImGui::Text(this->currentStrings["credits2"].c_str());
        ImGui::Text(this->currentStrings["credits3"].c_str());
        ImGui::Text(this->currentStrings["credits4"].c_str());
        ImGui::Text(this->currentStrings["credits5"].c_str());
        ImGui::Text(this->currentStrings["support"].c_str());
        if(ImGui::ImageButton((ImTextureID)(uintptr_t)imageButtonID, ImVec2(509, 60))){
            ofLaunchBrowser("http://www.funarte.gov.br/");
        }
        
        setFullscreen();
        selectResolution();
        
        ImGui::End();
        gui->end();
    }
}

//----------------------------------------------------------------------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'c') {
        
        if (this->configurationPanelShow == true) {
            this->hideConfigurationPanel();
        }
        else {
            this->showConfigurationPanel();
        }
        
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::pickParameters(){
    this->y = (int) ofRandom(0, this->cameraHeight);
    this->height = (int) ofRandom(1, this->maxHeight);
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::pickSpeed(){
    this->speed = (int) ofRandom(1, this->maxSpeed);
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::paintPixel( int pixelX, int pixelY, ofPixels pixels, float strength ){
    if (pixelX < 0 || pixelX >= pixels.getWidth()) {
        return;
    }
    if (pixelY < 0 || pixelY >= pixels.getHeight()) {
        return;
    }
    
    ofColor newColor = pixels.getColor( pixelX, pixelY );
    ofColor oldColor = this->screenImage.getPixels().getColor( pixelX, pixelY );
    float diffR = newColor.r - oldColor.r;
    float diffG = newColor.g - oldColor.g;
    float diffB = newColor.b - oldColor.b;
    
    ofColor resultColor;
    resultColor.r = oldColor.r + (int) (diffR * strength);
    resultColor.g = oldColor.g + (int) (diffG * strength);
    resultColor.b = oldColor.b + (int) (diffB * strength);
    
    this->screenImage.setColor( pixelX, pixelY, resultColor);
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::fillImageWithWhite( ofImage* image ){
    image->setColor(ofColor::whiteSmoke);
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::saveCurrentImage(){
    char filename[64];
    sprintf( filename, "pendulum_clock_%04d_%02d_%02d_%02d_%02d_%02d.png", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds() );
    
    this->screenImage.save(filename);
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::hideConfigurationPanel(){
    this->configurationPanelShow = false;
    ofHideCursor();
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::showConfigurationPanel(){
    this->configurationPanelShow = true;
    ofShowCursor();
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::cancelConfigurationChanges(){
    maxSpeed = 16;
    maxHeight = 48;
    intervalToSaveImage= 15;
    degreesButtonValue = 0;
    saveImageToggle = false;
    showAtStartup = true;
    fullscreen = true;
    currentResolution = 2;
    selectedCameraIndex = 0;
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::applyConfigurationChanges(){
    if (degreesButtonValue == 0) {
        this->rotations = 0;
    }
    if (degreesButtonValue == 1) {
        this->rotations = 1;
    }
    else if (degreesButtonValue == 2) {
        this->rotations = 2;
    }
    else if (degreesButtonValue == 3) {
        this->rotations = 3;
    }
    
    if (this->rotations % 2 == 0) {
        this->imageWidth = this->cameraWidth;
        this->imageHeight = this->cameraHeight;
    }
    else {
        this->imageWidth = this->cameraHeight;
        this->imageHeight = this->cameraWidth;
    }
    
    this->selectResolution();
    this->setFullscreen();
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::setFullscreen(){
    if (fullscreen == true){
        ofSetFullscreen(true);
    } else {
        ofSetFullscreen(false);
        ofSetWindowShape(800, 600);
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::selectResolution(){
    this->deviceResolution[0] = "800x600";
    this->deviceResolution[1] = "1280x720";
    this->deviceResolution[2] = "1920x1080";
    
    if(currentResolution == 0){
        cameraWidth = 800;
        cameraHeight = 600;
    } else if (currentResolution == 1){
        cameraWidth = 1280;
        cameraHeight = 720;
    } else if (currentResolution == 2){
        cameraWidth = 1920;
        cameraHeight = 1080;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::changeLocale(){
    //    string.c_str()
    if (this->currentLocale == LOCALE_ENGLISH) {
        this->currentLocale = LOCALE_PORTUGUESE;
        this->currentStrings = this->ptStrings;
    }
    else {
        this->currentLocale = LOCALE_ENGLISH;
        this->currentStrings = this->enStrings;
    }
    
    if (this->currentLocale == LOCALE_ENGLISH) {
        this->changeLocaleLabel = PORTUGUESE_LABEL;
    }
    else {
        this->changeLocaleLabel = ENGLISH_LABEL;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::saveXmlSettings(){
    settings.clear();
    settings.addChild("MAIN");
    settings.setTo("MAIN");
    settings.addValue("MAX_SPEED", ofToString(maxSpeed));
    settings.addValue("MAX_HEIGHT", ofToString(maxHeight));
    settings.addValue("INTERVAL_TO_SAVE", ofToString(intervalToSaveImage));
    settings.addValue("DEGREES_BUTTON_VAL", ofToString(degreesButtonValue));
    settings.addValue("CURRENT_RESOLUTION", ofToString(currentResolution));
    settings.addValue("SAVE_IMAGE_TOGGLE", ofToString(saveImageToggle));
    settings.addValue("SHOW_AT_STARTUP", ofToString(showAtStartup));
    settings.addValue("FULLSCREEN", ofToString(fullscreen));
    settings.addValue("SHOW_CONFIGURATION_PANEL", ofToString(configurationPanelShow));
    settings.save("settings.xml");
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::loadXmlSettings(){
    this->settings.load("settings.xml");
    
    if(settings.exists("//MAX_SPEED")){
        this->maxSpeed = settings.getValue<int>("//MAX_SPEED");
    } else {
        maxSpeed = 16;
    }
    
    if(settings.exists("//MAX_HEIGHT")){
        this->maxHeight = settings.getValue<int>("//MAX_HEIGHT");
    } else {
        maxHeight = 48;
    }
    
    if(settings.exists("//INTERVAL_TO_SAVE")){
        this->intervalToSaveImage = settings.getValue<int>("//INTERVAL_TO_SAVE");
    } else {
        intervalToSaveImage = 15;
    }
    
    if(settings.exists("//DEGREES_BUTTON_VAL")){
        this->degreesButtonValue = settings.getValue<int>("//DEGREES_BUTTON_VAL");
    } else {
        degreesButtonValue = 0;
    }
    
    if(settings.exists("//CURRENT_RESOLUTION")){
        this->currentResolution = settings.getValue<int>("//CURRENT_RESOLUTION");
    } else {
        this->currentResolution = 2;
    }
    
    if(settings.exists("//SAVE_IMAGE_TOGGLE")){
        this->saveImageToggle = settings.getValue<bool>("//SAVE_IMAGE_TOGGLE");
    } else {
        this->saveImageToggle = false;
    }
    
    if(settings.exists("//SHOW_AT_STARTUP")){
        this->showAtStartup = settings.getValue<bool>("//SHOW_AT_STARTUP");
    } else {
        this->showAtStartup = true;
    }
    
    if(settings.exists("//FULLSCREEN")){
        this->fullscreen = settings.getValue<bool>("//FULLSCREEN");
    } else {
        this->fullscreen = true;
    }
    
    if(settings.exists("//SHOW_CONFIGURATION_PANEL")){
        this->configurationPanelShow = settings.getValue<bool>("//SHOW_CONFIGURATION_PANEL");
    } else {
        this->configurationPanelShow = true;
    }
}
//----------------------------------------------------------------------------------------------------------------------------
void ofApp::mouseReleased(ofMouseEventArgs&){
    if (this->configurationPanelShow == false) {
        this->showConfigurationPanel();
    }
}
