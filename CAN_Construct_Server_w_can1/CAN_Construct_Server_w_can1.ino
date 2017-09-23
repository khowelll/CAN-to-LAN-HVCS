/* Here the Teensy acts as a server the Python script connects
   to the Teensy as a client. The Python script will include a GUI
   so that custom messages can be entered and sent. I need to
   start thinking of how to do this without using the UDP library
   as it will not work with Wi-Fi

   TODO Ensure CAN messages can be sent using this data
   transfer method, and modify if necessary. Implement CAN functionallity
   to write CAN messages from UDP packets

   After sending CAN messages:
   Setup to read CAN w transceiever 
      build UDP packet, send packet
*/
#include <Ethernet.h>
#include <SPI.h>
#include <EthernetUDP.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>

//for CAN
uint32_t currentMillis = 0;
uint32_t previousMillis10 = 0;
static CAN_message_t txmsg, rxmsg;

//for Ethernet
byte mac[] = { 0xDA, 0xED, 0xBE, 0xEF, 0xFE, 0xED }; //Give the Ethernet Shield a MAC address
unsigned int port = 5555; //the port that the Python socket will connect to
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //Data is sent in an array, the udp
//library is used here to specify the maximum possible udp packet size
String datReq; //Request from python is defined here as a string
int packetSize;
EthernetUDP Udp;

void setup() {
  //initialize Ethernet shield manually to avoid issues
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  delay(10);
  digitalWrite(9, HIGH);

  //Begin serial and CAN communications and Ethernet / UDP connection
  Serial.begin(9600);
  Can0.begin(250000);

  Ethernet.begin(mac);
  Udp.begin(port);
  delay(100); //give everything a chance to initialize

  //print the IP address of the Teensy
  Serial.print("Server IP: ");
  Serial.println(Ethernet.localIP());
  //server.begin();

}

void loop() {
  /* // this code returns bytes but it does not depend on the python client
    EthernetClient client = server.available();
     packetSize = client.read();
    server.println("1");
    client.println("!");
     if (server.available()>0) {
       String datReq(client.read());
       Serial.println(client.read());
       server.println("Message Recieved");
     }
  */
  currentMillis = millis();
  packetSize = Udp.parsePacket(); //read the packet size

  if (packetSize > 0) { //Check to see if there are bytes to read

    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); //read udp request

    //Prints ID of CAN Message
    Serial.print("CAN ID: ");
    Serial.print(packetBuffer[0]);
    Serial.print(packetBuffer[1]);
    Serial.print(packetBuffer[2]);
    Serial.print(packetBuffer[3]);
    Serial.print(packetBuffer[4]);
    Serial.print(packetBuffer[5]);
    Serial.print(packetBuffer[6]);
    Serial.println(packetBuffer[7]);

    //Shot in the dark here, set txmsg = udp bytes
    if (currentMillis - previousMillis10 >= 1000) {
        previousMillis10 = currentMillis;
        txmsg.id = 0x101;
        txmsg.len = 8;
        txmsg.buf[0] = packetBuffer[0];
        txmsg.buf[1] = packetBuffer[1];
        txmsg.buf[2] = packetBuffer[2];
        txmsg.buf[3] = packetBuffer[3];
        txmsg.buf[4] = packetBuffer[4];
        txmsg.buf[5] = packetBuffer[5];
        txmsg.buf[6] = packetBuffer[6];
        txmsg.buf[7] = packetBuffer[7];
    
        Can0.write(txmsg); 
        }
    
    if (datReq) {
      Udp.beginPacket(Udp.remoteIP(), Udp.remotePort()); // initialize packet send
      Udp.print("You sent this message: "); //send string back to client
      Udp.print(datReq);
      Udp.endPacket(); //packet has been seent
      //read message from UDP packet into CAN message
      //byte_1 = first byte of packet.... so on

    }
  }

  memset(packetBuffer, 0, UDP_TX_PACKET_MAX_SIZE);

}
//setup to send can message. "If 8 bits are received,
//fill the bits into the CAN message and send it"
