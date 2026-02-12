#ifndef INC_LICHTSCHRANKE_LOGIC_H_
#define INC_LICHTSCHRANKE_LOGIC_H_

#include "main.h"

/**
 * Sensor index: 1..4 (Lichtschranke_1 = PA2, _2 = PA3, _3 = PA6, _4 = PC0)
 * Returns 1 if card/reflection detected (pin LOW), 0 if no card (pin HIGH).
 */
uint8_t Lichtschranke_Is_Card_Detected(int sensor_index);

void M1_Start(void);
void M1_Stop(void);

/**
 * Test on breadboard: run M1 while Lichtschranke_1 detects card, stop M1 when no card.
 * Runs for duration_ms milliseconds (e.g. 60000 = 60 s).
 * Uses sensor 1 (PA2) only.
 */
void Lichtschranke_M1_Test_Run(uint32_t duration_ms);

#endif /* INC_LICHTSCHRANKE_LOGIC_H_ */
