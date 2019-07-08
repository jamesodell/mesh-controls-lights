# mesh-controls-lights
Using PainLess Mesh and ESP8266 to create mesh network of RGB addressable lights for interactive display




Description: 

The basic idea is to use PainLess Mesh networking to connect several ESP8266 (or ESP32) nodes together. Each node will have 8 WS2812B lights for control. Party guests will wear the Light nodes and help create interactive light displays and movements.

A Master node will send commands to the other nodes to change light patterns. Light patterns can be send to all nodes, only a single node or patterns (ie odd or even).

The initial version will use the FastLED library to control the WS2812B lights. IF this library causes interrupt problems with the Wifi mesh, other libraries can be tried.

The light patterns will start off simple and hopefully get more elaborate over time. I would like to add some music reactive modes.

Each node will consist of an ESP8266, 8 lights (could be individual, a strip, sewn into clothing, etc) and a battery.

Messaging between nodes will be JSON. Next task, figure out data required, types, and formats.

Current goals:
- Basic PainLessMesh operation
  - Establish network
  - Ensure nodes can add/drop dynamically
  - Determine JSON data and types
  - Ensure lights nodes can receive proper data for each light mode
  - Count active nodes
  - Determine how fast updates can occur without degrading network quality
  - Master Node
    - Can receive data from source, probably serial from computer
    - How many nodes can be supported with ESP8266
    - Maintain dynamic mode list with numbered order list
      - Used to create odd/even or other patterns
    - Ensure Master node can send to a signal light mode, all light nodes, or a defined subset of light nodes
    - Provide feedback on how many nodes active in real time
  - Light Node
    - Receive and decode JSON data
- JSON
  - Determine which set of data is required for Light modes and control
  - Determine maximum data size required
  - Can only required data be sent for the current Light mode or does all data have to be sent each time?

Light mode ideas:
- Off
- Solid color
  - Primary (named)
  - Set by RGB
  - Set by HSV
  - Random
- Flashing
- Flashing between two or more colors
- Binary patterns (counting or showing address)
- Larson scanner
- Ball bounce
- Fire effects
- Set brightness
- Animal eyes
- Direct pattern (Master sends all required color data for all pixels)
- Random pixel colors
- Fade in/out



Future goals:
- Have light nodes talk back to control node or each other
- Music visualization modes
- Naming nodes
- External visualization of node config
  - Changes realtime
- Determine how many LEDs possible before mesh has issues
- Add sensor data input to master node to affect light nodes
- Use PainLess Mesh with Python on desktop or raspberry pi to allow easier interacing to everything
- Add accelerometer to Light nodes for movement light effects
- Add noise level measurement to Master node for loudness effects


