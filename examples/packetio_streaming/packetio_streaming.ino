#include <Arduino.h>
#include <packet_interfaces.h>
#include <cobs/Stream.h>
#include <cobs/Print.h>
using namespace packetio;

COBSPrint cobs_out(Serial);
COBSStream cobs_in(Serial);

void setup() {
    Serial.begin(9600);
}

void loop() {
    // send a packet
    cobs_out.print("Starting packet duplicator");
    cobs_out.end();

    // duplicate bytes of every packet read
    while(true) {
        int c = cobs_in.read();
        if(c == COBSStream::EOP) {
            // incoming packet ended - end ours too
            cobs_out.end();
            cobs_in.next();
        }
        else if(c != COBSStream::EOF) {
            // got a byte - duplicate it
            cobs_out.write(c);
            cobs_out.write(c);
        }
    }
}
