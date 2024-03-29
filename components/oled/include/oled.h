#pragma once		  	 	
#include "driver/gpio.h"  
#include "driver/i2c.h"
#define  u8 unsigned char 
#define  u32 unsigned int 
#define OLED_CMD  0	
#define OLED_DATA 1	
#define OLED_MODE 0
 

#define PIN_NUM_MISO 13          //SDA
#define PIN_NUM_CLK 14           //SCL
#define PIN_NUM_CS  22          //
#define PIN_NUM_DC  21          //DC
#define PIN_NUM_RST 18          //RES

/*i2c*/
#define I2C_MASTER_SCL_IO 14        /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 13        /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM 0            /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 100

#define OLED_ADDR 0x3C
 
#define OLED_CS_Clr()  gpio_set_level(PIN_NUM_CS, 0)   
#define OLED_CS_Set()  gpio_set_level(PIN_NUM_CS, 1)  
 
#define OLED_RST_Clr() gpio_set_level(PIN_NUM_RST, 0)
#define OLED_RST_Set() gpio_set_level(PIN_NUM_RST, 1) 
 
#define OLED_DC_Clr() gpio_set_level(PIN_NUM_DC, 0)
#define OLED_DC_Set() gpio_set_level(PIN_NUM_DC, 1)
 
#define OLED_SCLK_Clr() gpio_set_level(PIN_NUM_CLK, 0) 
#define OLED_SCLK_Set() gpio_set_level(PIN_NUM_CLK, 1) 
 
#define OLED_SDIN_Clr() gpio_set_level(PIN_NUM_MISO, 0) 
#define OLED_SDIN_Set() gpio_set_level(PIN_NUM_MISO, 1)
 
 
#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
					
void delay_ms(unsigned int ms);
esp_err_t OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);
void OLED_ShowNumss(u8 x,u8 y,float num);
void OLED_ShowString(u8 x,u8 y, u8 *p);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void OLED_GPIO_Init(void);

/*I2C总线初始化*/
void I2C_Oled_Init(void);