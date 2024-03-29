//************************************************************
// PainLessMesh Serial Controller
// to receive Serial Commands
// and send to mesh
//
//
// Adapted from PainLess Mesh sample code - basic
// https://gitlab.com/painlessMesh/painlessMesh
//
// Version 0.1
// -- Early development
//************************************************************

// No interrupts - Not Advised
//#define FASTLED_ALLOW_INTERRUPTS 0
// ESP8266 Interrupt Option
#define FASTLED_INTERRUPT_RETRY_COUNT 1
#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// Task timings
unsigned int updateNodeListTiming = 3;
unsigned int sendMessageTiming = 3;

// Serial data
unsigned int incomingByte = 0;
unsigned int led_frame_rate = 30;

// Other data
unsigned int maxNumberNodes = 40;
unsigned int nodeArray[40];
int counter = 0;


// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void updateNodeList() ;

// User tasks
Task taskSendMessage( TASK_SECOND * sendMessageTiming , TASK_FOREVER, &sendMessage );
Task taskUpdateNodeList( TASK_SECOND * updateNodeListTiming, TASK_FOREVER, &updateNodeList );

//*******************************************************
//**** Update Node List *********************************
//*******************************************************

void updateNodeList()
{
  if (taskUpdateNodeList.isFirstIteration())
  {
    Serial.println("Update Node List Task First Iteration");
  }

  // Number of nodes
  SimpleList<uint32_t> nodes;

  // Get Node List
  nodes = mesh.getNodeList();

  // Get List size
  //  uint32_t theListSize = nodes->size();
  int theListSize = nodes.size();

  // Serial Output node list data1
  Serial.printf("Node List Size -> %d\n", theListSize);

  counter = 0;

  // Try to print all nodes
  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
    nodeArray[counter] = (int) * node;
    counter++;
  }
  Serial.println();
  Serial.print("Number of Nodes: ");
  Serial.println(counter);

  Serial.println("List of Nodes: ");
  for (int i = 0; i < counter; i++)
  {
    Serial.println(nodeArray[i]);
  }

  //  taskUpdateNodeList.setInterval(TASK_SECOND * updateNodeListTiming);
}



//*******************************************************
//**** Send Message *************************************
//*******************************************************

void sendMessage()
{
  if (taskSendMessage.isFirstIteration())
  {
    Serial.println("Send Message Task First Iteration");
  }

  String msg = "Hello from Control node -> ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval(TASK_SECOND * sendMessageTiming);

  // Frame rate timing (untested)
  //Task taskSendMessage((int)(1000 / led_frame_rate), TASK_FOREVER, &sendMessage );
}

//*******************************************************
//**** Receive Callback *********************************
//*******************************************************

// Needed for painless library
void receivedCallback( uint32_t from, String &msg )
{
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

//*******************************************************
//**** New Connection ***********************************
//*******************************************************

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);

}

//*******************************************************
//**** Changed Connection *******************************
//*******************************************************

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
}

//*******************************************************
//**** Time Adjusted ************************************
//*******************************************************

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

//*******************************************************
//**** Setup ********************************************
//*******************************************************

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Clear Array
  for (int x = 0; x < sizeof(nodeArray) / sizeof(nodeArray[0]); x++)
  {
    nodeArray[x] = 0;
  }

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  // Define and enable User tasks
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  userScheduler.addTask( taskUpdateNodeList );
  taskUpdateNodeList.enable();
}

//*******************************************************
//**** Loop *********************************************
//*******************************************************

void loop()
{
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();

  // Receive serial data
  if (Serial.available() > 0)
  {
    incomingByte = Serial.read();
    Serial.print("Incoming Serial Data -> ");
    Serial.println(incomingByte);

    String msg = String(incomingByte);
    mesh.sendBroadcast(msg);

    /*

    */
  }
}

// JSON data examples
//{
//  "mode": "AAA",
//  "count": 255,
//  "blinkCount": 255,
//  "rgb": [ 255, 255, 255 ],
//  "hsv": [ 255, 255, 255 ],
//  "binaryPattern": "ff",
//  "numberColors": 5,
//  "rgb2": [ 255, 255, 255 ],
//  "rgb3": [ 255, 255, 255 ],
//  "rgb4": [ 255, 255, 255 ],
//  "rgb5": [ 255, 255, 255 ],
//  "rgb6": [ 255, 255, 255 ],
//  "rgb7": [ 255, 255, 255 ],
//  "rgb8": [ 255, 255, 255 ]
//}

// Parser
//const size_t capacity = 9*JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(14) + 110;
//DynamicJsonDocument doc(capacity);
//
//const char* json = "{\"mode\":\"AAA\",\"count\":255,\"blinkCount\":255,\"rgb\":[255,255,255],\"hsv\":[255,255,255],\"binaryPattern\":\"ff\",\"numberColors\":5,\"rgb2\":[255,255,255],\"rgb3\":[255,255,255],\"rgb4\":[255,255,255],\"rgb5\":[255,255,255],\"rgb6\":[255,255,255],\"rgb7\":[255,255,255],\"rgb8\":[255,255,255]}";
//
//deserializeJson(doc, json);
//
//const char* mode = doc["mode"]; // "AAA"
//int count = doc["count"]; // 255
//int blinkCount = doc["blinkCount"]; // 255
//
//JsonArray rgb = doc["rgb"];
//int rgb_0 = rgb[0]; // 255
//int rgb_1 = rgb[1]; // 255
//int rgb_2 = rgb[2]; // 255
//
//JsonArray hsv = doc["hsv"];
//int hsv_0 = hsv[0]; // 255
//int hsv_1 = hsv[1]; // 255
//int hsv_2 = hsv[2]; // 255
//
//const char* binaryPattern = doc["binaryPattern"]; // "ff"
//int numberColors = doc["numberColors"]; // 5
//
//JsonArray rgb2 = doc["rgb2"];
//int rgb2_0 = rgb2[0]; // 255
//int rgb2_1 = rgb2[1]; // 255
//int rgb2_2 = rgb2[2]; // 255
//
//JsonArray rgb3 = doc["rgb3"];
//int rgb3_0 = rgb3[0]; // 255
//int rgb3_1 = rgb3[1]; // 255
//int rgb3_2 = rgb3[2]; // 255
//
//JsonArray rgb4 = doc["rgb4"];
//int rgb4_0 = rgb4[0]; // 255
//int rgb4_1 = rgb4[1]; // 255
//int rgb4_2 = rgb4[2]; // 255
//
//JsonArray rgb5 = doc["rgb5"];
//int rgb5_0 = rgb5[0]; // 255
//int rgb5_1 = rgb5[1]; // 255
//int rgb5_2 = rgb5[2]; // 255
//
//JsonArray rgb6 = doc["rgb6"];
//int rgb6_0 = rgb6[0]; // 255
//int rgb6_1 = rgb6[1]; // 255
//int rgb6_2 = rgb6[2]; // 255
//
//JsonArray rgb7 = doc["rgb7"];
//int rgb7_0 = rgb7[0]; // 255
//int rgb7_1 = rgb7[1]; // 255
//int rgb7_2 = rgb7[2]; // 255
//
//JsonArray rgb8 = doc["rgb8"];
//int rgb8_0 = rgb8[0]; // 255
//int rgb8_1 = rgb8[1]; // 255
//int rgb8_2 = rgb8[2]; // 255

// Serialization
//const size_t capacity = 9*JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(14);
//DynamicJsonDocument doc(capacity);
//
//doc["mode"] = "AAA";
//doc["count"] = 255;
//doc["blinkCount"] = 255;
//
//JsonArray rgb = doc.createNestedArray("rgb");
//rgb.add(255);
//rgb.add(255);
//rgb.add(255);
//
//JsonArray hsv = doc.createNestedArray("hsv");
//hsv.add(255);
//hsv.add(255);
//hsv.add(255);
//doc["binaryPattern"] = "ff";
//doc["numberColors"] = 5;
//
//JsonArray rgb2 = doc.createNestedArray("rgb2");
//rgb2.add(255);
//rgb2.add(255);
//rgb2.add(255);
//
//JsonArray rgb3 = doc.createNestedArray("rgb3");
//rgb3.add(255);
//rgb3.add(255);
//rgb3.add(255);
//
//JsonArray rgb4 = doc.createNestedArray("rgb4");
//rgb4.add(255);
//rgb4.add(255);
//rgb4.add(255);
//
//JsonArray rgb5 = doc.createNestedArray("rgb5");
//rgb5.add(255);
//rgb5.add(255);
//rgb5.add(255);
//
//JsonArray rgb6 = doc.createNestedArray("rgb6");
//rgb6.add(255);
//rgb6.add(255);
//rgb6.add(255);
//
//JsonArray rgb7 = doc.createNestedArray("rgb7");
//rgb7.add(255);
//rgb7.add(255);
//rgb7.add(255);
//
//JsonArray rgb8 = doc.createNestedArray("rgb8");
//rgb8.add(255);
//rgb8.add(255);
//rgb8.add(255);
//
//serializeJson(doc, Serial);
