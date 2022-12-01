//Libraries used 
#include <dht.h>
#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>

#define dht_pin A0 //pin to recieve data from dht11 sensor

/***** Configure the chosen CE,CS pins on NRF24L01 Module *****/
RF24 radio(9, 10);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
dht sensor; //declaring variable for dht sensor

//defining the struct payload consisting of data to send across network
struct payload_t {
  unsigned long temp;
  unsigned long humidity;
};

void setup() {
  Serial.begin(115200);

  while (!Serial) {
    // some boards need this because of native USB capability
  }

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  if (!mesh.begin()) {
    // if mesh.begin() returns false for a master node, then radio.begin() returned false.
    Serial.println(F("Radio hardware not responding."));
    while (1) {
      // hold in an infinite loop
    }
  }
}

uint32_t displayTimer = 0;

void loop() {

  // Call mesh.update to keep the network updated
  mesh.update();

  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();


  // Check for incoming data from the sensors
  if (network.available()) {
    RF24NetworkHeader header;
    network.peek(header);
    Serial.print("Got ");
    uint32_t dat = 0;
    switch (header.type) {
      // Display the incoming data from the sensor nodes
      case 'M':
        network.read(header, &dat, sizeof(dat));
        Serial.print(dat);
        Serial.print(" from RF24Network address 0");
        Serial.println(header.from_node, OCT);
        break;
      case 'L':
        network.read(header, &dat, sizeof(dat));
        Serial.print(dat);
        Serial.print(" from RF24Network address 0");
        Serial.print(header.from_node, OCT);
        Serial.println(" LCD receiver");
        break;
      default:
        network.read(header, 0, 0);
        Serial.println(header.type);
        break;
    }
  }
  
 //Reading data from sensor
 sensor.read11(dht_pin);
  

  // Send each node a message every five seconds
  // Send a different message to node 1, containing another counter instead of millis()
  if (millis() - displayTimer > 5000) {
    payload_t payload = { sensor.temperature, sensor.humidity };//setting values of temperature and humidity to send
    Serial.print("Temperature = ");
    Serial.println(payload.temp);
    Serial.print("Humidity = ");
    Serial.println(payload.humidity);

    for (int i = 0; i < mesh.addrListTop; i++) {
      RF24NetworkHeader header(mesh.addrList[i].address, OCT);                                                //Constructing a header
      Serial.println(network.write(header, &payload, sizeof(payload)) == 1 ? F("Send OK") : F("Send Fail"));  //Sending an message
    }
    displayTimer = millis();
  }
}
