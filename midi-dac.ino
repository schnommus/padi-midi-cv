#include <U8g2lib.h>
#include <SoftwareSerial.h>
#include <MIDI.h>

#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 12, 2, 10);
SoftwareSerial mySerial(17, 5);

MIDI_CREATE_INSTANCE(SoftwareSerial, mySerial, MIDI)

void noteOnCallback(byte channel, byte pitch, byte velocity) {
	u8g2.firstPage();
    int target_pitch = pitch - 64;
    int value = (((float) target_pitch / (float) 36) * (3.0f/3.3f)) * 4096.0f;
    dac.setVoltage(value, false);
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

    dac.begin(0x62);

	u8g2.firstPage();
	do {
		u8g2.setFont(u8g2_font_artossans8_8r);
		u8g2.drawStr(0,7,"MIDI/CV...");
	} while (u8g2.nextPage());
}

void loop(void) {
    MIDI.read();
}
