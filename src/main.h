#include <Arduino.h>
#ifndef Main_h
#define Main_h

void HandleRoot();
void HandleNotFound();
void showTextRectangle(String ln1, String ln2, boolean small);
void updateScreen(long now);
bool SleepPM2(void *);
bool WakupPM2(void *);
int SetCurrentPM2();
bool updateScreen(void *);
#endif