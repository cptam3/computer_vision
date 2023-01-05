#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp{

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
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        void ImageSelectionChange(int & num);
        void save();
        void setBrightness();
        void setContast();
        void colorMask();
        void gaussianBlur();
        void medianBlur();
        void reset();
        void removeBackground();
        void _flare();
        void _edge();
        void _toony();
    
    //some path, may be absolute or relative to bin/data
    string path = "";
    ofDirectory dir;
    
    ofImage SelectedImg;
    Mat imgMat;
    Mat modifiedMat;
    ofxPanel gui_img;
    ofxLabel instruction;
    vector<ofxLabel> choices;
    ofParameter<int> ImageSelection;
    
    
    ofxPanel gui_tools;
    
    ofxButton Reset;
    
    ofxButton Brightness;
    ofxIntSlider brightness_value;
    
    ofxButton Contast;
    ofxIntSlider contast_value;
    
    ofxButton ColorMask;
    ofxColorSlider colorMask_color;
    ofxFloatSlider colorMask_value;
    
    ofxButton _GaussianBlur;
    ofxIntSlider gaussianBlur_value;
    
    ofxButton _MedianBlur;
    ofxIntSlider medianBlur_value;
    
    ofxButton BackgroundRemoval;
    ofxIntSlider removal_choice;
    
    ofxButton Flare;
    ofxIntSlider flare_value;
    
    ofxToggle Crop;
    bool IsCreatingCropArea = false;
    ofVec2f Startposition;
    ofVec2f Endposition;
    
    ofxButton Sketch_effect;
    ofxIntSlider Sketch_effect_value;
    
    ofxButton Toony_effect;
    ofxIntSlider Toony_effect_value;
    
    int output_count;
    ofxButton _Save;
};
