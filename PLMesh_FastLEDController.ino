//************************************************************
// PainLessMesh LED Controller
// to control FastLED Addressable RGB pixels
//
// Respond to a mesh message and execute
// light pattern
//
// Using no delays
// each light sequence should pass through once
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
#include <FastLED.h>

// PainLessMesh defines
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

// FastLED defines
#define   NUM_LEDS        8
#define   DATA_PIN        2

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// LED data array
CRGB leds[NUM_LEDS];
unsigned int led_brightness = 150;

// Current LED pattern
char led_state = '0';
unsigned int led_counter = 0;
unsigned int led_frame_rate = 30;

// User task function prototypes
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void LEDController(); // LED Controlelr prototype

// Define user tasks
Task taskSendMessage( TASK_SECOND * 3 , TASK_FOREVER, &sendMessage );
Task taskLEDController((int)(1000 / led_frame_rate), TASK_FOREVER, &LEDController);

// User Task - Send message 'msg'

//*******************************************************
//************ Send Message *****************************
//*******************************************************
void sendMessage()
{
  //  String msg = "Hello from Light node -> ";
  //  msg += mesh.getNodeId();
  //  mesh.sendBroadcast( msg );    // Node NOT receives own broadcast message
  //  //  mesh.sendBroadcast( msg, true );  // Node receives own broadcast message
  //
  //  // bool painlessMesh::sendSingle(uint32_t dest, String &msg)
  //  // String painlessMesh::subConnectionJson() // Returns mesh topology in JSON
  //
  //  taskSendMessage.setInterval(TASK_SECOND * 3);
}

// Run different LED pattern based on 'led_state'

//*******************************************************
//*********** LED Controller ****************************
//*******************************************************

void LEDController()
{

  // Change LED pattern based on received message
  Serial.print("LED State -> ");
  Serial.println(led_state);
  switch (led_state)
  {
    case '1':   // RED
      {
        // Start all leds RED
        for (int i = 0; i < NUM_LEDS; i++)
        {
          leds[i] = CRGB::Red;
        }
        FastLED.show();
        break;
      }
    case '2':   // GREEN
      {
        // Start all leds GREEN
        for (int i = 0; i < NUM_LEDS; i++)
        {
          leds[i] = CRGB::Green;
        }
        FastLED.show();
        break;
      }
    case '3':   // BLUE
      {
        // Start all leds BLUE
        for (int i = 0; i < NUM_LEDS; i++)
        {
          leds[i] = CRGB::Blue;
        }
        FastLED.show();
        break;
      }
    case '4':   // One pixel moving
      {
        // Start all leds RED
        for (int i = 0; i < NUM_LEDS; i++)
        {
          leds[i] = CRGB::Red;
        }

        leds[led_counter] = CRGB::Green;
        led_counter++;
        Serial.print("LED Counter -> ");
        Serial.println(led_counter);
        if (led_counter >= NUM_LEDS)
        {
          led_counter = 0;
        }
      }
      FastLED.show();
      break;
    default:
      {
        // Maybe default should be a timeout check
        // Change to default pattern if no new commands recevied in x seconds
        // to ensure the lights are always doing something, even disconnected


        //        // Start all leds WHITE
        //        for (int i = 0; i < NUM_LEDS; i++)
        //        {
        //          leds[i] = CRGB::White;
        //        }
        //        FastLED.show();
        //
        //        break;
      }


      // Set Task interval
      // TODO Will this work if led_frame_rate changed dynamically?
      taskLEDController.setInterval((int)(1000 / led_frame_rate));
  }
}


// Painless Mesh Callbacks

//*******************************************************
//***************** Receive Message ********************
//*******************************************************

void receivedCallback( uint32_t from, String & msg )
{
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());

  // TODO May want to stay with string instead of int
  led_state = atoi(msg.c_str());
  // TEST
  Serial.print("led_state -> ");
  Serial.println(led_state);
}

void newConnectionCallback(uint32_t nodeId)
{
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback()
{
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset)
{
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

//*******************************************************
//******************* Setup *****************************
//*******************************************************

void setup()
{
  Serial.begin(115200);

  // FastLED setup - Blank all LEDs
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(led_brightness);
  FastLED.clear(true);


  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION);  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  // Add Send Message User Task
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  // Add FastLED User Task
  userScheduler.addTask( taskLEDController );
  taskLEDController.enable();

}

//*******************************************************
//**************** Loop *********************************
//*******************************************************

void loop()
{
  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
}






//
//  // Change LED pattern based on received message
//  switch (led_state)
//  {
//    case 1:
//      {
//        // RGBLoop - no parameters
//        RGBLoop();
//        break;
//      }
//
//    case 2:
//      {
//        // HalloweenEyes - Color (red, green, blue), Size of eye, space between eyes, fade (true/false), steps, fade delay, end pause
//        HalloweenEyes(0xff, 0x00, 0x00,
//                      1, 4,
//                      true, random(5, 50), random(50, 150),
//                      random(1000, 10000));
//        HalloweenEyes(0xff, 0x00, 0x00,
//                      1, 4,
//                      true, random(5, 50), random(50, 150),
//                      random(1000, 10000));
//        break;
//      }
//
//    case 3:
//      {
//        // CylonBounce - Color (red, green, blue), eye size, speed delay, end pause
//        CylonBounce(0xff, 0x00, 0x00, 4, 10, 50);
//        break;
//      }
//
//    case 4:
//      {
//        // NewKITT - Color (red, green, blue), eye size, speed delay, end pause
//        NewKITT(0xff, 0x00, 0x00, 8, 10, 50);
//        break;
//      }
//
//    case 5:
//      {
//        // Twinkle - Color (red, green, blue), count, speed delay, only one twinkle (true/false)
//        Twinkle(0xff, 0x00, 0x00, 10, 100, false);
//        break;
//      }
//
//    case 6:
//      {
//        // SnowSparkle - Color (red, green, blue), sparkle delay, speed delay
//        SnowSparkle(0x10, 0x10, 0x10, 20, random(100, 1000));
//        break;
//      }
//
//    case 7:
//      {
//        // colorWipe - Color (red, green, blue), speed delay
//        colorWipe(0x00, 0xff, 0x00, 50);
//        colorWipe(0x00, 0x00, 0x00, 50);
//        break;
//      }
//
//    case 8:
//      {
//        // rainbowCycle - speed delay
//        rainbowCycle(20);
//        break;
//      }
//
//    case 9:
//      {
//        // Fire - Cooling rate, Sparking rate, speed delay
//        Fire(55, 120, 15);
//        break;
//      }
//
//    default:
//      {
//        FastLED.clear(true);
//        for (int i = 0; i < NUM_LEDS; i++)
//        {
//          leds[i] = CRGB::Red;
//          i++;
//        }
//        FastLED.show();
//        break;
//      }



//*******************************************************
//*******************************************************
//*******************************************************

// *************************
// ** LEDEffect Functions **
// *************************

//
//// TODO Need to eliminate loops
//void RGBLoop()
//{
//  for (int j = 0; j < 3; j++ )
//  {
//    // Fade IN
//    for (int k = 0; k < 256; k++)
//    {
//      switch (j) {
//        case 0: setAll(k, 0, 0); break;
//        case 1: setAll(0, k, 0); break;
//        case 2: setAll(0, 0, k); break;
//      }
//      showStrip();
//
//    }
//    // Fade OUT
//    for (int k = 255; k >= 0; k--)
//    {
//      switch (j)
//      {
//        case 0: setAll(k, 0, 0); break;
//        case 1: setAll(0, k, 0); break;
//        case 2: setAll(0, 0, k); break;
//      }
//      showStrip();
//
//    }
//  }
//}
//
//void FadeInOut(byte red, byte green, byte blue)
//{
//  float r, g, b;
//
//  for (int k = 0; k < 256; k = k + 1)
//  {
//    r = (k / 256.0) * red;
//    g = (k / 256.0) * green;
//    b = (k / 256.0) * blue;
//    setAll(r, g, b);
//    showStrip();
//  }
//
//  for (int k = 255; k >= 0; k = k - 2)
//  {
//    r = (k / 256.0) * red;
//    g = (k / 256.0) * green;
//    b = (k / 256.0) * blue;
//    setAll(r, g, b);
//    showStrip();
//  }
//}
//
//void Strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause)
//{
//  for (int j = 0; j < StrobeCount; j++)
//  {
//    setAll(red, green, blue);
//    showStrip();
//    //    delay(FlashDelay);
//    setAll(0, 0, 0);
//    showStrip();
//    //    delay(FlashDelay);
//  }
//
//  //  delay(EndPause);
//}
//
//void HalloweenEyes(byte red, byte green, byte blue,
//                   int EyeWidth, int EyeSpace,
//                   boolean Fade, int Steps, int FadeDelay,
//                   int EndPause)
//{
//  randomSeed(analogRead(0));
//
//  int i;
//  int StartPoint  = random( 0, NUM_LEDS - (2 * EyeWidth) - EyeSpace );
//  int Start2ndEye = StartPoint + EyeWidth + EyeSpace;
//
//  for (i = 0; i < EyeWidth; i++)
//  {
//    setPixel(StartPoint + i, red, green, blue);
//    setPixel(Start2ndEye + i, red, green, blue);
//  }
//
//  showStrip();
//
//  if (Fade == true)
//  {
//    float r, g, b;
//
//    for (int j = Steps; j >= 0; j--)
//    {
//      r = j * (red / Steps);
//      g = j * (green / Steps);
//      b = j * (blue / Steps);
//
//      for (i = 0; i < EyeWidth; i++)
//      {
//        setPixel(StartPoint + i, r, g, b);
//        setPixel(Start2ndEye + i, r, g, b);
//      }
//
//      showStrip();
//      //      delay(FadeDelay);
//    }
//  }
//
//  setAll(0, 0, 0); // Set all black
//
//  //  delay(EndPause);
//}
//
//void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
//{
//
//  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++)
//  {
//    setAll(0, 0, 0);
//    setPixel(i, red / 10, green / 10, blue / 10);
//    for (int j = 1; j <= EyeSize; j++) {
//      setPixel(i + j, red, green, blue);
//    }
//    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
//    showStrip();
//    //    delay(SpeedDelay);
//  }
//
//  //  delay(ReturnDelay);
//
//  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--)
//  {
//    setAll(0, 0, 0);
//    setPixel(i, red / 10, green / 10, blue / 10);
//    for (int j = 1; j <= EyeSize; j++) {
//      setPixel(i + j, red, green, blue);
//    }
//    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
//    showStrip();
//    //    delay(SpeedDelay);
//  }
//
//  //  delay(ReturnDelay);
//}
//
//void NewKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
//{
//  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
//  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
//  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
//  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
//  LeftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
//  RightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
//  OutsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
//  CenterToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
//}
//
//// used by NewKITT
//void CenterToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
//{
//  for (int i = ((NUM_LEDS - EyeSize) / 2); i >= 0; i--)
//  {
//    setAll(0, 0, 0);
//
//    setPixel(i, red / 10, green / 10, blue / 10);
//    for (int j = 1; j <= EyeSize; j++)
//    {
//      setPixel(i + j, red, green, blue);
//    }
//    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
//
//    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
//    for (int j = 1; j <= EyeSize; j++)
//    {
//      setPixel(NUM_LEDS - i - j, red, green, blue);
//    }
//    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);
//
//    showStrip();
//    //    delay(SpeedDelay);
//  }
//  //  delay(ReturnDelay);
//}
//
//// used by NewKITT
//void OutsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
//{
//  for (int i = 0; i <= ((NUM_LEDS - EyeSize) / 2); i++)
//  {
//    setAll(0, 0, 0);
//
//    setPixel(i, red / 10, green / 10, blue / 10);
//    for (int j = 1; j <= EyeSize; j++)
//    {
//      setPixel(i + j, red, green, blue);
//    }
//    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
//
//    setPixel(NUM_LEDS - i, red / 10, green / 10, blue / 10);
//    for (int j = 1; j <= EyeSize; j++)
//    {
//      setPixel(NUM_LEDS - i - j, red, green, blue);
//    }
//    setPixel(NUM_LEDS - i - EyeSize - 1, red / 10, green / 10, blue / 10);
//
//    showStrip();
//    //    delay(SpeedDelay);
//  }
//  //  delay(ReturnDelay);
//}
//
//// used by NewKITT
//void LeftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
//{
//  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++)
//  {
//    setAll(0, 0, 0);
//    setPixel(i, red / 10, green / 10, blue / 10);
//    for (int j = 1; j <= EyeSize; j++)
//    {
//      setPixel(i + j, red, green, blue);
//    }
//    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
//    showStrip();
//    //    delay(SpeedDelay);
//  }
//  //  delay(ReturnDelay);
//}
//
//// used by NewKITT
//void RightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay)
//{
//  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--)
//  {
//    setAll(0, 0, 0);
//    setPixel(i, red / 10, green / 10, blue / 10);
//    for (int j = 1; j <= EyeSize; j++)
//    {
//      setPixel(i + j, red, green, blue);
//    }
//    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
//    showStrip();
//    //    delay(SpeedDelay);
//  }
//  //  delay(ReturnDelay);
//}
//
//void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, boolean OnlyOne)
//{
//  setAll(0, 0, 0);
//
//  for (int i = 0; i < Count; i++)
//  {
//    setPixel(random(NUM_LEDS), red, green, blue);
//    showStrip();
//    //    delay(SpeedDelay);
//    if (OnlyOne)
//    {
//      setAll(0, 0, 0);
//    }
//  }
//
//  //  delay(SpeedDelay);
//}
//
//void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne)
//{
//  setAll(0, 0, 0);
//
//  for (int i = 0; i < Count; i++)
//  {
//    setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
//    showStrip();
//    //    delay(SpeedDelay);
//    if (OnlyOne)
//    {
//      setAll(0, 0, 0);
//    }
//  }
//
//  //  delay(SpeedDelay);
//}
//
//void Sparkle(byte red, byte green, byte blue, int SpeedDelay)
//{
//  int Pixel = random(NUM_LEDS);
//  setPixel(Pixel, red, green, blue);
//  showStrip();
//  //  delay(SpeedDelay);
//  setPixel(Pixel, 0, 0, 0);
//}
//
//void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay)
//{
//  setAll(red, green, blue);
//
//  int Pixel = random(NUM_LEDS);
//  setPixel(Pixel, 0xff, 0xff, 0xff);
//  showStrip();
//  //  delay(SparkleDelay);
//  setPixel(Pixel, red, green, blue);
//  showStrip();
//  //  delay(SpeedDelay);
//}
//
//void RunningLights(byte red, byte green, byte blue, int WaveDelay)
//{
//  int Position = 0;
//
//  for (int i = 0; i < NUM_LEDS * 2; i++)
//  {
//    Position++; // = 0; //Position + Rate;
//    for (int i = 0; i < NUM_LEDS; i++) {
//      // sine wave, 3 offset waves make a rainbow!
//      //float level = sin(i+Position) * 127 + 128;
//      //setPixel(i,level,0,0);
//      //float level = sin(i+Position) * 127 + 128;
//      setPixel(i, ((sin(i + Position) * 127 + 128) / 255)*red,
//               ((sin(i + Position) * 127 + 128) / 255)*green,
//               ((sin(i + Position) * 127 + 128) / 255)*blue);
//    }
//
//    showStrip();
//    delay(WaveDelay);
//  }
//}
//
//void colorWipe(byte red, byte green, byte blue, int SpeedDelay)
//{
//  for (uint16_t i = 0; i < NUM_LEDS; i++)
//  {
//    setPixel(i, red, green, blue);
//    showStrip();
//    //    delay(SpeedDelay);
//  }
//}
//
//void rainbowCycle(int SpeedDelay)
//{
//  byte *c;
//  uint16_t i, j;
//
//  for (j = 0; j < 256 * 5; j++)
//  { // 5 cycles of all colors on wheel
//    for (i = 0; i < NUM_LEDS; i++)
//    {
//      c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
//      setPixel(i, *c, *(c + 1), *(c + 2));
//    }
//    showStrip();
//    //    delay(SpeedDelay);
//  }
//}
//
//// used by rainbowCycle and theaterChaseRainbow
//byte * Wheel(byte WheelPos)
//{
//  static byte c[3];
//
//  if (WheelPos < 85)
//  {
//    c[0] = WheelPos * 3;
//    c[1] = 255 - WheelPos * 3;
//    c[2] = 0;
//  }
//  else if (WheelPos < 170)
//  {
//    WheelPos -= 85;
//    c[0] = 255 - WheelPos * 3;
//    c[1] = 0;
//    c[2] = WheelPos * 3;
//  }
//  else
//  {
//    WheelPos -= 170;
//    c[0] = 0;
//    c[1] = WheelPos * 3;
//    c[2] = 255 - WheelPos * 3;
//  }
//
//  return c;
//}
//
//void theaterChase(byte red, byte green, byte blue, int SpeedDelay)
//{
//  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
//    for (int q = 0; q < 3; q++)
//    {
//      for (int i = 0; i < NUM_LEDS; i = i + 3)
//      {
//        setPixel(i + q, red, green, blue);  //turn every third pixel on
//      }
//      showStrip();
//
//      //      delay(SpeedDelay);
//
//      for (int i = 0; i < NUM_LEDS; i = i + 3)
//      {
//        setPixel(i + q, 0, 0, 0);    //turn every third pixel off
//      }
//    }
//  }
//}
//
//void theaterChaseRainbow(int SpeedDelay)
//{
//  byte *c;
//
//  for (int j = 0; j < 256; j++)
//  { // cycle all 256 colors in the wheel
//    for (int q = 0; q < 3; q++)
//    {
//      for (int i = 0; i < NUM_LEDS; i = i + 3)
//      {
//        c = Wheel( (i + j) % 255);
//        setPixel(i + q, *c, *(c + 1), *(c + 2)); //turn every third pixel on
//      }
//      showStrip();
//
//      //      delay(SpeedDelay);
//
//      for (int i = 0; i < NUM_LEDS; i = i + 3)
//      {
//        setPixel(i + q, 0, 0, 0);    //turn every third pixel off
//      }
//    }
//  }
//}
//
//void Fire(int Cooling, int Sparking, int SpeedDelay)
//{
//  static byte heat[NUM_LEDS];
//  int cooldown;
//
//  // Step 1.  Cool down every cell a little
//  for ( int i = 0; i < NUM_LEDS; i++)
//  {
//    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
//
//    if (cooldown > heat[i])
//    {
//      heat[i] = 0;
//    } else {
//      heat[i] = heat[i] - cooldown;
//    }
//  }
//
//  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
//  for ( int k = NUM_LEDS - 1; k >= 2; k--)
//  {
//    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
//  }
//
//  // Step 3.  Randomly ignite new 'sparks' near the bottom
//  if ( random(255) < Sparking )
//  {
//    int y = random(7);
//    heat[y] = heat[y] + random(160, 255);
//    //heat[y] = random(160,255);
//  }
//
//  // Step 4.  Convert heat to LED colors
//  for ( int j = 0; j < NUM_LEDS; j++)
//  {
//    setPixelHeatColor(j, heat[j] );
//  }
//
//  showStrip();
//  //  delay(SpeedDelay);
//}
//
//void setPixelHeatColor (int Pixel, byte temperature)
//{
//  // Scale 'heat' down from 0-255 to 0-191
//  byte t192 = round((temperature / 255.0) * 191);
//
//  // calculate ramp up from
//  byte heatramp = t192 & 0x3F; // 0..63
//  heatramp <<= 2; // scale up to 0..252
//
//  // figure out which third of the spectrum we're in:
//  if ( t192 > 0x80)
//  { // hottest
//    setPixel(Pixel, 255, 255, heatramp);
//  }
//  else if
//  (
//    t192 > 0x40 ) {            // middle
//    setPixel(Pixel, 255, heatramp, 0);
//  }
//  else
//  { // coolest
//    setPixel(Pixel, heatramp, 0, 0);
//  }
//}
//
//void BouncingColoredBalls(int BallCount, byte colors[][3], boolean continuous)
//{
//  float Gravity = -9.81;
//  int StartHeight = 1;
//
//  float Height[BallCount];
//  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
//  float ImpactVelocity[BallCount];
//  float TimeSinceLastBounce[BallCount];
//  int   Position[BallCount];
//  long  ClockTimeSinceLastBounce[BallCount];
//  float Dampening[BallCount];
//  boolean ballBouncing[BallCount];
//  boolean ballsStillBouncing = true;
//
//  for (int i = 0 ; i < BallCount ; i++)
//  {
//    ClockTimeSinceLastBounce[i] = millis();
//    Height[i] = StartHeight;
//    Position[i] = 0;
//    ImpactVelocity[i] = ImpactVelocityStart;
//    TimeSinceLastBounce[i] = 0;
//    Dampening[i] = 0.90 - float(i) / pow(BallCount, 2);
//    ballBouncing[i] = true;
//  }
//
//  while (ballsStillBouncing)
//  {
//    for (int i = 0 ; i < BallCount ; i++) {
//      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
//      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i] / 1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i] / 1000;
//
//      if ( Height[i] < 0 )
//      {
//        Height[i] = 0;
//        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
//        ClockTimeSinceLastBounce[i] = millis();
//
//        if ( ImpactVelocity[i] < 0.01 )
//        {
//          if (continuous)
//          {
//            ImpactVelocity[i] = ImpactVelocityStart;
//          }
//          else
//          {
//            ballBouncing[i] = false;
//          }
//        }
//      }
//      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
//    }
//
//    ballsStillBouncing = false; // assume no balls bouncing
//    for (int i = 0 ; i < BallCount ; i++)
//    {
//      setPixel(Position[i], colors[i][0], colors[i][1], colors[i][2]);
//      if ( ballBouncing[i] ) {
//        ballsStillBouncing = true;
//      }
//    }
//
//    showStrip();
//    setAll(0, 0, 0);
//  }
//}
//
//void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay)
//{
//  setAll(0, 0, 0);
//
//  for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++)
//  {
//    // fade brightness all LEDs one step
//    for (int j = 0; j < NUM_LEDS; j++)
//    {
//      if ( (!meteorRandomDecay) || (random(10) > 5) )
//      {
//        fadeToBlack(j, meteorTrailDecay );
//      }
//    }
//
//    // draw meteor
//    for (int j = 0; j < meteorSize; j++)
//    {
//      if ( ( i - j < NUM_LEDS) && (i - j >= 0) )
//      {
//        setPixel(i - j, red, green, blue);
//      }
//    }
//
//    showStrip();
//    //    delay(SpeedDelay);
//  }
//}
//
//// used by meteorrain
//void fadeToBlack(int ledNo, byte fadeValue)
//{
//#ifdef ADAFRUIT_NEOPIXEL_H
//  // NeoPixel
//  uint32_t oldColor;
//  uint8_t r, g, b;
//  int value;
//
//  oldColor = strip.getPixelColor(ledNo);
//  r = (oldColor & 0x00ff0000UL) >> 16;
//  g = (oldColor & 0x0000ff00UL) >> 8;
//  b = (oldColor & 0x000000ffUL);
//
//  r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 256);
//  g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 256);
//  b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 256);
//
//  strip.setPixelColor(ledNo, r, g, b);
//#endif
//#ifndef ADAFRUIT_NEOPIXEL_H
//  // FastLED
//  leds[ledNo].fadeToBlackBy( fadeValue );
//#endif
//}
//
//// *** REPLACE TO HERE ***
//
//
//
//// ***************************************
//// ** FastLed/NeoPixel Common Functions **
//// ***************************************
//
//// Apply LED color changes
//void showStrip()
//{
//#ifdef ADAFRUIT_NEOPIXEL_H
//  // NeoPixel
//  strip.show();
//#endif
//#ifndef ADAFRUIT_NEOPIXEL_H
//  // FastLED
//  FastLED.show();
//#endif
//}
//
//// Set a LED color (not yet visible)
//void setPixel(int Pixel, byte red, byte green, byte blue)
//{
//#ifdef ADAFRUIT_NEOPIXEL_H
//  // NeoPixel
//  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
//#endif
//#ifndef ADAFRUIT_NEOPIXEL_H
//  // FastLED
//  leds[Pixel].r = red;
//  leds[Pixel].g = green;
//  leds[Pixel].b = blue;
//#endif
//}
//
//// Set all LEDs to a given color and apply it (visible)
//void setAll(byte red, byte green, byte blue)
//{
//  for (int i = 0; i < NUM_LEDS; i++ )
//  {
//    setPixel(i, red, green, blue);
//  }
//  showStrip();
//}
