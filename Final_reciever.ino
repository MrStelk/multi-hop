//LIBRARIES
#include "RF24.h"                   //Used for NRF24L01 module 
#include "RF24Network.h"            //To develop network among the NRF24L01 modules
#include "RF24Mesh.h"               //To maintain the connectivity between nodes 
#include <SPI.h>                    //For SPI protocol
#include <LiquidCrystal.h>          //For LCD display 

/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(9, 10);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
LiquidCrystal lcd(3,4,5,6,7,8);
/*
 * User Configuration: nodeID - A unique identifier for each radio. Allows addressing
 * to change dynamically with physical changes to the mesh.
 */
#define nodeID 2


uint32_t displayTimer = 0;
uint32_t beginTimer = 0;

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
  pinMode(2, OUTPUT);
  lcd.begin(16,2);
  lcd.print("Temperature=");
  lcd.setCursor(0,1);
  lcd.print("Humidity=");
}



void loop() {

  mesh.update();

  // Send to the master node every second
  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();

    // Send an 'L' type message containing the current millis()
    //network.parent() returns address of its parent 
    //address of master node is 0

    if (!mesh.write(&displayTimer, 'L', sizeof(displayTimer)) || network.parent())  
    {
      
      // If a write fails, check connectivity to the mesh network
      if (!mesh.checkConnection()) {

        //refresh the network address
        Serial.println("Renewing Address");
        if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {

          //If address renewal fails, reconfigure the radio and restart the mesh
          //This allows recovery from most if not all radio errors
          Serial.println("Starting..");
          mesh.begin();
          Serial.println("Ending..");
      }
      } 
      else
      { //If parent is not master tries to reconnect with master for every 6 seconds
        if(network.parent())
        {
          Serial.print("Send OK: ");
          if((millis()-beginTimer) > 6000)
          {
            mesh.begin();           //If master not in range again connects to the other node
            beginTimer = millis();
          }
        }
        else
        {
          Serial.println("Send fail, Test OK");          
        }
      }
      //If the node connected to master LED glows
      if(!network.parent()){
         digitalWrite(2, HIGH);
      }else{
        digitalWrite(2, LOW);
      }
    } else {
      Serial.print("Send OK: ");
      Serial.println(displayTimer);
      if(!network.parent()){
         digitalWrite(2, HIGH);
      }else{
        digitalWrite(2, LOW);
      }
    }
  }
  //network.available() tests whether there is a message available to this node
  while (network.available()) {
    Serial.println(network.parent());
    RF24NetworkHeader header;
    payload_t payload;
    
     //network.read() reads a message
    network.read(header, &payload, sizeof(payload));
    Serial.print("Received packet #");
    Serial.print(payload.temp);
    Serial.print(" at ");
    Serial.println(payload.humidity);

     lcd.setCursor(13,0);
    lcd.print(payload.temp);
    lcd.setCursor(10,1);
    lcd.print(payload.humidity);
  }
}
