#ifndef MENU_LOGIC_H
#define MENU_LOGIC_H

#include "main.h"

// Initialize Display and Encoder
void Menu_Init(void);

// Blocking function: Shows menu, waits for selection, returns number of players
int Menu_Select_Player_Count(void);

// Shows a simple message on the screen (e.g. current phase)
void Menu_Show_Message(char* line1, char* line2);

// Blocking function: Waits for user to press button to start
void Menu_Wait_For_Start(void);

#endif
