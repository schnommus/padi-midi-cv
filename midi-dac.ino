// Used for LCD
#include <U8g2lib.h>

// Used for MIDI
#include <SoftwareSerial.h>
#include <MIDI.h>

// Used for DAC
#include <Wire.h>
#include <Adafruit_MCP4725.h>

/* PADI CONNECTIONS:
   == DAC ==
   SDA = GC4
   SCL = GC5
   == MIDI ==
   IN = GA0
   OUT = GA4 (N/C)
   == LCD ==
   D0 = GC1
   D1 = GC2
   CS = GC3
   DC = GA5
   RES = GC0
*/

Adafruit_MCP4725 dac;

U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 12, 2, 10);

SoftwareSerial mySerial(17, 5);
MIDI_CREATE_INSTANCE(SoftwareSerial, mySerial, MIDI);

void noteOnCallback(byte channel, byte pitch, byte velocity) {

    // At the moment, only interpret 3 octaves, rest is garbage
    // (so that we can use the 0-3V3 range, don't need an opamp)
    int target_pitch = pitch - 64;
    int value = (((float) target_pitch / (float) 36) * (3.0f/3.3f)) * 4096.0f;
    dac.setVoltage(value, false);

    // Show note values on the display
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_artossans8_8r);
        u8g2.drawStr(0,7,"MIDI/CV...");
        String ch = String(   String("Channel: ") + String(channel) );
        u8g2.drawStr(0,30,ch.c_str());
        String note = String( String("Pitch:   ") + String(pitch) );
        u8g2.drawStr(0,40,note.c_str());
        String vel = String(  String("Vel.:    ") + String(velocity) );
        u8g2.drawStr(0,50,vel.c_str());
        String dac = String(  String("DAC:     ") + String(value) );
        u8g2.drawStr(0,60,dac.c_str());
    } while (u8g2.nextPage());
}

void setup(void) {
    u8g2.begin();

    MIDI.begin();
    MIDI.setHandleNoteOn(noteOnCallback);

    // Noname eBay breakout has this I2C address (after trial & error)
    dac.begin(0x62);

    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_artossans8_8r);
        u8g2.drawStr(0,7,"MIDI/CV...");
    } while (u8g2.nextPage());
}

void loop(void) {
    // This will call any relevant callbacks if appropriate MIDI messages are recieved.
    MIDI.read();
}
