let handpose, video, hands;
let _modelReady = false;

let value_x, value_y;

let previous_transmission_ts = 0;
const MIN_INTERVAL = 50;

let pHtmlMsg;
let serialOptions = { baudRate: 115200 };
let serial;

function setup() {
  createCanvas(640,480);
  //set video
  video = createCapture(VIDEO);
  video.size(width, height);  //size: 640,480
  video.hide();
  //set ml5 handpose
  handpose = ml5.handpose(video, modelReady);
  handpose.on("hand", gotResults); 
  //init 
  value_x = 0;
  value_y = 0;
   
  //Setup Web Serial using serial.js
  //referenced fromn https://editor.p5js.org/jonfroehlich/sketches/vMbPOkdzu
  serial = new Serial();
  serial.on(SerialEvents.CONNECTION_OPENED, onSerialConnectionOpened);
  serial.on(SerialEvents.CONNECTION_CLOSED, onSerialConnectionClosed);
  serial.on(SerialEvents.DATA_RECEIVED, onSerialDataReceived);
  serial.on(SerialEvents.ERROR_OCCURRED, onSerialErrorOccurred);
  // If we have previously approved ports, attempt to connect with them
  serial.autoConnectAndOpenPreviouslyApprovedPort(serialOptions);
  // Add in a lil <p> element to provide messages. This is optional
  pHtmlMsg = createP("Click anywhere on this page to open the serial connection dialog");
}

function modelReady(){
  console.log('model ready');
  _modelReady = true;
}

function gotResults(results){
  hands = results;  //get detection result
}

function draw() {
  background(200);
  //image(video, 0,0,width,height); 
  //show loading screen 
  if(!_modelReady){
    loadingScreen();
  }
  //draw detected hand
  if (hands) {
    if (hands.length > 0){
      drawKeypoints();
      sendData();
    } 
  }
  //draw output
  drawValues();
}

function loadingScreen(){
  textSize(32);
  textAlign(CENTER);
  let instruction = "Loading";
  text(instruction, width / 2, height / 2);
}

function drawKeypoints() {
    let hand = hands[0];  //get the first detected hand
    
    //connect points with lines
    let annotations = hand.annotations;  //all annotation of one hand
    let parts = Object.keys(annotations);  //get annotations have different parts
    for (let part of parts) {    //each part
      beginShape();
      for (let position of annotations[part]) {  //each component of part
        stroke(0);
        strokeWeight(5);
        noFill();
        let [x, y, z] = position;
        vertex(x, y);  
      }
      endShape();
    }
    
    //draw all detected points
    for (let j = 1; j < hand.landmarks.length; j += 1) {
      let keypoint = hand.landmarks[j];
      fill(255,204,0);
      noStroke();
      ellipse(keypoint[0], keypoint[1], 10, 10);
    }
    
    //draw bounding Box
    let topLeft = hand.boundingBox.topLeft;
    let box_width = hand.boundingBox.bottomRight[0] - topLeft[0];
    let box_height = hand.boundingBox.bottomRight[1] - topLeft[1];
    noFill();
    stroke(255,0,0);
    rect(topLeft[0],topLeft[1],box_width,box_height);
    //get values
    value_x = round(constrain(map(topLeft[0],0,width-box_width,0,1),0,1),4);  //map Xdata to [0,1]
    value_y = round(constrain(map(topLeft[1],0,height-box_height,0,1),0,1),4); //map Ydata to [0,1]
}

function drawValues(){
  textAlign(LEFT);
  textSize(25);
  noStroke();
  fill(0);
  text("X: " + round(value_x,2) + " Y: " + round(value_y,2), 0, 25);
}

function sendData(){
  if(serial.isOpen()){
    const interval = millis() - previous_transmission_ts;
    if(interval > MIN_INTERVAL){
      serial.writeLine(value_x+" "+value_y);
      previous_transmission_ts = millis();
    }
  }
}

//referenced from https://editor.p5js.org/jonfroehlich/sketches/vMbPOkdzu
/**
 * Callback function by serial.js when there is an error on web serial
 * 
 * @param {} eventSender 
 */
 function onSerialErrorOccurred(eventSender, error) {
  console.log("onSerialErrorOccurred", error);
  pHtmlMsg.html(error);
}

/**
 * Callback function by serial.js when web serial connection is opened
 * 
 * @param {} eventSender 
 */
function onSerialConnectionOpened(eventSender) {
  console.log("onSerialConnectionOpened");
  pHtmlMsg.html("Serial connection opened successfully");
}

/**
 * Callback function by serial.js when web serial connection is closed
 * 
 * @param {} eventSender 
 */
function onSerialConnectionClosed(eventSender) {
  console.log("onSerialConnectionClosed");
  pHtmlMsg.html("onSerialConnectionClosed");
}

/**
 * Callback function serial.js when new web serial data is received
 * 
 * @param {*} eventSender 
 * @param {String} newData new data received over serial
 */
function onSerialDataReceived(eventSender, newData) {
  console.log("onSerialDataReceived", newData);
  pHtmlMsg.html("onSerialDataReceived: " + newData);
}
/**
 * Called automatically by the browser through p5.js when mouse clicked
 */
function mouseClicked() {
  if (!serial.isOpen()) {
    serial.connectAndOpen(null, serialOptions);
  }
}
