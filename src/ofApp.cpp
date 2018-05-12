//
// ISAAC CLARKE
// LANDSCAPE MAKER
//

#include "ofApp.h"
#include "ofxPS3EyeGrabber.h"


void ofApp::setup() {

    ofSetVerticalSync(true);
    //testImage.load("s.jpg");
    //SETUP THE PS3EYE CAM
    cam.setGrabber(std::make_shared<ofxPS3EyeGrabber>());
    cam.setDeviceID(0);
    cam.setDesiredFrameRate(60);
    cam.setup(320, 240);
    //SWITCH OFF AUTO UPDATES
    cam.getGrabber<ofxPS3EyeGrabber>()->setAutogain(false);
    cam.getGrabber<ofxPS3EyeGrabber>()->setAutoWhiteBalance(false);
    //PREP BUFFERS FOR DRAWING TO
    cvFBO.allocate(640, 480, GL_RGBA);
    modelFBO.allocate(640, 480, GL_RGBA);
    meshCam.disableMouseInput();
    //MAKE THE RECT MESH
    initMesh(320,240);
    //ADD CONTROLS FOR CAMERA SETTINGS
    gui.setup();
    gui.add(pseGain.set("Gain", cam.getGrabber<ofxPS3EyeGrabber>()->getGain(), 0, 63));
    gui.add(pseExposure.set("Exposure", cam.getGrabber<ofxPS3EyeGrabber>()->getExposure(), 0, 255));
    gui.add(pseSharpness.set("Sharpeness", 8, 0, 63));
    gui.add(pseContrast.set("Contrast", cam.getGrabber<ofxPS3EyeGrabber>()->getContrast(), 0, 255));
    gui.add(pseBrightness.set("Brightness", cam.getGrabber<ofxPS3EyeGrabber>()->getBrightness(), 0, 255));
    gui.add(pseHue.set("Hue", cam.getGrabber<ofxPS3EyeGrabber>()->getHue(), 0, 255));
    gui.add(pseTestPattern.set("Test Pattern", false));
    exportImageBtn.addListener(this, &ofApp::exportImage);
    gui.add(exportImageBtn.setup("Export Image"));
    gui.setPosition(645,485);
    gui.setName("PSEyeCam Settings");
    //ADD CONTROLS FOR MESH SETTINGS
    extMode = 2;
    guiTwo.setup();
    guiTwo.add(drawWireframe.set("Draw Wireframe", false));
    sHue.addListener(this, &ofApp::toggleHueMode);
    guiTwo.add(sHue.setup("Extrude Hue"));
    sSaturation.addListener(this, &ofApp::toggleSaturationMode);
    guiTwo.add(sSaturation.setup("Extrude Saturation"));
    sBrightness.addListener(this, &ofApp::toggleBrightnessMode);
    guiTwo.add(sBrightness.setup("Extrude Brightness"));
    sLightness.addListener(this, &ofApp::toggleLightnessMode);
    guiTwo.add(sLightness.setup("Extrude Lightness"));
    guiTwo.add(sRange.set("Depth", -75, -500, 500));
    exportBtn.addListener(this, &ofApp::exportModel);
    guiTwo.add(exportBtn.setup("Export Model"));
    guiTwo.setPosition(5,485);
    guiTwo.setName("Mesh Settings");
    //ADD CONTROLS FOR MESH CAMERA SETTINGS
    guiThree.setup();
    guiThree.add(rotX.set("Rotation X", 79, 0, 360));
    guiThree.add(rotY.set("Rotation Y", 5, 0, 360));
    guiThree.add(rotZ.set("Rotation Z", 38, 0, 360));
    guiThree.add(spin.set("Spin", false));
    guiThree.add(distance.set("Distance", 615, 0, 3000));
    guiThree.setPosition(225,485);
    guiThree.setName("Mesh Camera Settings");
    //SET THE MESH CAMERA
    meshCam.setDistance(distance);
    //BUFFER SIZE FOR IMAGE AVERAGING
    maxBufferSize = 15;
    
}

void ofApp::update() {
    //UPDATE THE MESH CAMERA
    meshCam.setDistance(distance);
    //UPDATE THE CAMERA WITH GUI SLIDERS
    cam.getGrabber<ofxPS3EyeGrabber>()->setTestPattern(pseTestPattern);
    cam.getGrabber<ofxPS3EyeGrabber>()->setGain(pseGain);
    cam.getGrabber<ofxPS3EyeGrabber>()->setExposure(pseExposure);
    cam.getGrabber<ofxPS3EyeGrabber>()->setSharpness(pseSharpness);
    cam.getGrabber<ofxPS3EyeGrabber>()->setContrast(pseContrast);
    cam.getGrabber<ofxPS3EyeGrabber>()->setBrightness(pseBrightness);
    cam.getGrabber<ofxPS3EyeGrabber>()->setHue(pseHue);
    //CHECK IF NEW FRAME FROM CAMERA
    cam.update();
    if(cam.isFrameNew()) {
        //BUFFER THE IMAGES COMING IN FROM THE CAMERA
        ofImage img;
        img.setFromPixels(cam.getPixels());
        imgBuffer.push_front(img);
        if(imgBuffer.size()>maxBufferSize) imgBuffer.pop_back();
        //FOR EVERY PIXEL
        for (int i=0; i<cam.getWidth()*cam.getHeight(); i++){
            float avgR;
            float avgG;
            float avgB;
            //ADD EACH PIXEL VALUE LOOPING OVER THE BUFFER
            for(int j=0;j<imgBuffer.size();j++){
                avgR += imgBuffer[j].getPixels()[i*3]/255.f;
                avgG += imgBuffer[j].getPixels()[i*3+1]/255.f;
                avgB += imgBuffer[j].getPixels()[i*3+2]/255.f;
            }
            //DIVIDE THAT BY BUFFER SIZE TO GET THE AVERAGE
            avgR = avgR/imgBuffer.size();
            avgG = avgG/imgBuffer.size();
            avgB = avgB/imgBuffer.size();
            //UPDATE THE COLOUR OF THE MESH AT THAT POSITION
            ofFloatColor sColour(avgR,avgG,avgB);
            ofVec3f tempV = meshyMcMeshface.getVertex(i);
            //CHECK GUI FOR THE MODE
            //THE MODE CHANGES THE Z POSITION FOR THE VERTS IN
            //THE MESH
            if(extMode == 0)tempV.z = sColour.getHue() * sRange;
            else if(extMode == 1)tempV.z = sColour.getSaturation() * sRange;
            else if(extMode == 2)tempV.z = sColour.getBrightness() * sRange;
            else if(extMode == 3)tempV.z = sColour.getLightness() * sRange;
            //SET THE VERT
            meshyMcMeshface.setVertex(i, tempV);
            //IF IT IS WIRE FRAME MODE THE COLOUR BLACK
            if(drawWireframe) meshyMcMeshface.setColor(i, ofColor(0,0,0));
            //OTHERWISE COLOUR IT FROM THE AVERAGE COLOUR
            else meshyMcMeshface.setColor(i, sColour);
        }
    }
   
}

void ofApp::draw() {
    //this is for the 3d camera
    ofDisableDepthTest();
    
    //draw the gui panels
    gui.draw();
    guiTwo.draw();
    guiThree.draw();
    
    //draw fps
    ofPushStyle();
    ofSetColor(0);
    string msg = "fps: " + ofToString(ofGetFrameRate(), 2);
    ofDrawBitmapString(msg, 5, 10);
    ofPopStyle();
    
    //draw the camera fbo
    drawCVThings();
    ofPushMatrix();
    ofScale(2, 2);
    cvFBO.draw(320,0);
    ofPopMatrix();
    //draw the mesh fbo
    drawMeshThings();
    modelFBO.draw(0,0);
    
}

void ofApp::drawMeshThings(){
    //begin drawing to fbo
    modelFBO.begin();
    //clear it first
    ofClear(255,255,255,0);
    //enable depth for 3d
    ofEnableDepthTest();
    //using the 3d camera
    meshCam.begin();
    //ofTranslate(0,-100,0);
    //if spin toggle is on
    if(spin){
        //rotate by the elapsed time
        ofRotateX(ofGetElapsedTimef()*rotX);
        ofRotateY(ofGetElapsedTimef()*rotY);
        ofRotateZ(ofGetElapsedTimef()*rotZ);
    }else {
        //otherwise just use the sliders
        ofRotateX(rotX);
        ofRotateY(rotY);
        ofRotateZ(rotZ);
    }
    ofSetColor(255);
    
    //draw the mesh either wireframe (black)
    //or colour (camera image)
    if(!drawWireframe) meshyMcMeshface.drawFaces();
    else meshyMcMeshface.drawWireframe();
    
    meshCam.end();
    modelFBO.end();
}

void ofApp::drawCVThings(){
    // THIS WAS DOING MORE THINGS BEFORE
    // BUT NOW JUST SHOWS THE CAMERA
    //STILL USEFUL TO HAVE IT DISPLAYED
    cvFBO.begin();
    //clear fbo on start
    ofClear(255,255,255, 0);
    //then draw to it
    ofSetColor(255);
    cam.draw(0, 0);
    ofSetLineWidth(2);
    ofNoFill();
    cvFBO.end();
}

void ofApp::initMesh(int w, int h){
    //
    //This is from the openframeworks 3d examples.
    //
    meshCam.setScale(1,-1,1);
   // add the points to match the camera size
    for(int x=0; x<h; x++){
        for(int y=0; y<w;y++){
            
            meshyMcMeshface.addVertex(ofPoint(x,y,0));
            meshyMcMeshface.addColor(ofFloatColor(0,0,0));
        }
    }
   // index the mesh so it draws correctly
    for (int y = 0; y<h-1; y++){
        for (int x=0; x<w-1; x++){
            meshyMcMeshface.addIndex(x+y*w);                // 0
            meshyMcMeshface.addIndex((x+1)+y*w);            // 1
            meshyMcMeshface.addIndex(x+(y+1)*w);            // 10
            meshyMcMeshface.addIndex((x+1)+y*w);            // 1
            meshyMcMeshface.addIndex((x+1)+(y+1)*w);        // 11
            meshyMcMeshface.addIndex(x+(y+1)*w);            // 10
        }
    }
}

void ofApp::exportModel(){
    //EXPORT A 3D MODEL
    //USES PLY AS IT CONTAINS THE COLOUR INFO TOO
    meshyMcMeshface.save("m-"+ofGetTimestampString()+".ply");
}

void ofApp::exportImage(){
    //Expor the current camera image
    //this is useful as you can print it out and it
    //works like saving where you just drop in the printed
    //image and carry on from where you left off
    ofSaveImage(cam.getPixels(), "cam-"+ofGetTimestampString()+".jpg");
}

void ofApp::mousePressed(int x, int y, int button) {

}

void ofApp::keyPressed(int key){
    //KEY CONTROLS FOR SETUP
    switch(key) {
        case 'f':
        case 'F':
            ofToggleFullscreen();
            break;
        case 'c':
        case 'C':
            if(meshCam.getMouseInputEnabled()) meshCam.disableMouseInput();
            else meshCam.enableMouseInput();
            break;
        case 's':
        case 'S':
            ss.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
            ss.save("s"+ofGetTimestampString()+".png");
            break;
    }
}

//THESE ARE TOGGLED BY THE GUI
void ofApp::toggleHueMode(){
    extMode = 0;
}
void ofApp::toggleSaturationMode(){
    extMode = 1;
}
void ofApp::toggleBrightnessMode(){
    extMode = 2;
}
void ofApp::toggleLightnessMode(){
    extMode = 3;
}
