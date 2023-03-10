/**
 * LCD Library Header: ILI9341_Lib.h
 * uP2 - Fall 2022
 */

#ifndef BOARDSUPPORT_INC_ILI9341_LIB_H_
#define BOARDSUPPORT_INC_ILI9341_LIB_H_

#include <stdbool.h>
#include <stdint.h>
/************************************ Defines *******************************************/

/* Screen size */
#define MAX_SCREEN_X     320
#define MAX_SCREEN_Y     240
#define MIN_SCREEN_X     0
#define MIN_SCREEN_Y     0
#define SCREEN_SIZE      76800

/* XPT2046 registers definition for X and Y coordinate retrieval */
#define CHX         0x90
#define CHY         0xD0

/* LCD colors */
#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F

/* LCD colors */
#define LCD_WHITE          0xFFFF
#define LCD_BLACK          0x0000
#define LCD_BLUE           0x0197
#define LCD_RED            0xF800
#define LCD_MAGENTA        0xF81F
#define LCD_GREEN          0x07E0
#define LCD_CYAN           0x7FFF
#define LCD_YELLOW         0xFFE0
#define LCD_GRAY           0x2104
#define LCD_PURPLE         0xF11F
#define LCD_ORANGE         0xFD20
#define LCD_PINK           0xfdba
#define LCD_OLIVE          0xdfe4

/* ILI 9341 registers definition */


/************************************ Defines *******************************************/

/********************************** Structures ******************************************/
typedef struct Point {
    uint16_t x;
    uint16_t y;
}Point;
/********************************** Structures ******************************************/

/************************************ Public Functions  *******************************************/

/*******************************************************************************
 * Function Name  : LCD_DrawRectangle
 * Description    : Draw a rectangle as the specified color
 * Input          : x, y, w, h, color
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
void LCD_DrawRectangle(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color);

/******************************************************************************
* Function Name  : PutChar
* Description    : Lcd screen displays a character
* Input          : - Xpos: Horizontal coordinate
*                  - Ypos: Vertical coordinate
*                  - ASCI: Displayed character
*                  - charColor: Character color
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
inline void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor);

/******************************************************************************
* Function Name  : LCD_Text
* Description    : Displays the string
* Input          : - Xpos: Horizontal coordinate
*                  - Ypos: Vertical coordinate
*                  - str: Displayed string
*                  - charColor: Character color
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color);

/*******************************************************************************
* Function Name  : LCD_Write_Data_Only
* Description    : Data writing to the LCD controller
* Input          : - data: data to be written
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
inline void LCD_Write_Data_Only(uint8_t data);

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : Fill the screen as the specified color
* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_Clear(uint16_t Color);

/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : Drawn at a specified point coordinates
* Input          : - Xpos: Row Coordinate
*                  - Ypos: Line Coordinate
* Output         : None
* Return         : None
* Attention      : 18N Bytes Written
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos, uint16_t Ypos, uint16_t color);

/*******************************************************************************
* Function Name  : LCD_WriteData
* Description    : LCD write register data
* Input          : - data: register data
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
inline void LCD_WriteData(uint8_t data);


/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
* Attention      : None
*******************************************************************************/
inline uint8_t LCD_ReadReg(uint8_t LCD_reg);

/*******************************************************************************
* Function Name  : LCD_WriteIndex
* Description    : LCD write register address
* Input          : - index: register address
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
inline void LCD_WriteIndex(uint8_t index);

/*******************************************************************************
 * Function Name  : SPISendRecvTPByte
 * Description    : Send one byte then receive one byte of response from Touchpanel
 * Input          : uint8_t: byte
 * Output         : None
 * Return         : None
 * Attention      : None
 *******************************************************************************/
inline uint8_t SPISendRecvTPByte (uint8_t byte);

/*******************************************************************************
* Function Name  : SPISendRecvByte
* Description    : Send one byte then recv one byte of response
* Input          : uint8_t: byte
* Output         : None
* Return         : Recieved value
* Attention      : None
*******************************************************************************/
inline uint8_t SPISendRecvByte(uint8_t byte);

/*******************************************************************************
* Function Name  : LCD_Write_Data_Start
* Description    : Start of data writing to the LCD controller
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
inline void LCD_Write_Data_Start(void);

/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    : LCD read data
* Input          : None
* Output         : None
* Return         : return data
* Attention  : None
*******************************************************************************/
inline uint16_t LCD_ReadData();

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
inline void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
inline void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos );

/*******************************************************************************
* Function Name  : LCD_Init
* Description    : Configures LCD Control lines, sets whole screen black
* Input          : bool usingTP: determines whether or not to enable TP interrupt
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_Init(bool usingTP);

/*******************************************************************************
* Function Name  : LCD_SetAddress
* Description    : Sets the draw area of the LCD
* Input          : uin16_t x1, y1, x2, y2: Represents the start and end LCD address for drawing
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);

/*******************************************************************************
* Function Name  : LCD_PushColor
* Description    : Sets a pixel on the LCD to a color
* Input          : uint16_t color: 16 bit value of the color to output
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_PushColor(uint16_t color);

/*******************************************************************************
 * Function Name  : TP_ReadXY
 * Description    : Obtain X and Y touch coordinates
 * Input          : None
 * Output         : None
 * Return         : Point structure
 * Attention      : None
 *******************************************************************************/
Point TP_ReadXY();

/*******************************************************************************
 * Function Name  : TP_ReadX
 * Description    : Obtain X touch coordinate
 * Input          : None
 * Output         : None
 * Return         : X Coordinate
 * Attention      : None
 *******************************************************************************/
uint16_t TP_ReadX();

/*******************************************************************************
 * Function Name  : TP_ReadY
 * Description    : Obtain Y touch coordinate
 * Input          : None
 * Output         : None
 * Return         : Y Coordinate
 * Attention      : None
 *******************************************************************************/
uint16_t TP_ReadY();

/************************************ Public Functions  *******************************************/




#endif /* BOARDSUPPORT_INC_ILI9341_LIB_H_ */
