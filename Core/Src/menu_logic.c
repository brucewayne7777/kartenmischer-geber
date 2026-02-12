#include "menu_logic.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include <stdio.h> // for sprintf

extern TIM_HandleTypeDef htim8; // Access the timer defined in main.c

void Menu_Init(void) {
    ILI9341_Init();
    ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
    HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
}

// Helper: Checks if button is pressed (debounced)
// Returns 1 if pressed, 0 if not
uint8_t Menu_Is_Button_Pressed(void) {
    // ACTIVE LOW assumption (Pin is low when pressed)
    // PLACEHOLDER: If not wired yet, this will never be true unless we simulate it.
    // To simulate "auto-press" for testing without hardware, change return 0 to return 1;
    
    if (HAL_GPIO_ReadPin(ENC_SW_GPIO_Port, ENC_SW_Pin) == GPIO_PIN_RESET) {
        HAL_Delay(50); // Debounce
        if (HAL_GPIO_ReadPin(ENC_SW_GPIO_Port, ENC_SW_Pin) == GPIO_PIN_RESET) {
             // Wait for release
             while(HAL_GPIO_ReadPin(ENC_SW_GPIO_Port, ENC_SW_Pin) == GPIO_PIN_RESET);
             return 1;
        }
    }
    return 0;
}

int Menu_Select_Player_Count(void) {
    int count = 4;  // Start in der Mitte
    int old_count = -1;
    int16_t old_raw = -9999;
    
    // Reset counter to known value (Mitte des 16-bit Bereichs)
    __HAL_TIM_SET_COUNTER(&htim8, 32768);
    int16_t base_value = 32768;

    ILI9341_Fill_Screen(RED); // Initial fill RED for debug
    HAL_Delay(500);
    ILI9341_Fill_Screen(BLACK);
    ILI9341_Draw_Text("SPIELERANZAHL:", 60, 20, ORANGE, 2, BLACK);
    ILI9341_Draw_Text("Drehen & Druecken", 40, 200, WHITE, 2, BLACK);

    while(1) {
        // Read Encoder - signed difference from base
        int16_t raw = (int16_t)__HAL_TIM_GET_COUNTER(&htim8);
        int16_t diff = (raw - base_value) / 4;  // 4 Schritte pro Raste
        count = 4 + diff;  // 4 ist unser Startwert

        // Limits (2 to 8 players)
        if (count < 2) { 
            count = 2; 
            base_value = raw - ((-2) * 4);  // Recalibrate
        }
        if (count > 8) { 
            count = 8; 
            base_value = raw - ((4) * 4);  // Recalibrate
        }

        // DEBUG: Zeige Raw-Wert am unteren Rand
        if (raw != old_raw) {
            char dbg[25];
            sprintf(dbg, "RAW:%6d", raw);
            ILI9341_Draw_Filled_Rectangle_Coord(0, 220, 200, 240, BLACK);
            ILI9341_Draw_Text(dbg, 10, 222, RED, 1, BLACK);
            old_raw = raw;
        }

        // Update Screen
        if (count != old_count) {
             char buf[5];
             sprintf(buf, "%d", count);
             // Clear old number area (Box)
             ILI9341_Draw_Filled_Rectangle_Coord(100, 70, 220, 150, BLACK);
             // Draw new
             ILI9341_Draw_Text(buf, 130, 80, CYAN, 6, BLACK);
             old_count = count;
        }

        // Check Button
        if (Menu_Is_Button_Pressed()) {
             // Animation confirm
             ILI9341_Draw_Text("OK!", 130, 160, GREEN, 3, BLACK);
             HAL_Delay(1000);
             return count;
        }
        
        HAL_Delay(50);
    }
}

void Menu_Wait_For_Start(void) {
    ILI9341_Fill_Screen(BLACK);
    ILI9341_Draw_Text("BEREIT?", 80, 80, WHITE, 3, BLACK);
    ILI9341_Draw_Text("Druecke Start", 60, 140, GREEN, 2, BLACK);
    
    while(1) {
        if (Menu_Is_Button_Pressed()) {
            ILI9341_Fill_Screen(BLACK);
            return;
        }
        HAL_Delay(100);
    }
}

void Menu_Show_Message(char* line1, char* line2) {
    ILI9341_Fill_Screen(BLACK);
    if(line1) ILI9341_Draw_Text(line1, 20, 80, WHITE, 3, BLACK);
    if(line2) ILI9341_Draw_Text(line2, 20, 140, YELLOW, 2, BLACK);
}
