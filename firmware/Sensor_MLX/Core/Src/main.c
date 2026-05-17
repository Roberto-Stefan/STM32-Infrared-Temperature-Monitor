/******************************************************************************
 * @file    main.c
 * @author  Roberto Stefan
 * @brief   Infrared temperature monitoring system using STM32G474RE
 *          and MLX90614 sensor with OLED SSD1306 display.
 *
 * @details
 * This firmware performs non-contact temperature measurement using the
 * MLX90614 infrared sensor through I2C communication.
 *
 * The system monitors:
 *  - Ambient temperature
 *  - Temperature of nearby heat-emitting objects
 *
 * Measured data is processed by the STM32G474RE microcontroller and
 * displayed in real time on an SSD1306 OLED display.
 *
 * Hardware:
 *  - STM32 NUCLEO-G474RE
 *  - MLX90614 Infrared Sensor
 *  - SSD1306 OLED Display
 *
 * Communication Interfaces:
 *  - I2C
 *  - UART (debug support)
 *
 * Development Environment:
 *  - STM32CubeIDE
 *  - STM32 HAL Driver
 *
 * @version 1.0
 * @date    2026
 *
 ******************************************************************************/

/* =========================================================
 *                     INCLUDE FILES
 * ========================================================= */

#include "main.h"
#include <stdio.h>
#include <string.h>

/* =========================================================
 *                  GLOBAL PERIPHERAL HANDLES
 * ========================================================= */

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

/* =========================================================
 *                  MLX90614 DEFINITIONS
 * ========================================================= */

#define MLX90614_ADDR     (0x5A << 1)
#define MLX90614_TA       0x06
#define MLX90614_TOBJ1    0x07

/* =========================================================
 *                  SSD1306 DEFINITIONS
 * ========================================================= */

#define SSD1306_ADDR      (0x3C << 1)
#define SSD1306_WIDTH     128
#define SSD1306_HEIGHT    64
#define SSD1306_PAGES     (SSD1306_HEIGHT/8)

/* =========================================================
 *                    OLED DISPLAY BUFFER
 * ========================================================= */

static uint8_t oled_buf[SSD1306_WIDTH * SSD1306_PAGES];

static uint8_t curX = 0;
static uint8_t curY = 0;

/* =========================================================
 *                      FONT TABLE
 * ========================================================= */

static const uint8_t font5x7[][5] = {
  {0x00,0x00,0x00,0x00,0x00}, /* space */
  {0x00,0x00,0x5F,0x00,0x00}, /* ! */
  {0x00,0x07,0x00,0x07,0x00}, /* " */
  {0x14,0x7F,0x14,0x7F,0x14}, /* # */
  {0x24,0x2A,0x7F,0x2A,0x12}, /* $ */
  {0x23,0x13,0x08,0x64,0x62}, /* % */
  {0x36,0x49,0x55,0x22,0x50}, /* & */
  {0x00,0x05,0x03,0x00,0x00}, /* ' */
  {0x00,0x1C,0x22,0x41,0x00}, /* ( */
  {0x00,0x41,0x22,0x1C,0x00}, /* ) */
  {0x14,0x08,0x3E,0x08,0x14}, /* * */
  {0x08,0x08,0x3E,0x08,0x08}, /* + */
  {0x00,0x50,0x30,0x00,0x00}, /* , */
  {0x08,0x08,0x08,0x08,0x08}, /* - */
  {0x00,0x60,0x60,0x00,0x00}, /* . */
  {0x20,0x10,0x08,0x04,0x02}, /* / */

  /* Continue com o restante da tabela normalmente */
};

/* =========================================================
 *                  FUNCTION PROTOTYPES
 * ========================================================= */

void SystemClock_Config(void);

static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

void Error_Handler(void);

/* =========================================================
 *                    SSD1306 LOW LEVEL
 * ========================================================= */

static void ssd1306_cmd(uint8_t cmd)
{
  uint8_t d[2] = {0x00, cmd};

  HAL_I2C_Master_Transmit(
      &hi2c1,
      SSD1306_ADDR,
      d,
      2,
      100
  );
}

static void ssd1306_data(uint8_t* data, uint16_t len)
{
  uint8_t pkt[129];

  pkt[0] = 0x40;

  while(len)
  {
    uint8_t chunk = (len > 128) ? 128 : len;

    memcpy(&pkt[1], data, chunk);

    HAL_I2C_Master_Transmit(
        &hi2c1,
        SSD1306_ADDR,
        pkt,
        chunk + 1,
        100
    );

    data += chunk;
    len  -= chunk;
  }
}

/* =========================================================
 *                    SSD1306 DRIVER
 * ========================================================= */

void SSD1306_Init(void)
{
  HAL_Delay(100);

  ssd1306_cmd(0xAE);

  ssd1306_cmd(0x20);
  ssd1306_cmd(0x00);

  ssd1306_cmd(0xB0);
  ssd1306_cmd(0xC8);

  ssd1306_cmd(0x00);
  ssd1306_cmd(0x10);

  ssd1306_cmd(0x40);

  ssd1306_cmd(0x81);
  ssd1306_cmd(0x7F);

  ssd1306_cmd(0xA1);
  ssd1306_cmd(0xA6);

  ssd1306_cmd(0xA8);
  ssd1306_cmd(0x3F);

  ssd1306_cmd(0xD3);
  ssd1306_cmd(0x00);

  ssd1306_cmd(0xD5);
  ssd1306_cmd(0x80);

  ssd1306_cmd(0xD9);
  ssd1306_cmd(0xF1);

  ssd1306_cmd(0xDA);
  ssd1306_cmd(0x12);

  ssd1306_cmd(0xDB);
  ssd1306_cmd(0x40);

  ssd1306_cmd(0x8D);
  ssd1306_cmd(0x14);

  ssd1306_cmd(0xAF);
}

void SSD1306_Clear(void)
{
  memset(oled_buf, 0x00, sizeof(oled_buf));

  curX = 0;
  curY = 0;
}

void SSD1306_Update(void)
{
  for(uint8_t p = 0; p < SSD1306_PAGES; p++)
  {
    ssd1306_cmd(0xB0 + p);

    ssd1306_cmd(0x00);
    ssd1306_cmd(0x10);

    ssd1306_data(
        &oled_buf[p * SSD1306_WIDTH],
        SSD1306_WIDTH
    );
  }
}

void SSD1306_GotoXY(uint8_t x, uint8_t y)
{
  curX = x;
  curY = y;
}

/* =========================================================
 *                    MLX90614 DRIVER
 * ========================================================= */

uint16_t MLX90614_ReadReg(uint8_t reg)
{
  uint8_t data[3];

  if(HAL_I2C_Mem_Read(
         &hi2c1,
         MLX90614_ADDR,
         reg,
         I2C_MEMADD_SIZE_8BIT,
         data,
         3,
         100) != HAL_OK)
  {
    return 0xFFFF;
  }

  return (uint16_t)(data[1] << 8) | data[0];
}

float MLX90614_GetTemp(uint8_t reg)
{
  uint16_t raw = MLX90614_ReadReg(reg);

  if(raw == 0xFFFF)
  {
    return -273.15f;
  }

  return (float)raw * 0.02f - 273.15f;
}

/* =========================================================
 *                    MAIN APPLICATION
 * ========================================================= */

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  SSD1306_Init();

  SSD1306_Clear();
  SSD1306_Update();

  char line[32];

  while(1)
  {
    float temp_ambient =
        MLX90614_GetTemp(MLX90614_TA);

    float temp_object =
        MLX90614_GetTemp(MLX90614_TOBJ1);

    SSD1306_Clear();

    SSD1306_GotoXY(0,0);
    SSD1306_WriteString("MLX90614 Monitor");

    SSD1306_GotoXY(0,2);

    snprintf(
        line,
        sizeof(line),
        "Amb: %.2f C",
        temp_ambient
    );

    SSD1306_WriteString(line);

    SSD1306_GotoXY(0,4);

    snprintf(
        line,
        sizeof(line),
        "Obj: %.2f C",
        temp_object
    );

    SSD1306_WriteString(line);

    SSD1306_Update();

    HAL_Delay(500);
  }
}

/* =========================================================
 *               PERIPHERAL INITIALIZATION
 * ========================================================= */

/* MX_I2C1_Init() */
/* MX_USART2_UART_Init() */
/* MX_GPIO_Init() */

/* =========================================================
 *                 SYSTEM CLOCK CONFIGURATION
 * ========================================================= */

/* SystemClock_Config() */

/* =========================================================
 *                     ERROR HANDLER
 * ========================================================= */

void Error_Handler(void)
{
  __disable_irq();

  while(1)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);

    HAL_Delay(100);
  }
}
