#include "lichtschranke_logic.h"
#include "main.h"

/* 0 = Standard: LOW = Karte (Reflexion), HIGH = keine Karte -> M1 nur bei Karte
 * 1 = Invertiert: HIGH = Karte
 */
#define LICHTSCHRANKE_INVERT 0

/**
 * Returns 1 if card detected, 0 if not.
 */
uint8_t Lichtschranke_Is_Card_Detected(int sensor_index)
{
    GPIO_TypeDef *port = NULL;
    uint16_t pin = 0;

    switch (sensor_index) {
        case 1:
            port = Lichtschranke_1_Sig_Pull_Up_GPIO_Port;
            pin  = Lichtschranke_1_Sig_Pull_Up_Pin;
            break;
        case 2:
            port = Lichtschranke_2_Sig_Pull_Up_GPIO_Port;
            pin  = Lichtschranke_2_Sig_Pull_Up_Pin;
            break;
        case 3:
            port = Lichtschranke_3_Sig_Pull_Up_GPIO_Port;
            pin  = Lichtschranke_3_Sig_Pull_Up_Pin;
            break;
        case 4:
            port = Lichtschranke_4_Sig_Pull_Up_GPIO_Port;
            pin  = Lichtschranke_4_Sig_Pull_Up_Pin;
            break;
        default:
            return 0;
    }

#if LICHTSCHRANKE_INVERT
    if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET)
        return 1;  /* HIGH = card detected (invertiert) */
    return 0;
#else
    if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET)
        return 1;  /* LOW = card detected */
    return 0;
#endif
}

/**
 * M1 forward: IN1=SET, IN2=RESET
 * M1 stop:    IN1=RESET, IN2=RESET
 */
void M1_Start(void)
{
    HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);
}

void M1_Stop(void)
{
    HAL_GPIO_WritePin(M1_SchieberOben_IN1_M1_DIR_GPIO_Port, M1_SchieberOben_IN1_M1_DIR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(M1_SchieberOben_IN2_M1_DIR_GPIO_Port, M1_SchieberOben_IN2_M1_DIR_Pin, GPIO_PIN_RESET);
}

/**
 * Test: card at Lichtschranke_1 -> M1 runs; no card -> M1 stops.
 * Runs for duration_ms milliseconds.
 */
void Lichtschranke_M1_Test_Run(uint32_t duration_ms)
{
    uint32_t start = HAL_GetTick();

    while ((HAL_GetTick() - start) < duration_ms) {
        if (Lichtschranke_Is_Card_Detected(1))
            M1_Start();
        else
            M1_Stop();
        HAL_Delay(20);  /* Poll every 20 ms */
    }

    M1_Stop();
}
