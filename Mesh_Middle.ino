
//LIBRARIES
#include "RF24.h"                   //Used for NRF24L01 module 
#include "RF24Network.h"            //To develop network among the NRF24L01 modules
#include "RF24Mesh.h"               //To maintain the connectivity between nodes 
#include <SPI.h>                    //For SPI protocol

/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(9, 10);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/*
 * User Configuration: nodeID - A unique identifier for each radio. Allows addressing
 * to change dynamically with physical changes to the mesh.
 */
#define nodeID 1


uint32_t displayTimer = 0;

//Data structure to store the recieved data
struct payload_t {
  unsigned long temp;
  unsigned long humidity;
};

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need this because of native USB capability
  }

  // Set the nodeID manually
  mesh.setNodeID(nodeID);

  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  //mesh.begin() configure the mesh and requests an address
  if (!mesh.begin()) {  
    if (radio.isChipConnected()) {            
      do {
        // mesh.renewAddress() will return MESH_DEFAULT_ADDRESS on failure to connect
        Serial.println(F("Could not connect to network.\nConnecting to the mesh..."));
      } while (mesh.renewAddress() == MESH_DEFAULT_ADDRESS);
    } else {
      Serial.println(F("Radio hardware not responding."));
      while (1) {
        // hold in an infinite loop
      }
    }
  }
  pinMode(2,OUTPUT);
}



void loop() {
  //mesh.update() called regularly to keep the network and mesh updated
  mesh.update();

  // Send to the master node every second
  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();

    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&displayTimer, 'M', sizeof(displayTimer))) {

      // If a write fails, check connectivity to the mesh network
      if (!mesh.checkConnection()) {
        //refresh the network address
        Serial.println("Renewing Address");
        if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {
          //If address renewal fails, reconfigure the radio and restart the mesh
          //This allows recovery from most if not all radio errors
          mesh.begin();
        }
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: ");
      Serial.println(displayTimer);
      
      //If the node connected to master LED glows
      if(!network.parent()){
         digitalWrite(2, HIGH);
      }else{
        digitalWrite(2, LOW);
      }
    }
  }
  //network.available() tests whether there is a message available to this node
  while (network.available()) {
    RF24NetworkHeader header;
    payload_t payload;
    //network.read() reads a message 
    network.read(header, &payload, sizeof(payload));
    Serial.print("Received packet # temp = ");
    Serial.print(payload.temp);
    Serial.print(" and humidity =  ");
    Serial.println(payload.humidity);
  }
}