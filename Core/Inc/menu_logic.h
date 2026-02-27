#ifndef MENU_LOGIC_H
#define MENU_LOGIC_H

#include "main.h"

// Initialize Display and Encoder
void Menu_Init(void);

// Blocking: Waits for button press to start
void Menu_Wait_For_Start(void);

// Blocking: Returns number of players (2–8)
int Menu_Select_Player_Count(void);

// NEU: Blocking: Returns modus (0 = Alle Karten, 1 = X Karten, 2 = Flush)
int Menu_Select_Modus(void);

// NEU: Blocking: Returns cards per player (1–8)
int Menu_Select_Karten_Count(void);

// NEU: Frage nach erneutem Start: 1 = gleichen Modus weiter, 0 = neuen Modus wählen
int Menu_Ask_Repeat_Mode(int aktueller_modus, int spieler, int karten);

// NEU: Shows confirmation screen, waits for START press
void Menu_Show_Confirmation(int spieler, int modus, int karten);

// Shows a simple message on the screen (e.g. current phase)
void Menu_Show_Message(char* line1, char* line2);

// Full display reset + re-init
void Display_Reinit(void);

#endif
