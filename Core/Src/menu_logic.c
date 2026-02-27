#include "menu_logic.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "5x5_font.h"
#include <stdio.h>
#include <string.h>

extern TIM_HandleTypeDef htim8; // Access the timer defined in main.c

// Casino/Felt Theme Colors
#define FELT_GREEN   0x1A4A  // dark green approximation
#define GOLD_YELLOW  0xFEA0  // approx. 0xFFD700 in 16-bit
#define CASINO_RED   RED

static uint16_t CenterX(const char* text, uint16_t size)
{
    uint16_t len = (uint16_t)strlen(text);
    uint16_t w = len * CHAR_WIDTH * size;
    if (w >= ILI9341_SCREEN_WIDTH) return 0;
    return (ILI9341_SCREEN_WIDTH - w) / 2;
}

// --- Simple suit icon helpers (drawn inside a small card) ---
static void Draw_HeartIcon(int cx, int cy, uint16_t colour)
{
    // two circles oben + kleiner "Kegel" nach unten
    ILI9341_Draw_Filled_Circle(cx - 4, cy - 2, 5, colour);
    ILI9341_Draw_Filled_Circle(cx + 4, cy - 2, 5, colour);
    ILI9341_Draw_Filled_Rectangle_Coord(cx - 6, cy - 2, cx + 6, cy + 10, colour);
}

static void Draw_DiamondIcon(int cx, int cy, uint16_t colour)
{
    // einfacher Rhombus aus gestuften Rechtecken
    ILI9341_Draw_Filled_Rectangle_Coord(cx - 2, cy - 8, cx + 2, cy + 8, colour);
    ILI9341_Draw_Filled_Rectangle_Coord(cx - 4, cy - 6, cx + 4, cy + 6, colour);
}

static void Draw_ClubIcon(int cx, int cy, uint16_t colour)
{
    // drei Kreise + kleiner Stiel
    ILI9341_Draw_Filled_Circle(cx,     cy - 5, 4, colour);
    ILI9341_Draw_Filled_Circle(cx - 5, cy + 1, 4, colour);
    ILI9341_Draw_Filled_Circle(cx + 5, cy + 1, 4, colour);
    ILI9341_Draw_Filled_Rectangle_Coord(cx - 2, cy + 4, cx + 2, cy + 12, colour);
}

static void Draw_SpadeIcon(int cx, int cy, uint16_t colour)
{
    // Herz-Form nach oben gespiegelt + Stiel
    ILI9341_Draw_Filled_Circle(cx - 4, cy + 2, 5, colour);
    ILI9341_Draw_Filled_Circle(cx + 4, cy + 2, 5, colour);
    ILI9341_Draw_Filled_Rectangle_Coord(cx - 6, cy - 10, cx + 6, cy + 2, colour);
    ILI9341_Draw_Filled_Rectangle_Coord(cx - 2, cy + 4, cx + 2, cy + 12, colour);
}

static void Draw_Startup_Screen(void) {
    ILI9341_Fill_Screen(FELT_GREEN);
    // Title zentriert
    uint16_t title_x = CenterX("KARTENMISCHER", 2);
    ILI9341_Draw_Text("KARTENMISCHER", title_x, 20, GOLD_YELLOW, 2, FELT_GREEN);

    // Card symbols row: 4 Poker-Symbole in Schwarz/Rot abwechselnd, zentriert
    int card_w = 40;
    int gap = 15;
    int total_w = 4 * card_w + 3 * gap;
    int x = (int)((ILI9341_SCREEN_WIDTH - total_w) / 2);
    for (int i = 0; i < 4; i++) {
        int cx = x + 20;
        int cy = 100;

        // Kartenrahmen
        ILI9341_Draw_Filled_Rectangle_Coord(x, 70, x+40, 130, WHITE);
        ILI9341_Draw_Hollow_Rectangle_Coord(x, 70, x+40, 130, GOLD_YELLOW);

        // Suit-Icon
        switch (i) {
            case 0: // Pik (schwarz)
                Draw_SpadeIcon(cx, cy, BLACK);
                break;
            case 1: // Herz (rot)
                Draw_HeartIcon(cx, cy, CASINO_RED);
                break;
            case 2: // Kreuz (schwarz)
                Draw_ClubIcon(cx, cy, BLACK);
                break;
            case 3: // Karo (rot)
                Draw_DiamondIcon(cx, cy, CASINO_RED);
                break;
        }

        x += 55;
    }

    // Blinking hint text (Startzustand: sichtbar, zentriert)
    uint16_t hint_x = CenterX("Druecke Start", 2);
    ILI9341_Draw_Text("Druecke Start", hint_x, 200, WHITE, 2, FELT_GREEN);

    // Namen der Projektpartner zentriert im Footer
    uint16_t name1_x = CenterX("Sudhaavan Kumaran", 1);
    uint16_t name2_x = CenterX("Ali Ayari", 1);
    ILI9341_Draw_Text("Sudhaavan Kumaran", name1_x, 220, GOLD_YELLOW, 1, FELT_GREEN);
    ILI9341_Draw_Text("Ali Ayari",         name2_x, 230, GOLD_YELLOW, 1, FELT_GREEN);
}

void Menu_Init(void) {
    ILI9341_Init();
    ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
    HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);

    // Startup / Waiting Screen (static); blinking will be in loop
    Draw_Startup_Screen();
}

// Vollständiger Display-Reset + Re-Initialisierung mit kurzer Statusanzeige
void Display_Reinit(void) {
    // Hardware-Reset des ILI9341
    ILI9341_Reset();

    // Standard-Initialisierung (Rotation + Encoder)
    Menu_Init();

    // Kurze Rueckmeldung im neuen Theme
    ILI9341_Fill_Screen(FELT_GREEN);
    ILI9341_Draw_Text("Display Reset", 40, 100, GOLD_YELLOW, 2, FELT_GREEN);
}

// Helper: Checks if button is pressed (debounced)
// Returns 1 if pressed, 0 if not
uint8_t Menu_Is_Button_Pressed(void) {
    if (HAL_GPIO_ReadPin(ENC_SW_GPIO_Port, ENC_SW_Pin) == GPIO_PIN_RESET) {
        HAL_Delay(50); // Debounce
        if (HAL_GPIO_ReadPin(ENC_SW_GPIO_Port, ENC_SW_Pin) == GPIO_PIN_RESET) {
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

    // Background
    ILI9341_Fill_Screen(FELT_GREEN);
    uint16_t title_x = CenterX("SPIELERANZAHL", 2);
    uint16_t hint_x  = CenterX("Drehen & Druecken", 2);
    ILI9341_Draw_Text("SPIELERANZAHL", title_x, 10, GOLD_YELLOW, 2, FELT_GREEN);
    ILI9341_Draw_Text("Drehen & Druecken", hint_x, 200, WHITE, 2, FELT_GREEN);

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
            ILI9341_Draw_Filled_Rectangle_Coord(0, 220, 200, 240, FELT_GREEN);
            ILI9341_Draw_Text(dbg, 10, 222, GOLD_YELLOW, 1, FELT_GREEN);
            old_raw = raw;
        }

        // Update Screen: big center number + card backs row
        if (count != old_count) {
             char buf[5];
             sprintf(buf, "%d", count);
             // Clear number area
             ILI9341_Draw_Filled_Rectangle_Coord(80, 50, 240, 150, FELT_GREEN);
             uint16_t num_x = CenterX(buf, 6);
             ILI9341_Draw_Text(buf, num_x, 70, WHITE, 6, FELT_GREEN);

             // Card-back icons row: simple rectangles, etwas hoeher positioniert und zentriert
             ILI9341_Draw_Filled_Rectangle_Coord(20, 150, 300, 190, FELT_GREEN);
             int card_w = 18;
             int gap = 7;
             int total_w = count * card_w + (count - 1) * gap;
             int x = (int)((ILI9341_SCREEN_WIDTH - total_w) / 2);
             for (int i = 0; i < count; i++) {
                 ILI9341_Draw_Filled_Rectangle_Coord(x, 155, x+18, 185, GOLD_YELLOW);
                 ILI9341_Draw_Hollow_Rectangle_Coord(x+2, 157, x+16, 183, FELT_GREEN);
                 x += 25;
             }
             old_count = count;
        }

        // Check Button
        if (Menu_Is_Button_Pressed()) {
             ILI9341_Draw_Text("OK!", 130, 160, GOLD_YELLOW, 3, FELT_GREEN);
             HAL_Delay(800);
             return count;
        }
        
        HAL_Delay(50);
    }
}

void Menu_Wait_For_Start(void) {
    // Reuse startup screen background but add blinking prompt
    Draw_Startup_Screen();
    
    uint8_t visible = 1;
    while(1) {
        if (Menu_Is_Button_Pressed()) {
            ILI9341_Fill_Screen(FELT_GREEN);
            return;
        }

        // Simple blink on/off for "Druecke Start"
        if (visible) {
            uint16_t hint_x = CenterX("Druecke Start", 2);
            ILI9341_Draw_Text("Druecke Start", hint_x, 200, WHITE, 2, FELT_GREEN);
        } else {
            ILI9341_Draw_Filled_Rectangle_Coord(70, 200, 260, 220, FELT_GREEN);
        }
        visible = !visible;
        HAL_Delay(400);
    }
}

void Menu_Show_Message(char* line1, char* line2) {
    ILI9341_Fill_Screen(FELT_GREEN);
    if(line1) {
        uint16_t x1 = CenterX(line1, 3);
        ILI9341_Draw_Text(line1, x1, 80, GOLD_YELLOW, 3, FELT_GREEN);
    }
    if(line2) {
        uint16_t x2 = CenterX(line2, 2);
        ILI9341_Draw_Text(line2, x2, 140, WHITE, 2, FELT_GREEN);
    }
}
