#ifndef INC_MOTOR_LOGIC_H_
#define INC_MOTOR_LOGIC_H_

#include "main.h"

// Neue Funktions-Namen für den Zeit-Ablauf
void Phase1_Sortieren_1Minute(void);
void Phase2_Transport_1Minute(void);

// Geändert: Nimmt jetzt Anzahl der Spieler entgegen
void Phase3_Auswurf(int spieler_anzahl);

// Diagnose-Funktion für Stepper (M8)
void Stepper_Test_Only(void);

#endif /* INC_MOTOR_LOGIC_H_ */
