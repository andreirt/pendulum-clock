#include "ofApp.h"

const string ofApp::ENGLISH_LABEL = "English";
const string ofApp::PORTUGUESE_LABEL = "Portugues";

const float ofApp::MAX_STRENGTH_AROUND_PIXEL = .15;
const string ofApp::SUPPORT_BUTTON_NAME = "support";
const string ofApp::CHANGE_LOCALE_BUTTON_NAME = "changeLocale";

//--------------------------------------------------------------
void ofApp::setup(){
    this->selectedCameraIndex = 0;
    this->step = 0;
    this->maxSpeed = 16;
    this->maxHeight = 48;
    this->cameraWidth = 1280;
    this->cameraHeight = 720;
    this->lastTimeImageWasSaved = 0;
    this->intervalToSaveImage = 15;
    
    // reads strings.xml
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
    
    this->currentStrings = this->ptStrings;

	this->gui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());
    this->gui->setWidgetSpacing(10);
    gui->setFontSize(OFX_UI_FONT_SMALL, 8);
    gui->setFontSize(OFX_UI_FONT_LARGE, 18);

    ofxUIColor backgroundColor = ofxUIColor::white;
    ofxUIColor fillColor = ofxUIColor::black;
    ofxUIColor fillHightlightColor = ofxUIColor::black;
    ofxUIColor outline = ofxUIColor::black;
    ofxUIColor outlineHighlight = ofxUIColor::red;
    ofxUIColor paddedColor = ofxUIColor::blue;
    ofxUIColor paddedOutlineColor = ofxUIColor::orange;

    this->gui->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );

    this->titleLabel = this->gui->addLabel("title", this->currentStrings["pendulumclock"], OFX_UI_FONT_LARGE);
    this->changeLocaleButton = new ofxUILabelButton(ofApp::CHANGE_LOCALE_BUTTON_NAME, true, 200, 30, 500, 10);
    this->gui->addWidgetRight(this->changeLocaleButton);
    this->changeLocaleButton->getLabelWidget()->setLabel(ENGLISH_LABEL);
    this->gui->addSpacer();

    this->cameraPanel = new ofxUICanvas(0, 0, 400, 235);
    this->cameraPanel->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );
    this->cameraPanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->cameraPanel->setWidgetSpacing(10);
    this->gui->addWidgetDown(this->cameraPanel);

    // lista as câmeras conectadas a este computador
    this->videoGrabber = new ofVideoGrabber();
    vector< ofVideoDevice > devices = this->videoGrabber->listDevices();

    vector<string> *cameras = new vector<string>();
    vector<ofVideoDevice>::iterator it;

    for (it = devices.begin(); it != devices.end(); ++it) {
        ofVideoDevice device = *it;
        cameras->push_back(device.deviceName);
    }

    this->pickCameraLabel = this->cameraList = this->cameraPanel->addDropDownList(this->currentStrings["pickCamera"], *cameras, 300, 10);
    this->cameraList->setAllowMultiple(false);
    this->cameraList->setDrawOutline(true);
    this->cameraList->setAutoClose(true);

    vector<ofxUILabelToggle *> toggles = this->cameraList->getToggles();
    vector<ofxUILabelToggle *>::iterator togglesIterator;

    for (togglesIterator = toggles.begin(); togglesIterator != toggles.end(); ++togglesIterator) {
        ofxUILabelToggle* labelToggle = *togglesIterator;
        labelToggle->setColorFillHighlight(ofxUIColor::black);
        labelToggle->setColorFill(ofxUIColor::white);
        labelToggle->setColorOutlineHighlight(ofxUIColor::black);
        labelToggle->setColorBack(ofxUIColor::white);
    }

    this->cameraWidthLabel = new ofxUILabel(180, this->currentStrings["cameraWidth"], OFX_UI_FONT_SMALL);
    this->cameraPanel->addWidgetDown( cameraWidthLabel );
    this->cameraWidthTextInput = new ofxUITextInput("CameraWidth", "1920", 80, 18) ;
    this->cameraWidthTextInput->setOnlyNumericInput(true);
    this->cameraWidthTextInput->setDrawOutline(true);
    this->cameraWidthTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( cameraWidthTextInput );
    this->textInputs.push_back(this->cameraWidthTextInput);

    this->cameraHeightLabel = new ofxUILabel(170, this->currentStrings["cameraHeight"], OFX_UI_FONT_SMALL);
    this->cameraPanel->addWidgetDown( cameraHeightLabel );
    this->cameraHeightTextInput = new ofxUITextInput("CameraHeight", "1080", 80, 18);
    this->cameraHeightTextInput->setOnlyNumericInput(true);
    this->cameraHeightTextInput->setDrawOutline(true);
    this->cameraHeightTextInput->setDrawOutlineHighLight(true);
    this->cameraPanel->addWidgetRight( this->cameraHeightTextInput );
    this->textInputs.push_back(this->cameraHeightTextInput);

    this->rotations = 0;

    this->imageRotationLabel = this->cameraPanel->addLabel(this->currentStrings["imageRotation"], OFX_UI_FONT_SMALL);

    this->zeroRotationToggle = new ofxUIToggle(this->currentStrings["zeroDegress"], true, 16, 16);
    this->zeroRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->zeroRotationToggle);

    this->ninetyRotationToggle = new ofxUIToggle(this->currentStrings["ninetyDegress"], true, 16, 16);
    this->ninetyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->ninetyRotationToggle);

    this->oneHundredEightyRotationToggle = new ofxUIToggle(this->currentStrings["oneHundredEightyDegress"], true, 16, 16);
    this->oneHundredEightyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->oneHundredEightyRotationToggle);

    this->twoHundredSeventyRotationToggle = new ofxUIToggle(this->currentStrings["twoHundredSeventyDegress"], true, 16, 16);
    this->twoHundredSeventyRotationToggle->setDrawOutline(true);
    this->cameraPanel->addWidgetDown(this->twoHundredSeventyRotationToggle);

    this->imagePanel = new ofxUICanvas(190, 0, 400, 235);
    this->imagePanel->setUIColors( backgroundColor, outline, outlineHighlight, fillColor, fillHightlightColor, paddedColor, paddedOutlineColor );
    this->imagePanel->setFontSize(OFX_UI_FONT_SMALL, 8);
    this->imagePanel->setWidgetSpacing(10);

    this->maxSpeedLabel = new ofxUILabel(230, this->currentStrings["maxSpeed"], OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetDown( maxSpeedLabel );

    this->maxSpeedTextInput =new ofxUITextInput("maxSpeed", "30", 80, 18);
    this->maxSpeedTextInput->setOnlyNumericInput(true);
    this->maxSpeedTextInput->setDrawOutline(true);
    this->imagePanel->addWidgetRight( maxSpeedTextInput );

    this->maxHeightLabel = new ofxUILabel(230, this->currentStrings["maxHeight"], OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetDown( maxHeightLabel );

    this->maxHeightTextInput =new ofxUITextInput("maxHeight", "30", 80, 18);
    this->maxHeightTextInput->setOnlyNumericInput(true);
    this->maxHeightTextInput->setDrawOutline(true);
    this->imagePanel->addWidgetRight( maxHeightTextInput );

    this->imagePanel->addSpacer();

    this->saveImageToggle = new ofxUIToggle(this->currentStrings["saveImage"], true, 16, 16);
    this->saveImageToggle->setDrawOutline(true);
    this->imagePanel->addWidgetDown(this->saveImageToggle);

    this->intervalToSaveImage = 15;

    this->intervalToSaveTextInput = new ofxUITextInput("A cada", "15", 60, 18);
    this->intervalToSaveTextInput->setOnlyNumericInput(true);
    this->intervalToSaveTextInput->setDrawOutline(true);
    this->intervalToSaveTextInput->setDrawOutlineHighLight(true);
    this->imagePanel->addWidgetRight( this->intervalToSaveTextInput );
    this->textInputs.push_back(this->intervalToSaveTextInput);

    this->minutesLabel = minutesLabel = new ofxUILabel(90, this->currentStrings["minutes"], OFX_UI_FONT_SMALL);
    this->imagePanel->addWidgetRight( minutesLabel );

    this->clearButton = this->gui->addLabelButton(this->currentStrings["resetImage"], false, 150, 20);
    clearButton->setDrawFill(true);
    clearButton->setDrawOutline(true);
    this->imagePanel->addWidgetDown( clearButton );

    this->imagePanel->addSpacer();

    this->showAtStartupToggle = this->imagePanel->addToggle(this->currentStrings["showAtStartup"], true, 16, 16);
    this->showAtStartupToggle->setDrawOutline(true);
    this->imagePanel->addSpacer();

    this->fullScreenToggle = this->imagePanel->addToggle(this->currentStrings["fullScreen"], true, 16, 16);
    this->fullScreenToggle->setDrawOutline(true);

    this->gui->addWidgetRight(this->imagePanel);

    this->gui->addSpacer();

    this->saveButton = this->gui->addLabelButton(this->currentStrings["save"], false, 100, 20);
    saveButton->setDrawFill(true);
    saveButton->setDrawOutline(true);

    this->cancelButton = new ofxUILabelButton(this->currentStrings["cancel"], false, 100, 20);
    cancelButton->setDrawFill(true);
    cancelButton->setDrawOutline(true);
    this->gui->addWidgetRight(cancelButton);
    this->gui->addSpacer();

    this->credits1Label = this->gui->addLabel(this->currentStrings["credits1"], OFX_UI_FONT_SMALL);
    this->credits2Label = this->gui->addLabel(this->currentStrings["credits2"], OFX_UI_FONT_SMALL);
    this->credits3Label = this->gui->addLabel(this->currentStrings["credits3"], OFX_UI_FONT_SMALL);
    this->credits4Label = this->gui->addLabel(this->currentStrings["credits4"], OFX_UI_FONT_SMALL);
    this->credits5Label = this->gui->addLabel(this->currentStrings["credits5"], OFX_UI_FONT_SMALL);
    this->gui->addSpacer();

    this->supportLabel = this->gui->addLabel(this->currentStrings["support"]);
    this->gui->addImageButton(this->currentStrings["support"], "funarte.png", false, 509, 60);

    ofAddListener(this->gui->newGUIEvent, this, &ofApp::guiEvent);
    ofAddListener(this->cameraPanel->newGUIEvent, this, &ofApp::cameraPanelEvent);
    ofAddListener(this->imagePanel->newGUIEvent, this, &ofApp::imagePanelEvent);

    this->cameraPanel->loadSettings("camera.xml");
    this->imagePanel->loadSettings("image.xml");
    this->gui->loadSettings("settings.xml");

    // reads values from controls and stores them into properties
    this->applyConfigurationChanges();

    if (this->showAtStartup) {
        this->showConfigurationPanel();
    }
    else {
        this->hideConfigurationPanel();
    }

    this->reset();
}

void ofApp::reset()
{
    if (this->videoGrabber != NULL) {
        if (this->videoGrabber->isInitialized()) {
            this->videoGrabber->close();
            delete this->videoGrabber;
            this->videoGrabber = new ofVideoGrabber();
        }
    }

    // set camera
    vector<int> selectedCamera = this->cameraList->getSelectedIndeces();
    if (selectedCamera.size() > 0) {
        this->videoGrabber->setDeviceID( selectedCamera[0] );
    }
    else {
        this->videoGrabber->setDeviceID( 0 );
    }

    this->videoGrabber->setDesiredFrameRate(30);
    this->videoGrabber->initGrabber(this->cameraWidth, this->cameraHeight);

    // image to be drawn
    this->screenImage.allocate(this->imageWidth, this->imageHeight, OF_IMAGE_COLOR_ALPHA);
    this->fillImageWithWhite( &this->screenImage );

    // check if we have a image to load
    ofImage previousImage;
    if (previousImage.loadImage("pendulum_clock.png")) {

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


//--------------------------------------------------------------
void ofApp::update(){

    this->videoGrabber->update();

    if (this->gui->isVisible()) {
        return;
    }

    if (this->videoGrabber->isFrameNew()) {
        ofPixels pixels = this->videoGrabber->getPixelsRef();
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


        if (ofGetElapsedTimef() - this->lastTimeImageWasSaved > this->intervalToSaveImage * 60) {
            // image we will try to load on start up
            this->screenImage.saveImage("pendulum_clock.png");

            // image we will save to have a recording of work
            this->saveCurrentImage();

            this->lastTimeImageWasSaved = ofGetElapsedTimef();
        }

    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    // We scale the image so it covers all the screen.
    // The image is croppred if necessary.
    float hScale = (float) ofGetWidth() / (float) this->screenImage.width;
    float vScale = (float) ofGetHeight() / (float) this->screenImage.height;

    float scale = max(hScale, vScale);

    int scaledWidth = (int) roundf((float) this->screenImage.width * scale);
    int scaledHeight = (int) roundf((float) this->screenImage.height * scale);

    int x = (ofGetWidth() - scaledWidth) / 2;
    int y = (ofGetHeight() - scaledHeight) / 2;

    this->screenImage.draw(x, y, scaledWidth, scaledHeight);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if (key == OF_KEY_TAB) {

        if (this->gui->isVisible()) {
            this->hideConfigurationPanel();
        }
        else {
            this->showConfigurationPanel();
        }

    }

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

//--------------------------------------------------------------
// ofxUI
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    if (e.getName() == this->currentStrings["save"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {

            this->cameraPanel->saveSettings("camera.xml");
            this->imagePanel->saveSettings("image.xml");
            this->applyConfigurationChanges();
            this->reset();
            this->hideConfigurationPanel();

        }
    }
    else if (e.getName() == this->currentStrings["cancel"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {

            this->cancelConfigurationChanges();
            this->hideConfigurationPanel();

        }
    }
    else if (e.getName() == this->currentStrings["support"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            ofLaunchBrowser("http://www.funarte.gov.br/");
        }
    }
    else if (e.getName() == ofApp::CHANGE_LOCALE_BUTTON_NAME) {
        // catches the click when mouse is released, not pressed
        if(!e.getButton()->getValue()) {
            if (this->currentLocale == LOCALE_ENGLISH) {
                this->currentLocale = LOCALE_PORTUGUESE;
                this->currentStrings = this->ptStrings;
            }
            else {
                this->currentLocale = LOCALE_ENGLISH;
                this->currentStrings = this->enStrings;
            }
            if (this->currentLocale == LOCALE_ENGLISH) {
                this->changeLocaleButton->getLabelWidget()->setLabel(PORTUGUESE_LABEL);
            }
            else {
                this->changeLocaleButton->getLabelWidget()->setLabel(ENGLISH_LABEL);
            }
            
            this->titleLabel->setLabel( this->currentStrings["pendulumclock"] );
            
            this->pickCameraLabel->setLabelText( this->currentStrings["pickCamera"] );
            
            this->cameraWidthLabel->setLabel( this->currentStrings["cameraWidth"] );
            this->cameraHeightLabel->setLabel( this->currentStrings["cameraHeight"] );
            
            this->imageRotationLabel->setLabel( this->currentStrings["imageRotation"] );
            
            this->zeroRotationToggle->getLabelWidget()->setLabel( this->currentStrings["zeroDegress"] );
            this->ninetyRotationToggle->getLabelWidget()->setLabel( this->currentStrings["ninetyDegress"] );
            this->oneHundredEightyRotationToggle->getLabelWidget()->setLabel( this->currentStrings["oneHundredEightyDegress"] );
            this->twoHundredSeventyRotationToggle->getLabelWidget()->setLabel( this->currentStrings["twoHundredSeventyDegress"] );
            
            this->maxSpeedLabel->setLabel( this->currentStrings["maxSpeed"] );
            this->maxHeightLabel->setLabel( this->currentStrings["maxHeight"] );
            
            this->saveImageToggle->getLabelWidget()->setLabel( this->currentStrings["saveImage"] );
            
            this->minutesLabel->setLabel( this->currentStrings["minutes"] );
            
            this->clearButton->setLabelText( this->currentStrings["resetImage"] );
            
            this->showAtStartupToggle->getLabelWidget()->setLabel( this->currentStrings["showAtStartup"] );
            
            this->fullScreenToggle->getLabelWidget()->setLabel( this->currentStrings["fullScreen"] );
            
            this->saveButton->setLabelText( this->currentStrings["save"] );
            this->cancelButton->setLabelText( this->currentStrings["cancel"] );
            
            this->credits1Label->setLabel( this->currentStrings["credits1"] );
            this->credits2Label->setLabel( this->currentStrings["credits2"] );
            this->credits3Label->setLabel( this->currentStrings["credits3"] );
            this->credits4Label->setLabel( this->currentStrings["credits4"] );
            this->credits5Label->setLabel( this->currentStrings["credits5"] );
            
            this->supportLabel->setLabel( this->currentStrings["support"] );
            
        }
    }
}

void ofApp::cameraPanelEvent(ofxUIEventArgs &e)
{
    if (e.getName() == this->currentStrings["zeroDegress"] && e.getToggle()->getValue()) {
        this->ninetyRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == this->currentStrings["ninetyDegress"] && e.getToggle()->getValue()) {
        this->zeroRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == this->currentStrings["oneHundredEightyDegress"] && e.getToggle()->getValue()) {
        this->zeroRotationToggle->setValue(false);
        this->ninetyRotationToggle->setValue(false);
        this->twoHundredSeventyRotationToggle->setValue(false);
    }
    else if (e.getName() == this->currentStrings["twoHundredSeventyDegress"] && e.getToggle()->getValue()) {
        this->zeroRotationToggle->setValue(false);
        this->ninetyRotationToggle->setValue(false);
        this->oneHundredEightyRotationToggle->setValue(false);
    }
    else {
        this->checkTextInputFocus(e);
    }
}

void ofApp::imagePanelEvent(ofxUIEventArgs &e)
{
    if (e.getName() == this->currentStrings["resetImage"]) {
        // catches the click when mouse is released, not pressed
        if (!e.getButton()->getValue()) {
            this->fillImageWithWhite( &this->screenImage );
            this->screenImage.saveImage("pendulum_clock.png");
        }
    }
    else {
        this->checkTextInputFocus(e);
    }
}

void ofApp::checkTextInputFocus(ofxUIEventArgs &e)
{
    if (e.getKind() == OFX_UI_WIDGET_TEXTINPUT){
        ofxUITextInput *ti = (ofxUITextInput *) e.widget;
        if (ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS){
            this->unfocusAllTextInputs(ti);
        }
    }
}

void ofApp::fillImageWithWhite( ofImage* image )
{
    // paints with white
    unsigned char* pixels = image->getPixels();
    int bytesPerPixel = 3;
    if (image->type == OF_IMAGE_COLOR_ALPHA)
        bytesPerPixel = 4;
    else if (image->type == OF_IMAGE_GRAYSCALE)
        bytesPerPixel = 1;
    for (int i =0; i < image->getWidth() * image->getHeight() * bytesPerPixel; i++) {
        pixels[i] = 255;
    }
}

void ofApp::saveCurrentImage()
{
    char filename[64];
    sprintf( filename, "pendulum_clock_%04d_%02d_%02d_%02d_%02d_%02d.png", ofGetYear(), ofGetMonth(), ofGetDay(), ofGetHours(), ofGetMinutes(), ofGetSeconds() );

    this->screenImage.saveImage(filename);
}

void ofApp::hideConfigurationPanel()
{
    this->gui->setVisible(false);
    this->cameraPanel->setVisible(false);
    this->imagePanel->setVisible(false);
    ofHideCursor();
}

void ofApp::showConfigurationPanel()
{
    this->gui->setVisible(true);
    this->gui->disableKeyEventCallbacks();

    this->cameraPanel->setVisible(true);
    this->imagePanel->setVisible(true);
    ofShowCursor();
}

void ofApp::cancelConfigurationChanges()
{
    this->zeroRotationToggle->setValue(this->rotations == 0);
    this->ninetyRotationToggle->setValue(this->rotations == 1);
    this->oneHundredEightyRotationToggle->setValue(this->rotations == 2);
    this->twoHundredSeventyRotationToggle->setValue(this->rotations == 3);

    this->cameraWidthTextInput->setTextString( ofToString(this->cameraWidth) );
    this->cameraHeightTextInput->setTextString( ofToString(this->cameraHeight) );

    this->maxSpeedTextInput->setTextString( ofToString(this->maxSpeed) );
    this->maxHeightTextInput->setTextString( ofToString(this->maxHeight) );
    this->intervalToSaveTextInput->setTextString( ofToString(this->intervalToSaveImage) );
    this->showAtStartupToggle->setValue( this->showAtStartup );
    this->fullScreenToggle->setValue( this->fullScreen );
    this->saveImageToggle->setValue( this->saveImage );
}

void ofApp::applyConfigurationChanges()
{
    if (this->zeroRotationToggle->getValue()) {
        this->rotations = 0;
    }
    if (this->ninetyRotationToggle->getValue()) {
        this->rotations = 1;
    }
    else if (this->oneHundredEightyRotationToggle->getValue()) {
        this->rotations = 2;
    }
    else if (this->twoHundredSeventyRotationToggle->getValue()) {
        this->rotations = 3;
    }

    this->zeroRotationToggle->setValue(this->rotations == 0);
    this->ninetyRotationToggle->setValue(this->rotations == 1);
    this->oneHundredEightyRotationToggle->setValue(this->rotations == 2);
    this->twoHundredSeventyRotationToggle->setValue(this->rotations == 3);

    this->cameraWidth = this->cameraWidthTextInput->getIntValue();
    this->cameraHeight = this->cameraHeightTextInput->getIntValue();

    if (this->rotations % 2 == 0) {
        this->imageWidth = this->cameraWidth;
        this->imageHeight = this->cameraHeight;
    }
    else {
        this->imageWidth = this->cameraHeight;
        this->imageHeight = this->cameraWidth;
    }

    this->maxSpeed = this->maxSpeedTextInput->getIntValue();
    this->maxHeight = this->maxHeightTextInput->getIntValue();
    this->intervalToSaveImage = this->intervalToSaveTextInput->getIntValue();

    this->showAtStartup = this->showAtStartupToggle->getValue();
    this->fullScreen = this->fullScreenToggle->getValue();
    this->saveImage = this->saveImageToggle->getValue();

    if (this->fullScreen) {
        ofSetFullscreen(true);
    }
    else {
        ofSetFullscreen(false);
        ofSetWindowShape(this->imageWidth, this->imageHeight);
    }
}

void ofApp::unfocusAllTextInputs(ofxUITextInput* except){
    for (int i = 0; i < this->textInputs.size(); i ++){
        if (except != this->textInputs[i]){
            this->textInputs[i]->setFocus(false);
        }
    }
}

//--------------------------------------------------------------
// ofxUI
void ofApp::exit()
{
    //delete gui;
}
