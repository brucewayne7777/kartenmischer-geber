#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "5x5_font.h"
#include "main.h" // Wichtig!

// SPI Handle (aus main.c)
extern SPI_HandleTypeDef hspi1;

/*Draw hollow circle at X,Y location with specified radius and colour*/
void ILI9341_Draw_Hollow_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
    int x = Radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (Radius << 1);

    while (x >= y)
    {
        ILI9341_Draw_Pixel(X + x, Y + y, Colour);
        ILI9341_Draw_Pixel(X + y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - y, Y + x, Colour);
        ILI9341_Draw_Pixel(X - x, Y + y, Colour);
        ILI9341_Draw_Pixel(X - x, Y - y, Colour);
        ILI9341_Draw_Pixel(X - y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + y, Y - x, Colour);
        ILI9341_Draw_Pixel(X + x, Y - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-Radius << 1) + dx;
        }
    }
}

/*Draw filled circle at X,Y location with specified radius and colour*/
void ILI9341_Draw_Filled_Circle(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Colour)
{
    int x = Radius;
    int y = 0;
    int xChange = 1 - (Radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = X - x; i <= X + x; i++)
        {
            ILI9341_Draw_Pixel(i, Y + y,Colour);
            ILI9341_Draw_Pixel(i, Y - y,Colour);
        }
        for (int i = X - y; i <= X + y; i++)
        {
            ILI9341_Draw_Pixel(i, Y + x,Colour);
            ILI9341_Draw_Pixel(i, Y - x,Colour);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

/*Draw a hollow rectangle*/
void ILI9341_Draw_Hollow_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
    uint16_t    X_length = 0;
    uint16_t    Y_length = 0;
    uint8_t     Negative_X = 0;
    uint8_t     Negative_Y = 0;
    float       Calc_Negative = 0;

    Calc_Negative = X1 - X0;
    if(Calc_Negative < 0) Negative_X = 1;
    Calc_Negative = 0;

    Calc_Negative = Y1 - Y0;
    if(Calc_Negative < 0) Negative_Y = 1;

    if(!Negative_X) X_length = X1 - X0;
    else X_length = X0 - X1;

    ILI9341_Draw_Horizontal_Line(X0, Y0, X_length, Colour);
    ILI9341_Draw_Horizontal_Line(X0, Y1, X_length, Colour);

    if(!Negative_Y) Y_length = Y1 - Y0;
    else Y_length = Y0 - Y1;

    ILI9341_Draw_Vertical_Line(X0, Y0, Y_length, Colour);
    ILI9341_Draw_Vertical_Line(X1, Y0, Y_length, Colour);

    if((X_length > 0)||(Y_length > 0)) ILI9341_Draw_Pixel(X1, Y1, Colour);
}

/*Draw a filled rectangle*/
void ILI9341_Draw_Filled_Rectangle_Coord(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Colour)
{
    uint16_t    X_length = 0;
    uint16_t    Y_length = 0;
    uint8_t     Negative_X = 0;
    uint8_t     Negative_Y = 0;
    int32_t     Calc_Negative = 0;

    uint16_t X0_true = 0;
    uint16_t Y0_true = 0;

    Calc_Negative = X1 - X0;
    if(Calc_Negative < 0) Negative_X = 1;

    Calc_Negative = Y1 - Y0;
    if(Calc_Negative < 0) Negative_Y = 1;

    if(!Negative_X) { X_length = X1 - X0; X0_true = X0; }
    else { X_length = X0 - X1; X0_true = X1; }

    if(!Negative_Y) { Y_length = Y1 - Y0; Y0_true = Y0; }
    else { Y_length = Y0 - Y1; Y0_true = Y1; }

    ILI9341_Draw_Rectangle(X0_true, Y0_true, X_length, Y_length, Colour);
}

/*Draws a character*/
void ILI9341_Draw_Char(char Character, uint8_t X, uint8_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour) 
{
    uint8_t     function_char;
    uint8_t     i,j;

    function_char = Character;
    if (function_char < ' ') Character = 0;
    else function_char -= 32;

    char temp[CHAR_WIDTH];
    for(uint8_t k = 0; k<CHAR_WIDTH; k++)
    {
        temp[k] = font[function_char][k];
    }

    ILI9341_Draw_Rectangle(X, Y, CHAR_WIDTH*Size, CHAR_HEIGHT*Size, Background_Colour);

    for (j=0; j<CHAR_WIDTH; j++) {
        for (i=0; i<CHAR_HEIGHT; i++) {
            if (temp[j] & (1<<i)) {
                if(Size == 1) ILI9341_Draw_Pixel(X+j, Y+i, Colour);
                else ILI9341_Draw_Rectangle(X+(j*Size), Y+(i*Size), Size, Size, Colour);
            }
        }
    }
}

/*Draws an array of characters*/
void ILI9341_Draw_Text(const char* Text, uint8_t X, uint8_t Y, uint16_t Colour, uint16_t Size, uint16_t Background_Colour)
{
    while (*Text) {
        ILI9341_Draw_Char(*Text++, X, Y, Colour, Size, Background_Colour);
        X += CHAR_WIDTH*Size;
    }
}

/*Draws a full screen picture from flash*/
void ILI9341_Draw_Image(const char* Image_Array, uint8_t Orientation)
{
    // Hier verwenden wir deine echten Pin-Definitionen!

    // Rotation setzen
    ILI9341_Set_Rotation(Orientation);

    // Address Window setzen (je nach Orientierung Breite/Höhe tauschen)
    if(Orientation == SCREEN_HORIZONTAL_1 || Orientation == SCREEN_HORIZONTAL_2) {
         ILI9341_Set_Address(0,0,ILI9341_SCREEN_WIDTH,ILI9341_SCREEN_HEIGHT);
    } else {
         ILI9341_Set_Address(0,0,ILI9341_SCREEN_HEIGHT,ILI9341_SCREEN_WIDTH);
    }

    // DC auf High (Data)
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
    // CS auf Low (Select)
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

    unsigned char Temp_small_buffer[BURST_MAX_SIZE];
    uint32_t counter = 0;
    // Größe berechnen: 320*240*2 Bytes
    uint32_t total_pixels = ILI9341_SCREEN_WIDTH * ILI9341_SCREEN_HEIGHT * 2;

    for(uint32_t i = 0; i < total_pixels / BURST_MAX_SIZE; i++)
    {
            for(uint32_t k = 0; k < BURST_MAX_SIZE; k++)
            {
                Temp_small_buffer[k] = Image_Array[counter+k];
            }
            // KORRIGIERT: hspi1 statt hspi5
            HAL_SPI_Transmit(&hspi1, (unsigned char*)Temp_small_buffer, BURST_MAX_SIZE, 10);
            counter += BURST_MAX_SIZE;
    }
    // CS auf High (Deselect)
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
