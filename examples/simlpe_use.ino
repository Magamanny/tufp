#include <Arduino.h>
#include "tufp.h"
// A demo, This demo is not tested and is given as an example
tufp_st tufp_s;
char frame[100];

void setup() {
  Serial.begin(9600); // open the serial port at 9600 bps:
}

void loop()
{
    // process 64 bytes of data
    for(int i=0;i<64;i++)
    {
        if(Serial.avaliable())
        {
            char ch = Serial.read();
            tufp_rcv(&tufp_s,ch); // process the byte byte
        }
        // if packet recived break the loop
        if(tufp_s.pending)
        {
            break;
        }
    }
    // process the command
    if(tufp_s.pending)
    {
        tufp_s.pending=0;
        // print the data for debuging
        Serial.print("Code:");
        Serial.println(tufp_s.rcode);

        Serial.print("field 0:");
        Serial.println(tufp_s.field[0]);
        Serial.print("field 1:");
        Serial.println(tufp_s.field[1]);
        Serial.print("field 2:");
        Serial.println(tufp_s.field[2]);

        serial_debug.print("check  :");
        serial_debug.println(tufp_s.check_sum_rcv);
        // check command code
        if(tufp_s.rcode==7)
        {
            // send respose code/packet
            tufp_s.scode = 5; // code 36
            tufp_s.snum = 1; // 1 field
            strcpy(tufp_s.field[0],"33"); //
            tufp_frame(&tufp_s, frame); // load the fram in fram buffer
            Serial.print(frame); // send the frame
        }
    }
    delay(10);
}