#include "motor_logic.h"
#include "main.h"
#include "lichtschranke_logic.h"
#define HALF_TURN_TIME_MS 300   // Zeit für 1/2 Umdrehung (anpassen!)
#define FULL_TURN_TIME_MS 600
#define PAUSE_BETWEEN_MS 1500

// ============================================================
// HILFSFUNKTIONEN
// ============================================================

// Stepper M8 (NEMA 17 mit DRV8825 Treiber)
// STEPPER_IN1 (PB7) -> DIR
// STEPPER_IN2 (PB8) -> STEP

void M8_Rotate(int steps, int delay_ms, int direction) {
    // 1. Set Direction
    // direction: 1 = Forward (SET), 0 = Reverse (RESET)
    if (direction == 1) {
        HAL_GPIO_WritePin(STEPPER_IN1_GPIO_Port, STEPPER_IN1_Pin, GPIO_PIN_SET); 
    } else {
        HAL_GPIO_WritePin(STEPPER_IN1_GPIO_Port, STEPPER_IN1_Pin, GPIO_PIN_RESET);
    }
    
    // Setup-Zeit für DIR-Signal geben
    HAL_Delay(1); 

    // 2. Pulse the STEP pin
    int current_delay = delay_ms + 30; // Start MUCH slower (Soft Start Ramp) - erhöht für mehr Stabilität

    for (int i = 0; i < steps; i++) {
        // High Pulse
        HAL_GPIO_WritePin(STEPPER_IN2_GPIO_Port, STEPPER_IN2_Pin, GPIO_PIN_SET);
        delay_us(50); 
        
        // Low
        HAL_GPIO_WritePin(STEPPER_IN2_GPIO_Port, STEPPER_IN2_Pin, GPIO_PIN_RESET);
        
        // --- RAMP LOGIC ---
        // Accelerate until we reach target 'delay_ms'
        // Slower ramp down
        if (current_delay > delay_ms) {
            current_delay--; 
        }

        // Wait between steps
        HAL_Delay(current_delay); 
    }
}

// ============================================================
// GLOBALER MOTOR-STOPP
// ============================================================

void All_Motors_Stop_Immediate(void)
{
    // M1
    HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);

    // M2
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);

    // M3
    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);

    // M4
    HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_RESET);

    // M5
    HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_RESET);

    // M6
    HAL_GPIO_WritePin(M6_WelleUnten_IN3_GPIO_Port, M6_WelleUnten_IN3_Pin, GPIO_PIN_RESET);

    // M7
    HAL_GPIO_WritePin(M7_SchieberUnten_IN1_GPIO_Port, M7_SchieberUnten_IN1_Pin, GPIO_PIN_RESET);

    // Stepper M8
    HAL_GPIO_WritePin(STEPPER_IN1_GPIO_Port, STEPPER_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(STEPPER_IN2_GPIO_Port, STEPPER_IN2_Pin, GPIO_PIN_RESET);
}

// ============================================================
// DIAGNOSE TEST (Nur Stepper)
// ============================================================
void Stepper_Test_Only(void)
{
    // Full Step Mode (200 steps/rev)
    // 8ms delay target (Slower = Smoother/Stronger)
    M8_Rotate(100, 8, 1);
    HAL_Delay(500); 
    
    // Reverse
    M8_Rotate(100, 8, 0);
    HAL_Delay(1000);
}

// ============================================================
// PHASE 1: Sortieren (M2/M3 konstant, M1 kommt später dazu)
// ============================================================

// Alte, zeitbasierte Logik (Standard aktiv)
#if 0
void Phase1_Sortieren_1Minute(void)
{
    // 1. M2 und M3 starten SOFORT (Wellen drehen konstant)
    // M2: Linksrum (RESET/SET)
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_SET);

    // M3: Rechtsrum (RESET/SET) - gegenläufig zu M2
    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_SET);

    // 2. Warte 10 Sekunden (nur M2/M3 laufen)
    HAL_Delay(10000); 

    // 3. M1 startet (Rechts/Links Zyklus) für den Rest der Minute (50 Sek)
    
    uint32_t start_time = HAL_GetTick();
    while ((HAL_GetTick() - start_time) < 50000) // 50 Sekunden lang
    {
        // M1: Rechts (Vorwärts) -> Test: IN1=SET, IN2=RESET
        HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_Delay(900); // Schiebedauer


        // M1: Links (Rückwärts) -> Test: IN1=RESET, IN2=SET
        HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_SET);
        HAL_Delay(900 + 900); // Rückfahrtdauer
        
        // Kurze Pause
        HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_Delay(1500);
    }

    // 4. Minute vorbei -> ALLES STOPPEN (M1, M2, M3)
    HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);
}
#endif

// Neue, sensorbasierte Logik (Lichtschranke 1) – aktiviert
#if 1
void Phase1_Sortieren_1Minute(void)
{
    if (g_SystemState != SYSTEM_RUN) return;

    // --- 1. M2 und M3 SOFORT starten (erste Drehrichtung) ---
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_SET);

    uint32_t t0 = HAL_GetTick();

    // 10 s: nur M2/M3 laufen, abbrechen, wenn oben keine Karte mehr
    while ((HAL_GetTick() - t0) < 10000 && Lichtschranke_Is_Card_Detected(1)) {
        Check_Encoder_Button();
        if (g_SystemState == SYSTEM_EMERGENCY_STOP) {
            All_Motors_Stop_Immediate();
            return;
        }
        HAL_Delay(20);
    }

    // --- 2. M1 Vorwärts–Rückwärts–Pause-Zyklus, solange oben noch Karte erkannt wird ---
    while (Lichtschranke_Is_Card_Detected(1)) {
        Check_Encoder_Button();
        if (g_SystemState == SYSTEM_EMERGENCY_STOP) {
            All_Motors_Stop_Immediate();
            return;
        }

        // M1 vorwärts
        HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_Delay(900);

        // Prüfen, ob noch Karte oben (kann während Bewegung verschwinden)
        if (!Lichtschranke_Is_Card_Detected(1)) break;

        // M1 rückwärts
        HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_SET);
        HAL_Delay(1800);

        // Prüfen, ob noch Karte oben
        if (!Lichtschranke_Is_Card_Detected(1)) break;

        // Pause
        HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_Delay(1500);
    }

    // M1 sicher stoppen (keine Karte mehr oben -> Phase 1 beendet)
    HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);

    // --- 3. M2/M3 kurz in Gegenrichtung laufen lassen ---
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);

    // Richtungswechsel (wie in Phase 2)
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);

    uint32_t t_rev = HAL_GetTick();
    while ((HAL_GetTick() - t_rev) < 15000) {
        Check_Encoder_Button();
        if (g_SystemState == SYSTEM_EMERGENCY_STOP) {
            All_Motors_Stop_Immediate();
            return;
        }
        HAL_Delay(20);
    }

    // M2/M3 stoppen (Phase 1 komplett beendet, Phase 2 kann starten)
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);
}
#endif


// ============================================================
// PHASE 2: Transport zur Mitte (M4/M5 zuerst, dann M2/M3 dazu)
// ============================================================
//void Phase2_Transport_1Minute(void)
//{
//    // 1. M4 und M5 SOFORT Starten (Schieber zur Mitte)
//    HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_SET);

//
//    // 2. Warte 10 Sekunden (nur M4/M5 laufen)
//    HAL_Delay(10000);
//
//    // 3. M2 und M3 zuschalten (Richtung GETAUSCHT gegenüber Phase 1)
//
//    // M2 (Andersrum)
//    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);
//
//    // M3 (Andersrum)
//    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_SET);
//    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);
//
//    // 4. Alle zusammen laufen lassen für den Rest der Minute (50 Sek)
//    HAL_Delay(50000);
//
//    // 5. Minute vorbei -> ALLES STOPPEN
//    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);
//
//    HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_RESET);
//}

// Alte, zeitbasierte Logik (deaktiviert, bleibt aber im Code)
#if 0
void Phase2_Transport_1Minute(void)
{
    uint32_t start_time = HAL_GetTick();

    // --- M2 & M3 START (laufen konstant, gegenläufig zu Phase 1) ---
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);

    // --- 1 Minute lang M4 & M5 takten ---
    while ((HAL_GetTick() - start_time) < 60000)
    {
        // M4: 1 Umdrehung
        HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port,
                          M4_SchieberLinks_IN3_Pin,
                          GPIO_PIN_SET);
        HAL_Delay(FULL_TURN_TIME_MS);
        HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port,
                          M4_SchieberLinks_IN3_Pin,
                          GPIO_PIN_RESET);

//        HAL_Delay(PAUSE_BETWEEN_MS);

        // M5: 1 Umdrehung
        HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port,
                          M5_SchieberRechts_IN1_Pin,
                          GPIO_PIN_SET);
        HAL_Delay(FULL_TURN_TIME_MS);
        HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port,
                          M5_SchieberRechts_IN1_Pin,
                          GPIO_PIN_RESET);

        HAL_Delay(PAUSE_BETWEEN_MS);
    }

    // --- ALLES STOPPEN ---
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_RESET);
}
#endif

// Neue, sensorbasierte Logik (Lichtschranke 3) – aktiviert
#if 1
void Phase2_Transport_1Minute(void)
{
    if (g_SystemState != SYSTEM_RUN) return;

    // M4/M5 starten (Schieber zur Mitte)
    HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_SET);

    // M2/M3 in Gegenrichtung zu Phase 1
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);

    // Solange in der Mitte Karten erkannt werden (Lichtschranke 3)
    while (Lichtschranke_Is_Card_Detected(3)) {
        Check_Encoder_Button();
        if (g_SystemState == SYSTEM_EMERGENCY_STOP) {
            All_Motors_Stop_Immediate();
            return;
        }

        // M4: 1 Umdrehung
        HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_SET);
        HAL_Delay(FULL_TURN_TIME_MS);
        HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_RESET);

        // M5: 1 Umdrehung
        HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_SET);
        HAL_Delay(FULL_TURN_TIME_MS);
        HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_RESET);

        HAL_Delay(PAUSE_BETWEEN_MS);
    }

    // Keine Karten mehr in der Mitte -> alles stoppen
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_RESET);
}
#endif


// ============================================================
// PHASE 3: Auswurf & Verteilung (Pendulum / Snake Mode)
// ============================================================

// Alte, zeitbasierte Logik (deaktiviert, bleibt aber im Code)
#if 0
void Phase3_Auswurf(int spieler_anzahl)
{
    if (spieler_anzahl < 2) spieler_anzahl = 2;
    
    // 1. Setup Calculations
    // Gear Ratio 3:1 -> 600 Steps for full main rotation
    int total_steps_per_rev = 600; 
    int steps_per_segment = total_steps_per_rev / spieler_anzahl;
    int speed_delay = 12;  // Erhöht von 10 auf 12 für mehr Drehmoment

    // 2. Static State to remember direction between function calls
    // 1 = Forward (1->4), 0 = Reverse (4->1)
    static int direction = 1; 

    // 3. Start Feeder (M6)
    HAL_GPIO_WritePin(M6_WelleUnten_IN3_GPIO_Port, M6_WelleUnten_IN3_Pin, GPIO_PIN_SET);
    
    // Wait 2 seconds for feeder to prime the card
    HAL_Delay(2000); 

    // 4. The Loop
    for (int i = 0; i < spieler_anzahl; i++) {
        
        // --- A. DISPENSE CARD FIRST ---
        // We assume we are already AT the correct position for the first player of this round
        
        // M7 Forward ON (Push Card)
        HAL_GPIO_WritePin(M7_SchieberUnten_IN1_GPIO_Port, M7_SchieberUnten_IN1_Pin, GPIO_PIN_SET);
        HAL_Delay(HALF_TURN_TIME_MS);
        // M7 STOP
        HAL_GPIO_WritePin(M7_SchieberUnten_IN1_GPIO_Port, M7_SchieberUnten_IN1_Pin, GPIO_PIN_RESET);

        HAL_Delay(1000); // Wait for player

        // --- B. MOVE TO NEXT (If not last) ---
        if (i < spieler_anzahl - 1) {
            // If Direction is 1 (Forward), rotate Forward (1)
            // If Direction is 0 (Reverse), rotate Backward (0)
            M8_Rotate(steps_per_segment, speed_delay, direction);
            HAL_Delay(500); // Settle time
        }
    }

    // 5. Toggle Direction for next round
    // If we were Forward (1), next time we are Reverse (0).
    // The machine is physically sitting at the Last Player's position now.
    // Next round, it will Deal to Last Player first, then move backwards.
    direction = !direction; 

    // 6. Stop Feeder
    HAL_GPIO_WritePin(M6_WelleUnten_IN3_GPIO_Port, M6_WelleUnten_IN3_Pin, GPIO_PIN_RESET);
}
#endif

// Neue, sensorbasierte Logik (Lichtschranke 4) – aktiviert
#if 1
void Phase3_Auswurf(int spieler_anzahl)
{
    if (spieler_anzahl < 2) spieler_anzahl = 2;
    
    int total_steps_per_rev = 600; 
    int steps_per_segment = total_steps_per_rev / spieler_anzahl;
    int speed_delay = 12;

    static int direction = 1; // 1 = vorwärts, 0 = rückwärts (Snake)

    // Solange unten Karten erkannt werden (Lichtschranke 4)
    while (Lichtschranke_Is_Card_Detected(4)) {

        if (g_SystemState == SYSTEM_EMERGENCY_STOP) {
            All_Motors_Stop_Immediate();
            return;
        }

        // Feeder (M6) an
        HAL_GPIO_WritePin(M6_WelleUnten_IN3_GPIO_Port, M6_WelleUnten_IN3_Pin, GPIO_PIN_SET);
        HAL_Delay(2000); // Vorlauf

        for (int i = 0; i < spieler_anzahl && Lichtschranke_Is_Card_Detected(4); i++) {
            Check_Encoder_Button();
            if (g_SystemState == SYSTEM_EMERGENCY_STOP) {
                All_Motors_Stop_Immediate();
                return;
            }

            // Karte auswerfen: M7
            HAL_GPIO_WritePin(M7_SchieberUnten_IN1_GPIO_Port, M7_SchieberUnten_IN1_Pin, GPIO_PIN_SET);
            HAL_Delay(HALF_TURN_TIME_MS);
            HAL_GPIO_WritePin(M7_SchieberUnten_IN1_GPIO_Port, M7_SchieberUnten_IN1_Pin, GPIO_PIN_RESET);

            HAL_Delay(1000);

            // Zur nächsten Spielerposition drehen (außer beim letzten)
            if (i < spieler_anzahl - 1 && Lichtschranke_Is_Card_Detected(4)) {
                M8_Rotate(steps_per_segment, speed_delay, direction);
                HAL_Delay(500);
            }
        }

        // Richtung für nächste Runde umdrehen (Snake)
        direction = !direction;

        // Feeder aus zwischen Runden
        HAL_GPIO_WritePin(M6_WelleUnten_IN3_GPIO_Port, M6_WelleUnten_IN3_Pin, GPIO_PIN_RESET);
        HAL_Delay(1000);
    }

    // Keine Karten mehr unten -> alles stoppen & System in Wartezustand
    All_Motors_Stop_Immediate();
    g_SystemState = SYSTEM_EMERGENCY_STOP;
}
#endif

