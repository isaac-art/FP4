//
// ISAAC CLARKE
// LANDSCAPE MAKER
//
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
    void keyPressed  (int key);
	void mousePressed(int x, int y, int button);
    void drawCVThings();
    void initMesh(int w, int h);
    void drawMeshThings();
    void exportModel();
    void exportImage();
    void switchExtrudeMode(int m);
    
    void toggleHueMode();
    void toggleSaturationMode();
    void toggleBrightnessMode();
    void toggleLightnessMode();
    
    
    int extMode;
    ofImage testImage;
	ofVideoGrabber cam;
    ofxPanel gui;
    ofParameter<float> pseGain;
    ofParameter<float> pseExposure;
    ofParameter<float> pseSharpness;
    ofParameter<float> pseContrast;
    ofParameter<float> pseBrightness;
    ofParameter<float> pseHue;
    ofParameter<bool> pseTestPattern;
    ofxButton exportImageBtn;
    
    ofxPanel guiTwo;
    ofxButton sHue;
    ofxButton sSaturation;
    ofxButton sBrightness;
    ofxButton sLightness;
    ofParameter<float> sRange;
    
    ofxPanel guiThree;
    ofParameter<bool> drawWireframe;
    ofParameter<float> rotX;
    ofParameter<float> rotY;
    ofParameter<float> rotZ;
    ofParameter<bool> spin;
    ofParameter<float> distance;
    ofParameter<float> colorDistThresh;
    ofxButton exportBtn;
    
    ofFbo cvFBO;
    ofFbo modelFBO;
    ofEasyCam meshCam;
    ofMesh meshyMcMeshface;
    ofImage ss;
    int maxBufferSize;
    deque<ofImage> imgBuffer;
   
};
