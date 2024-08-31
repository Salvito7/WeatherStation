
#ifndef NO_DISPLAY
#ifndef DISPLAY_H
#define DISPLAY_H

void setup_display();
void loop_display();
void display_clear();
void display_toggle(bool toggle);

void show_display(const String& header, const String& line1, const String& line2, int wait = 0);
void show_display(const String& header, const String& line1, const String& line2, const String& line3, const String& line4, const String& line5, int wait = 0);

void startupScreen(const float &version);
void showDirectory(const String& currentDir, const String& dirList);
void showStatus(const String& status);

#endif
#endif