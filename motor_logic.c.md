#include "motor_logic.h"
#include "main.h"

// ============================================================
// PHASE 1: Sortieren (M2/M3 konstant, M1 kommt später dazu)
// ============================================================
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
    // Annahme: Ein Zyklus (Hin+Zurück) dauert ca. 1 Sekunde (400ms hin + 400ms zurück + Pausen)
    // Wir machen das jetzt zeitgesteuert für 50 Sekunden.
    
    uint32_t start_time = HAL_GetTick();
    while ((HAL_GetTick() - start_time) < 50000) // 50 Sekunden lang
    {
        // M1: Rechts (Vorwärts) -> Test: IN1=SET, IN2=RESET
        HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_Delay(2000); // Schiebedauer

        // M1: Links (Rückwärts) -> Test: IN1=RESET, IN2=SET
        HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_SET);
        HAL_Delay(2000); // Rückfahrtdauer
        
        // Kurze Pause (optional, damit der Motor nicht heiß läuft)
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


// ============================================================
// PHASE 2: Transport zur Mitte (M4/M5 zuerst, dann M2/M3 dazu)
// ============================================================
void Phase2_Transport_1Minute(void)
{
    // 1. M4 und M5 SOFORT Starten (Schieber zur Mitte)
    HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_SET);

    // 2. Warte 10 Sekunden (nur M4/M5 laufen)
    HAL_Delay(10000);

    // 3. M2 und M3 zuschalten (Richtung GETAUSCHT gegenüber Phase 1)
    // Phase 1 war: M2 (RESET/SET), M3 (RESET/SET)
    // Jetzt neu:   M2 (SET/RESET), M3 (SET/RESET) -> Andere Richtung
    
    // M2 (Andersrum)
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);

    // M3 (Andersrum)
    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);

    // 4. Alle zusammen laufen lassen für den Rest der Minute (50 Sek)
    HAL_Delay(50000);

    // 5. Minute vorbei -> ALLES STOPPEN
    HAL_GPIO_WritePin(M2_WelleLinks_IN3_GPIO_Port, M2_WelleLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M2_WelleLinks_IN4_GPIO_Port, M2_WelleLinks_IN4_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN1_GPIO_Port, M3_WelleRechts_IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M3_WelleRechts_IN2_GPIO_Port, M3_WelleRechts_IN2_Pin, GPIO_PIN_RESET);
    
    HAL_GPIO_WritePin(M4_SchieberLinks_IN3_GPIO_Port, M4_SchieberLinks_IN3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M5_SchieberRechts_IN1_GPIO_Port, M5_SchieberRechts_IN1_Pin, GPIO_PIN_RESET);
}


// ============================================================
// PHASE 3: Auswurf (Platzhalter / Deaktiviert)
// ============================================================
void Phase3_Auswurf(void)
{
    // Vorerst deaktiviert wie gewünscht
}
