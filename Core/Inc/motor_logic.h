#ifndef INC_MOTOR_LOGIC_H_
#define INC_MOTOR_LOGIC_H_

#include "main.h"

// Neue Funktions-Namen für den Zeit-Ablauf
void Phase1_Sortieren_1Minute(void);
void Phase2_Transport_1Minute(void);

// Phase 3: Auswurf, jetzt mit Kartenlimit (karten_pro_spieler; 99 = alle)
void Phase3_Auswurf(int spieler_anzahl, int karten_pro_spieler);

// Phase 3 Flush: Alle Karten an Position 1 auswerfen
void Phase3_Flush(void);

// Diagnose-Funktion für Stepper (M8)
void Stepper_Test_Only(void);

// Globaler Motorstopp
void All_Motors_Stop_Immediate(void);

#endif /* INC_MOTOR_LOGIC_H_ */
