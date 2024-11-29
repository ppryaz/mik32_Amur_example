/******************************************************************************
* | File      	:   GUI_Paint.h
* | Author      :   Waveshare electronics
* | Function    :	Achieve drawing: draw points, lines, boxes, circles and
*                   their size, solid dotted line, solid rectangle hollow
*                   rectangle, solid circle hollow circle.
* | Info        :
*   Achieve display characters: Display a single character, string, number
*   Achieve time display: adaptive size display time minutes and seconds
*----------------
* |	This version:   V3.2
* | Date        :   2020-08-18
* | Info        :
* -----------------------------------------------------------------------------
* V3.2(2020-08-18):
* 1.Change: Paint_SetScale(unsigned char scale)
*        Add scale 65K
* 2.Change: Paint_SetPixel(unsigned int Xpoint, unsigned int Ypoint, unsigned int Color)
*        Add the branch for scale 65K
* 3.Change: Paint_Clear(unsigned int Color)
*        Add the branch for scale 65K
* -----------------------------------------------------------------------------
* V3.1(2020-08-14):
* 1.Change: Paint_SetScale(unsigned char scale)
*        Add scale 16
* 2.Change: Paint_SetPixel(unsigned int Xpoint, unsigned int Ypoint, unsigned int Color)
*        Add the branch for scale 16
* 3.Change: Paint_Clear(unsigned int Color)
*        Add the branch for scale 16
* -----------------------------------------------------------------------------
* V3.0(2019-04-18):
* 1.Change: 
*    Paint_DrawPoint(..., DOT_STYLE DOT_STYLE)
* => Paint_DrawPoint(..., DOT_STYLE Dot_Style)
*    Paint_DrawLine(..., LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel)
* => Paint_DrawLine(..., DOT_PIXEL Line_width, LINE_STYLE Line_Style)
*    Paint_DrawRectangle(..., DRAW_FILL Filled, DOT_PIXEL Dot_Pixel)
* => Paint_DrawRectangle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
*    Paint_DrawCircle(..., DRAW_FILL Draw_Fill, DOT_PIXEL Dot_Pixel)
* => Paint_DrawCircle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Filll)
*
* -----------------------------------------------------------------------------
* V2.0(2018-11-15):
* 1.add: Paint_NewImage()
*    Create an image's properties
* 2.add: Paint_SelectImage()
*    Select the picture to be drawn
* 3.add: Paint_SetRotate()
*    Set the direction of the cache    
* 4.add: Paint_RotateImage() 
*    Can flip the picture, Support 0-360 degrees, 
*    but only 90.180.270 rotation is better
* 4.add: Paint_SetMirroring() 
*    Can Mirroring the picture, horizontal, vertical, origin
* 5.add: Paint_DrawString_CN() 
*    Can display Chinese(GB1312)   
*
* ----------------------------------------------------------------------------- 
* V1.0(2018-07-17):
*   Create library
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documnetation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to  whom the Software is
* furished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
******************************************************************************/
#ifndef __GUI_PAINT_H
#define __GUI_PAINT_H

#include "fonts.h"

/**
 * Image attributes
**/
typedef struct {
    unsigned char *Image;
    unsigned int Width;
    unsigned int Height;
    unsigned int WidthMemory;
    unsigned int HeightMemory;
    unsigned int Color;
    unsigned int Rotate;
    unsigned int Mirror;
    unsigned int WidthByte;
    unsigned int HeightByte;
    unsigned int Scale;
} PAINT;
extern PAINT Paint;

/**
 * Display rotate
**/
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270

/**
 * Display Flip
**/
typedef enum {
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
} MIRROR_IMAGE;
#define MIRROR_IMAGE_DFT MIRROR_NONE

/**
 * image color
**/
/**
 * image color
**/
#define WHITE          0xFFFF
#define BLACK          0x0000
#define BLUE           0x001F
#define BRED           0XF81F
#define GRED           0XFFE0
#define GBLUE          0X07FF
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0
#define BROWN          0XBC40
#define BRRED          0XFC07
#define GRAY           0X8430

#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE

//4 Gray level
#define  GRAY1 0x03 //Blackest
#define  GRAY2 0x02
#define  GRAY3 0x01 //gray
#define  GRAY4 0x00 //white
/**
 * The size of the point
**/
typedef enum {
    DOT_PIXEL_1X1  = 1,	// 1 x 1
    DOT_PIXEL_2X2  , 		// 2 X 2
    DOT_PIXEL_3X3  ,		// 3 X 3
    DOT_PIXEL_4X4  ,		// 4 X 4
    DOT_PIXEL_5X5  , 		// 5 X 5
    DOT_PIXEL_6X6  , 		// 6 X 6
    DOT_PIXEL_7X7  , 		// 7 X 7
    DOT_PIXEL_8X8  , 		// 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

/**
 * Point size fill style
**/
typedef enum {
    DOT_FILL_AROUND  = 1,		// dot pixel 1 x 1
    DOT_FILL_RIGHTUP  , 		// dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

/**
 * Line style, solid or dashed
**/
typedef enum {
    LINE_STYLE_SOLID = 0,
    LINE_STYLE_DOTTED,
} LINE_STYLE;

/**
 * Whether the graphic is filled
**/
typedef enum {
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL,
} DRAW_FILL;

/**
 * Custom structure of a time attribute
**/
typedef struct {
    unsigned int	Year;  //0000
    unsigned char Month; //1 - 12
    unsigned char Day;   //1 - 30
    unsigned char Hour;  //0 - 23
    unsigned char Min;   //0 - 59
    unsigned char Sec;   //0 - 59
} PAINT_TIME;
extern PAINT_TIME sPaint_time;

//init and Clear
void Paint_NewImage(unsigned char *image, unsigned int Width, unsigned int Height, unsigned int Rotate, unsigned int Color);
void Paint_SelectImage(unsigned char *image);
void Paint_SetRotate(unsigned int Rotate);
void Paint_SetMirroring(unsigned char mirror);
void Paint_SetPixel(unsigned int Xpoint, unsigned int Ypoint, unsigned int Color);
void Paint_SetScale(unsigned char scale);

void Paint_Clear(unsigned int Color);
void Paint_ClearWindows(unsigned int Xstart, unsigned int Ystart, unsigned int Xend, unsigned int Yend, unsigned int Color);

//Drawing
void Paint_DrawPoint(unsigned int Xpoint, unsigned int Ypoint, unsigned int Color, DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay);
void Paint_DrawLine(unsigned int Xstart, unsigned int Ystart, unsigned int Xend, unsigned int Yend, unsigned int Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style);
void Paint_DrawRectangle(unsigned int Xstart, unsigned int Ystart, unsigned int Xend, unsigned int Yend, unsigned int Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill);
void Paint_DrawCircle(unsigned int X_Center, unsigned int Y_Center, unsigned int Radius, unsigned int Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill);

//Display string
void Paint_DrawChar(unsigned int Xstart, unsigned int Ystart, const char Acsii_Char, sFONT* Font, unsigned int Color_Foreground, unsigned int Color_Background);
void Paint_DrawString_EN(unsigned long Xstart, unsigned long Ystart, const char * pString, sFONT* Font, unsigned long Color_Foreground, unsigned long Color_Background);
void Paint_DrawString_CN(unsigned int Xstart, unsigned int Ystart, const char * pString, cFONT* font, unsigned int Color_Foreground, unsigned int Color_Background);
void Paint_DrawNum(unsigned long Xpoint, unsigned long Ypoint, double Nummber, sFONT* Font, unsigned long Digit,unsigned long Color_Foreground, unsigned long Color_Background);
void Paint_DrawTime(unsigned int Xstart, unsigned int Ystart, PAINT_TIME *pTime, sFONT* Font, unsigned int Color_Foreground, unsigned int Color_Background);

//pic
void Paint_DrawBitMap(const unsigned char* image_buffer);
//void Paint_DrawBitMap_Half(const unsigned char* image_buffer, unsigned char Region);
//void Paint_DrawBitMap_OneQuarter(const unsigned char* image_buffer, unsigned char Region);
//void Paint_DrawBitMap_OneEighth(const unsigned char* image_buffer, unsigned char Region);
void Paint_DrawBitMap_Block(const unsigned char* image_buffer, unsigned char Region);
#endif




