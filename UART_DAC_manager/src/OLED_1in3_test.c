/*****************************************************************************
* | File      	:   OLED_1in3_test.c
* | Author      :   Waveshare team
* | Function    :   1.3inch OLED test demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-08-13
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "test.h"
#include "OLED_1in3.h"

//#define Imagesize = ((OLED_1IN3_WIDTH%8==0)? (OLED_1IN3_WIDTH/8): (OLED_1IN3_WIDTH/8+1)) * OLED_1IN3_HEIGHT;
	unsigned char BlackImage[((OLED_1IN3_WIDTH%8==0)? (OLED_1IN3_WIDTH/8): (OLED_1IN3_WIDTH/8+1)) * OLED_1IN3_HEIGHT];
extern unsigned char _acmikron_logo[];
extern unsigned char _acMIK32[];

int OLED_1in3_test(void)
{
	//printf("1.3inch OLED test demo\n");
//	if(System_Init() != 0) {
//		return -1;
//	}
	iic_init();
	
	//printf("OLED Init...\r\n");
	OLED_1IN3_Init();
	Driver_Delay_ms(500);
	// 0.Create a new image cache

	//printf("Paint_NewImage\r\n");
	Paint_NewImage(BlackImage, OLED_1IN3_WIDTH, OLED_1IN3_HEIGHT, 90, BLACK);	

	//printf("Drawing\r\n");
	//1.Select Image
	Paint_SelectImage(BlackImage);
	Driver_Delay_ms(500);
	Paint_Clear(BLACK);
	while(1) {
		
		OLED_1IN3_Display(_acmikron_logo);
//		Driver_Delay_ms(5000);
//		Paint_Clear(BLACK);
		Driver_Delay_ms(2000);

		OLED_1IN3_Display(_acMIK32);
		Driver_Delay_ms(5000);
//		Paint_Clear(BLACK);
//		Driver_Delay_ms(2000);

		// 2.Drawing on the image		
		//printf("Drawing:page 1\r\n");
		Paint_DrawPoint(20, 10, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
		Paint_DrawPoint(30, 10, WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
		Paint_DrawPoint(40, 10, WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
		Paint_DrawLine(10, 10, 10, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(20, 20, 20, 30, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
		Paint_DrawLine(30, 30, 30, 40, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawLine(40, 40, 40, 50, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
		Paint_DrawCircle(60, 30, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawCircle(100, 40, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);			
		Paint_DrawRectangle(50, 30, 60, 40, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
		Paint_DrawRectangle(90, 30, 110, 50, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);		
		// 3.Show image on page1
		
		OLED_1IN3_Display(BlackImage);
		Driver_Delay_ms(2000);
		Paint_Clear(BLACK);
		//Driver_Delay_ms(2000);

		// Drawing on the image
		//printf("Drawing:page 2\r\n");

		Paint_DrawString_EN(10, 0, "K1948BK018", &Font16, WHITE, WHITE);
		Paint_DrawString_EN(10, 17, "hello world", &Font8, WHITE, WHITE);
		Paint_DrawNum(10, 30, 123.456789, &Font8, 4, WHITE, WHITE);
		Paint_DrawNum(10, 43, 987654, &Font12, 5, WHITE, WHITE);
		// Show image on page2
		OLED_1IN3_Display(BlackImage);
		Driver_Delay_ms(10000);
		Paint_Clear(BLACK);
		

		// Drawing on the image
		//printf("Drawing:page 3\r\n");
		//Paint_DrawString_CN(10, 0,"���Abc", &Font12CN, WHITE, WHITE);
		//Paint_DrawString_CN(0, 20, "΢ѩ����", &Font24CN, WHITE, WHITE);
		// Show image on page3
		//OLED_1IN3_Display(BlackImage);
		//Driver_Delay_ms(2000);
		//Paint_Clear(BLACK);

		// Drawing on the image
		//printf("Drawing:page 4\r\n");
		OLED_1IN3_Display(gImage_1in3);
//		Driver_Delay_ms(2000);
//		Paint_Clear(BLACK);
		Driver_Delay_ms(2000);

	}
}

