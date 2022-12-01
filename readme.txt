Wireless MultiHop Communication
-by team Fiery Minds

Libraries used in the codes were:
-SPI.h            //available in arduino ide library manager
-LiquidCrystal.h  //available in arduino ide library manager
-RF24.h           //available in arduino ide library manager
-RF24Network.h    //available in arduino ide library manager
-RF24Mesh.h       //available in arduino ide library manager
-dht.h            //downloaded from https://www.brainy-bits.com/post/how-to-use-the-dht11-temperature-and-humidity-sensor-with-an-arduino

There are three code files used in the project:
-Masters_To_Nodes.ino
-Final_reciever.ino
-Mesh_Middle.ino

-Master_To_Nodes.ino is uploaded on thr arduino in master node which is connected to DHT sensor
-Final_reciver.ino is uploaded on the arduino in final node which is connected to LCD
-Mesh_Middle.ino is uploaded on arduino in the middle node

After uploading all the code files on respective arduinos,whenever power source is connected to all the arduinos,
the Master node will automatically send the temperature and humidity data every 5 seconds which is displayed on the LCD in the Final node.
In the mesh configuration sensors/nodes can move around physically, far from the Master node and the 
addressing and topology is automatically reconfigured as connections are broken and re-established within different areas of the network.
If the Final Node is directly in range of Master node then data is directly sent to the final node otherwise it first goes to the middle node
and then reaches the final node.
