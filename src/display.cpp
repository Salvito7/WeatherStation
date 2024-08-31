#ifndef NO_DISPLAY
#include <U8g2lib.h>
#include "config.h"
#include "boards_pinout.h"

#ifdef TTGO_T_LORA32_V2_1
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, OLED_SCL, OLED_SDA, U8X8_PIN_NONE);
#endif
#ifdef TTGO_T_Beam_S3_SUPREME_V3
    U8G2_SH1106_128X64_NONAME_1_SW_I2C(U8G2_R0, OLED_SCL, OLED_SDA, U8X8_PIN_NONE);
#endif

extern bool disableDisplay;

void setup_display() {
    #ifdef NO_DISPLAY
        return;
    #else
        u8g2.begin();
        u8g2.setFont(u8g2_font_6x10_tf);
        u8g2.clearBuffer();
        u8g2.setCursor(0, 10);
        u8g2.print("Hello World!");
        u8g2.sendBuffer();
    #endif
}

void loop_display() {
    //u8g2.clearBuffer();
    //u8g2.setCursor(0, 10);
    //u8g2.print("Hello World!");
    //u8g2.sendBuffer();
}

void display_clear() {
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

void display_toggle(bool toggle) {
    if (toggle) {
        u8g2.setPowerSave(0);
    } else {
        u8g2.setPowerSave(1);
    }
}

void show_display(const String& header, const String& line1, const String& line2, int wait = 0) {
    if(disableDisplay) {
        return;
    }
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print(header);
    u8g2.setCursor(0, 20);
    u8g2.print(line1);
    u8g2.setCursor(0, 30);
    u8g2.print(line2);
    u8g2.sendBuffer();
    delay(wait);
}

void show_display(const String& header, const String& line1, const String& line2, const String& line3, const String& line4, const String& line5, int wait = 0) {
    if(disableDisplay) {
        return;
    }
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print(header);
    u8g2.setCursor(0, 20);
    u8g2.print(line1);
    u8g2.setCursor(0, 30);
    u8g2.print(line2);
    u8g2.setCursor(0, 40);
    u8g2.print(line3);
    u8g2.setCursor(0, 50);
    u8g2.print(line4);
    u8g2.setCursor(0, 60);
    u8g2.print(line5);
    u8g2.sendBuffer();
    delay(wait);
}

void startupScreen(const float &version) {
    show_display(" WeatherStation", "      ", "443mHz", " Wifi: OFF", " BT: OFF", "  v" + String(version), 1500);
}

void showDirectory(const String& currentDir, const String& dirList) {
    if(disableDisplay) {
        return;
    }
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print("Dir: " + currentDir);
    u8g2.setCursor(0, 20);
    u8g2.print(dirList);
    u8g2.sendBuffer();
}

void showStatus(const String& status) {
    if(disableDisplay) {
        return;
    }
    u8g2.clearBuffer();
    u8g2.setCursor(0, 10);
    u8g2.print("Status");
    u8g2.setCursor(0, 20);
    u8g2.print(status);
    u8g2.sendBuffer();
}

#endif