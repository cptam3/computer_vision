#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    //show png,jpg files
    dir.open(path);
    dir.allowExt("jpg");
    dir.allowExt("png");
    //populate the directory object
    dir.listDir();
    
    //print all the image choices
    gui_img.setup();
    gui_img.add(instruction.setup("Select one image",""));
    int numOfChoice = dir.size();
    for(int i=0;i<numOfChoice;i++){
        ofxLabel temp;
        choices.push_back(temp);    //create the dynamic size of vector
    }
    for(int i=0;i<numOfChoice;i++){
        gui_img.add(choices[i].setup("Image "+ ofToString(i) + ": "+ dir.getPath(i)));      //get all the img path
    }
    
    if(numOfChoice>0){
        ImageSelection.addListener(this, &ofApp::ImageSelectionChange);     //add listener and load the selected image
        gui_img.add(ImageSelection.set("Image number", 0, 0, numOfChoice-1));
        SelectedImg.load(dir.getPath(0));
        SelectedImg.setImageType(OF_IMAGE_COLOR);
        imgMat = toCv(SelectedImg);
        
        output_count = 0;
        _Save.addListener(this, &ofApp::save);
        gui_img.add(_Save.setup("Save"));
        
        gui_tools.setup();
        
        Reset.addListener(this, &ofApp::reset);
        gui_tools.add(Reset.setup("Reset"));
        
        Brightness.addListener(this, &ofApp::setBrightness);
        gui_tools.add(Brightness.setup("Brightness"));
        gui_tools.add(brightness_value.setup("Strength", 0, -255, 255));
        
        Contast.addListener(this, &ofApp::setContast);
        gui_tools.add(Contast.setup("Contast"));
        gui_tools.add(contast_value.setup("Strength",0,-255,255));
        
        ColorMask.addListener(this, &ofApp::colorMask);
        gui_tools.add(ColorMask.setup("Color Mask"));
        gui_tools.add(colorMask_color.setup(ofColor(255,255,255)));
        gui_tools.add(colorMask_value.setup("Strength", 0, 0, 1));
        
        gui_tools.add(Crop.setup("Crop",false));
        
        _GaussianBlur.addListener(this, &ofApp::gaussianBlur);
        gui_tools.add(_GaussianBlur.setup("Gauusain Blur"));
        gui_tools.add(gaussianBlur_value.setup("Strength", 1, 1, 8));
        
        _MedianBlur.addListener(this, &ofApp::medianBlur);
        gui_tools.add(_MedianBlur.setup("Median Blur"));
        gui_tools.add(medianBlur_value.setup("Strength", 2, 2, 15));
        
        BackgroundRemoval.addListener(this, &ofApp::removeBackground);
        gui_tools.add(BackgroundRemoval.setup("Remove Background"));
        gui_tools.add(removal_choice.setup("Choice",0,0,3));
        
        Flare.addListener(this, &ofApp::_flare);
        gui_tools.add(Flare.setup("Flare"));
        gui_tools.add(flare_value.setup("Strength",1,1,10));
        
        Sketch_effect.addListener(this, &ofApp::_edge);
        gui_tools.add(Sketch_effect.setup("Sketch Effect"));
        gui_tools.add(Sketch_effect_value.setup("value", 10, 30, 100));
        
        Toony_effect.addListener(this, &ofApp::_toony);
        gui_tools.add(Toony_effect.setup("Toony Effect"));
        gui_tools.add(Toony_effect_value.setup("Number of Colors",2,2,10));
    }
}

void ofApp::ImageSelectionChange(int &num){     //select image using number slider
    SelectedImg.load(dir.getPath(num));
    SelectedImg.setImageType(OF_IMAGE_COLOR);
    imgMat = toCv(SelectedImg);
}

void ofApp::save(){     //save image
    output_count++;
    ofImage output;
    toOf(imgMat,output);    //mat to ofimage
    
    string filname = "output_" + ofToString(output_count, 3, '0') + ".png";     //set dynamic filename
    output.save("../output/"+filname, OF_IMAGE_QUALITY_BEST);   //export result to output folder
}

void ofApp::reset(){    //reset image using the selected one
    SelectedImg.load(dir.getPath(ImageSelection));
    SelectedImg.setImageType(OF_IMAGE_COLOR);
    imgMat = toCv(SelectedImg);
}

void ofApp::setBrightness(){    //increase or decrease the brightness
    for(int i = 0;i<imgMat.rows*imgMat.cols;i++){
        imgMat.data[i*3] = ofClamp(imgMat.data[i*3]+brightness_value, 0, 255);
        imgMat.data[i*3+1] = ofClamp(imgMat.data[i*3+1]+brightness_value, 0, 255);
        imgMat.data[i*3+2] = ofClamp(imgMat.data[i*3+2]+brightness_value, 0, 255);
    }
}

void ofApp::setContast(){   //increase the color deviation using grey color as average
    float factor = 256*(255.0f + (float)contast_value)/((256.0f - (float)contast_value)*255);     //range: [-255,255] map with 256 to prevent inifinte factor
    for(int i = 0;i<imgMat.rows*imgMat.cols;i++){
        imgMat.data[i*3] = ofClamp((imgMat.data[i*3]-128)*factor + 128, 0, 255);
        imgMat.data[i*3+1] = ofClamp((imgMat.data[i*3+1]-128)*factor + 128, 0, 255);
        imgMat.data[i*3+2] = ofClamp((imgMat.data[i*3+2]-128)*factor + 128, 0, 255);
    }
}

void ofApp::colorMask(){    //modify color tone
    ofColor colors = colorMask_color;   //shift toward the seleced color
    for(int i = 0;i<imgMat.rows*imgMat.cols;i++){
        imgMat.data[i*3] = ofClamp(colorMask_value*(colors.r - imgMat.data[i*3]) + imgMat.data[i*3], 0, 255);
        imgMat.data[i*3+1] = ofClamp(colorMask_value*(colors.g - imgMat.data[i*3+1]) + imgMat.data[i*3+1], 0, 255);
        imgMat.data[i*3+2] = ofClamp(colorMask_value*(colors.b - imgMat.data[i*3+2]) + imgMat.data[i*3+2], 0, 255);
    }
}

void ofApp::gaussianBlur(){     //blur 1
    GaussianBlur(imgMat, imgMat, cv::Size(0, 0), gaussianBlur_value);
}

void ofApp::medianBlur(){      //blur 2
    ofxCv::medianBlur(imgMat, imgMat, medianBlur_value);
}

void ofApp::removeBackground(){
    Mat mask;
    vector<Mat> bgr;
    split(imgMat, bgr);
    Mat greyMat;
    imgMat.copyTo(greyMat);
    cvtColor(greyMat, greyMat, CV_BGR2GRAY);
    
    if(removal_choice == 3){
        threshold(greyMat, mask, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    } else {
        threshold(bgr[removal_choice], mask, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    }
    
    bitwise_and(bgr[0], mask, bgr[0]);
    bitwise_and(bgr[1], mask, bgr[1]);
    bitwise_and(bgr[2], mask, bgr[2]);
    
    merge(bgr, imgMat);
}

void ofApp::_flare(){
    Mat blurred;    //prepare the blurred img
    
    Mat mask;
    vector<Mat> bgr;
    vector<Mat> bgr2;
    split(imgMat, bgr);
    split(imgMat, bgr2);
    Mat greyMat;
    imgMat.copyTo(greyMat);
    cvtColor(greyMat, greyMat, CV_BGR2GRAY);
    
    threshold(greyMat, mask, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    
    bitwise_and(bgr[0], mask, bgr[0]);  //remove far objects
    bitwise_and(bgr[1], mask, bgr[1]);
    bitwise_and(bgr[2], mask, bgr[2]);
    
    bitwise_not(bgr2[0], mask, bgr2[0]);    //remove close objects
    bitwise_not(bgr2[1], mask, bgr2[1]);
    bitwise_not(bgr2[2], mask, bgr2[2]);
    
    merge(bgr, imgMat);     //get the closed objects
    merge(bgr2, blurred);   //get the far object
    GaussianBlur(blurred, blurred, cv::Size(0,0), flare_value);
    for(int i=0;i<imgMat.cols*imgMat.rows;i++){
        if(imgMat.data[i*3]==0 && imgMat.data[i*3+1]==0 && imgMat.data[i*3+2]==0){ //fill back in far objects
            imgMat.data[i*3] = blurred.data[i*3];
            imgMat.data[i*3+1] = blurred.data[i*3+1];
            imgMat.data[i*3+2] = blurred.data[i*3+2];
        }
    }
}

void ofApp::_edge(){
    Mat greyMat;
    cvtColor(imgMat,greyMat, CV_BGR2GRAY);
    GaussianBlur(greyMat, greyMat, 3);
    Canny(greyMat,greyMat,Sketch_effect_value,Sketch_effect_value*2);
    cvtColor(greyMat, greyMat, CV_BGR2RGB);
    greyMat.copyTo(imgMat);
}

void ofApp::_toony(){
    int value = (int)((float)256/(float)Toony_effect_value);
    for(int i=0;i<imgMat.cols*imgMat.rows;i++){
        int factor_r = (int)floor((float)imgMat.data[i*3]/(float)value);
        imgMat.data[i*3] = factor_r * value;
        int factor_g = (int)floor((float)imgMat.data[i*3+1]/(float)value);
        imgMat.data[i*3+1] = factor_g * value;
        int factor_b = (int)floor((float)imgMat.data[i*3+2]/(float)value);
        imgMat.data[i*3+2] = factor_b * value;
    }
    ofxCv::medianBlur(imgMat, imgMat, 8);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    drawMat(imgMat,1024/2 - imgMat.cols/2,768/2 - imgMat.rows/2);
    gui_img.draw();
    gui_tools.draw();
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
    if(Crop == true){       //crop button is pressed
        if(mouseX>1024/2 - imgMat.cols/2 && mouseX<1024/2 + imgMat.cols/2&&     //ensure creating cropping zone within the image
           mouseY>768/2 - imgMat.rows/2 && mouseY<768/2 + imgMat.rows/2){
            IsCreatingCropArea = true;
            Startposition = ofVec2f(mouseX,mouseY);     //store start position
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if(Crop == true){   //crop button is pressed
        if(IsCreatingCropArea){     //creating cropping zone
            if(mouseX>1024/2 - imgMat.cols/2 && mouseX<1024/2 + imgMat.cols/2&&
               mouseY>768/2 - imgMat.rows/2 && mouseY<768/2 + imgMat.rows/2){
                Endposition = ofVec2f(mouseX,mouseY);   //store end position
                int width = abs((int)Startposition.x-(int)Endposition.x);   //width of cropped img
                int height = abs((int)Startposition.y-(int)Endposition.y);  //height of cropped img
                int start_x;    //find start pixel of cropped img
                if(Startposition.x<Endposition.x){
                    start_x = (int)Startposition.x - (1024/2 - imgMat.cols/2);
                } else{
                    start_x = (int)Endposition.x - (1024/2 - imgMat.cols/2);
                }
                int start_y;
                if(Startposition.y<Endposition.y){
                    start_y = (int)Startposition.y - (int)(768/2 - imgMat.rows/2);
                } else{
                    start_y = (int)Endposition.y - (int)(768/2 - imgMat.rows/2);
                }
                cout<<start_x<<", "<<start_y;
                Mat cropped = Mat::zeros(height,width,imgMat.type());   //create cropped img
                int start = start_y*imgMat.cols+start_x;    //start position of orginal img
                for(int y=0;y<height;y++){
                    for(int x=0;x<width;x++){
                        cropped.data[(y*width+x)*3] = imgMat.data[(start+y*imgMat.cols+x)*3];
                        cropped.data[(y*width+x)*3+1] = imgMat.data[(start+y*imgMat.cols+x)*3+1];
                        cropped.data[(y*width+x)*3+2] = imgMat.data[(start+y*imgMat.cols+x)*3+2];
                    }
                }
                imgMat = Mat(height,width,cropped.type());
                cropped.copyTo(imgMat);
                
            }
            IsCreatingCropArea = false;     //end creating
            Crop = false;   //turn off crop function
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
