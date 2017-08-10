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

byte current_note = 0;

int pitch_to_dac_value(byte pitch) {
    // Assumes the 3V3 output of the DAC is pumped through an opamp with gain =3
    // for 0-9V9 range
    return (((float) pitch / (float) (12*9)) * (9.0f/9.9f)) * 4096.0f;
}

void noteOnCallback(byte channel, byte pitch, byte velocity) {

    current_note = pitch;
    int value = pitch_to_dac_value(pitch);

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

void noteOffCallback(byte channel, byte note, byte velocity) {
    if(note == current_note) {
        current_note = 0;
        dac.setVoltage(0, false); // Ideally '0' is so low in frequency
                                  // that there is no audible sound
    }
}

void pitchBendCallback(byte channel, int bend) {
    // We'll only handle pitch bend if a note is actually playing
    if(current_note) {
        // Pitch bend => 0 to 16384
        // Usually means major second down to major second up
        // = 1/6 of a volt down to 1/6 of a volt up
        float volts_bend = ((float)bend) / (8192.0f * 6);
        int dac_bend = volts_bend * (1.0f/9.9f) * 4096.0f;
        dac.setVoltage(pitch_to_dac_value(current_note) + dac_bend, false);
    }
}

void setup(void) {
    u8g2.begin();

    MIDI.begin();
    MIDI.setHandleNoteOn(noteOnCallback);
    MIDI.setHandleNoteOff(noteOffCallback);
    MIDI.setHandlePitchBend(pitchBendCallback);

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
