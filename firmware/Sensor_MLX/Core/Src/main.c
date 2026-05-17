/******************************************************************************

 * @file    main.c
 * @brief   [Breve descrição da funcionalidade do firmware]
 * @author  [Seu Nome]
 * @date    [Data]
 * @version [Versão]
 *
 * @details [Descrição detalhada do firmware, incluindo propósito, hardware,
 *          interfaces utilizadas (ex.: I2C para MLX90614 e SSD1306), e quaisquer
 *          recursos ou algoritmos específicos implementados.]
 *
 * @note    [Notas adicionais, como dependências, problemas conhecidos ou
 *          instruções de uso.]
 *
 ******************************************************************************/

// Incluímos bibliotecas necessárias para o programa funcionar.
// main.h: Contém definições básicas do projeto STM32.
// stdio.h: Para funções de entrada/saída, como printf (usado aqui para formatar texto).
// string.h: Para manipulação de strings, como memcpy e memset.
#include "main.h"
#include <stdio.h>
#include <string.h>

// Estruturas para controlar os periféricos de comunicação I2C (para sensores) e UART (para serial, como USB).
// I2C_HandleTypeDef: Gerencia a comunicação I2C com o sensor MLX90614 e display SSD1306.
// UART_HandleTypeDef: Gerencia a comunicação serial (não usada no loop principal, mas inicializada).
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

// Definições para o sensor de temperatura MLX90614.
// MLX90614_ADDR: Endereço do sensor no barramento I2C (0x5A deslocado para 8 bits).
// MLX90614_TA: Registro para temperatura ambiente.
// MLX90614_TOBJ1: Registro para temperatura do objeto detectado.
#define MLX90614_ADDR     (0x5A << 1)   // HAL usa endereço deslocado
#define MLX90614_TA       0x06
#define MLX90614_TOBJ1    0x07

// Definições para o display OLED SSD1306 (128x64 pixels, via I2C).
// SSD1306_ADDR: Endereço do display no I2C.
// SSD1306_WIDTH/HEIGHT: Dimensões do display.
// SSD1306_PAGES: Número de "páginas" de memória (cada página é 8 linhas de pixels).
#define SSD1306_ADDR      (0x3C << 1)
#define SSD1306_WIDTH     128
#define SSD1306_HEIGHT    64
#define SSD1306_PAGES     (SSD1306_HEIGHT/8)

// Buffer de memória para armazenar o que será mostrado no display OLED.
// Cada byte representa 8 pixels verticais em uma coluna.
// Total: 128 colunas x 8 páginas = 1024 bytes.
static uint8_t oled_buf[SSD1306_WIDTH * SSD1306_PAGES];  // 1024 bytes

// Posição atual do cursor no display (X: coluna, Y: página).
static uint8_t curX = 0, curY = 0;

// Fonte de caracteres 5x7 pixels para desenhar texto no display.
// Cada caractere (de espaço a ~) é representado por 5 bytes (colunas de pixels).
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
  {0x3E,0x51,0x49,0x45,0x3E}, /* 0 */
  {0x00,0x42,0x7F,0x40,0x00}, /* 1 */
  {0x42,0x61,0x51,0x49,0x46}, /* 2 */
  {0x21,0x41,0x45,0x4B,0x31}, /* 3 */
  {0x18,0x14,0x12,0x7F,0x10}, /* 4 */
  {0x27,0x45,0x45,0x45,0x39}, /* 5 */
  {0x3C,0x4A,0x49,0x49,0x30}, /* 6 */
  {0x01,0x71,0x09,0x05,0x03}, /* 7 */
  {0x36,0x49,0x49,0x49,0x36}, /* 8 */
  {0x06,0x49,0x49,0x29,0x1E}, /* 9 */
  {0x00,0x36,0x36,0x00,0x00}, /* : */
  {0x00,0x56,0x36,0x00,0x00}, /* ; */
  {0x08,0x14,0x22,0x41,0x00}, /* < */
  {0x14,0x14,0x14,0x14,0x14}, /* = */
  {0x00,0x41,0x22,0x14,0x08}, /* > */
  {0x02,0x01,0x51,0x09,0x06}, /* ? */
  {0x32,0x49,0x79,0x41,0x3E}, /* @ */
  {0x7E,0x11,0x11,0x11,0x7E}, /* A */
  {0x7F,0x49,0x49,0x49,0x36}, /* B */
  {0x3E,0x41,0x41,0x41,0x22}, /* C */
  {0x7F,0x41,0x41,0x22,0x1C}, /* D */
  {0x7F,0x49,0x49,0x49,0x41}, /* E */
  {0x7F,0x09,0x09,0x09,0x01}, /* F */
  {0x3E,0x41,0x49,0x49,0x7A}, /* G */
  {0x7F,0x08,0x08,0x08,0x7F}, /* H */
  {0x00,0x41,0x7F,0x41,0x00}, /* I */
  {0x20,0x40,0x41,0x3F,0x01}, /* J */
  {0x7F,0x08,0x14,0x22,0x41}, /* K */
  {0x7F,0x40,0x40,0x40,0x40}, /* L */
  {0x7F,0x02,0x0C,0x02,0x7F}, /* M */
  {0x7F,0x04,0x08,0x10,0x7F}, /* N */
  {0x3E,0x41,0x41,0x41,0x3E}, /* O */
  {0x7F,0x09,0x09,0x09,0x06}, /* P */
  {0x3E,0x41,0x51,0x21,0x5E}, /* Q */
  {0x7F,0x09,0x19,0x29,0x46}, /* R */
  {0x46,0x49,0x49,0x49,0x31}, /* S */
  {0x01,0x01,0x7F,0x01,0x01}, /* T */
  {0x3F,0x40,0x40,0x40,0x3F}, /* U */
  {0x1F,0x20,0x40,0x20,0x1F}, /* V */
  {0x3F,0x40,0x38,0x40,0x3F}, /* W */
  {0x63,0x14,0x08,0x14,0x63}, /* X */
  {0x07,0x08,0x70,0x08,0x07}, /* Y */
  {0x61,0x51,0x49,0x45,0x43}, /* Z */
  {0x00,0x7F,0x41,0x41,0x00}, /* [ */
  {0x02,0x04,0x08,0x10,0x20}, /* \ */
  {0x00,0x41,0x41,0x7F,0x00}, /* ] */
  {0x04,0x02,0x01,0x02,0x04}, /* ^ */
  {0x40,0x40,0x40,0x40,0x40}, /* _ */
  {0x00,0x01,0x02,0x04,0x00}, /* ` */
  {0x20,0x54,0x54,0x54,0x78}, /* a */
  {0x7F,0x48,0x44,0x44,0x38}, /* b */
  {0x38,0x44,0x44,0x44,0x20}, /* c */
  {0x38,0x44,0x44,0x48,0x7F}, /* d */
  {0x38,0x54,0x54,0x54,0x18}, /* e */
  {0x08,0x7E,0x09,0x01,0x02}, /* f */
  {0x08,0x14,0x54,0x54,0x3C}, /* g */
  {0x7F,0x08,0x04,0x04,0x78}, /* h */
  {0x00,0x44,0x7D,0x40,0x00}, /* i */
  {0x20,0x40,0x44,0x3D,0x00}, /* j */
  {0x7F,0x10,0x28,0x44,0x00}, /* k */
  {0x00,0x41,0x7F,0x40,0x00}, /* l */
  {0x7C,0x04,0x18,0x04,0x78}, /* m */
  {0x7C,0x08,0x04,0x04,0x78}, /* n */
  {0x38,0x44,0x44,0x44,0x38}, /* o */
  {0x7C,0x14,0x14,0x14,0x08}, /* p */
  {0x08,0x14,0x14,0x18,0x7C}, /* q */
  {0x7C,0x08,0x04,0x04,0x08}, /* r */
  {0x48,0x54,0x54,0x54,0x20}, /* s */
  {0x04,0x3F,0x44,0x40,0x20}, /* t */
  {0x3C,0x40,0x40,0x20,0x7C}, /* u */
  {0x1C,0x20,0x40,0x20,0x1C}, /* v */
  {0x3C,0x40,0x30,0x40,0x3C}, /* w */
  {0x44,0x28,0x10,0x28,0x44}, /* x */
  {0x0C,0x50,0x50,0x50,0x3C}, /* y */
  {0x44,0x64,0x54,0x4C,0x44}, /* z */
  {0x00,0x08,0x36,0x41,0x00}, /* { */
  {0x00,0x00,0x7F,0x00,0x00}, /* | */
  {0x00,0x41,0x36,0x08,0x00}, /* } */
  {0x10,0x08,0x08,0x10,0x08}  /* ~ */
};

// Declarações antecipadas das funções (protótipos).
// Permite que as funções sejam usadas antes de serem definidas.
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
void Error_Handler(void);

// Funções de baixo nível para o display SSD1306.
// ssd1306_cmd: Envia um comando ao display via I2C (ex.: configurar modo).
static void ssd1306_cmd(uint8_t cmd){
  uint8_t d[2] = {0x00, cmd};  // Primeiro byte indica comando (0x00), segundo é o comando.
  HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR, d, 2, 100);  // Envia via I2C.
}

// ssd1306_data: Envia dados (pixels) ao display via I2C.
static void ssd1306_data(uint8_t* data, uint16_t len){
  uint8_t pkt[129];  // Pacote: primeiro byte 0x40 (indica dados), depois os dados.
  pkt[0] = 0x40;
  while(len){  // Envia em pedaços se necessário (máx. 128 bytes por vez).
    uint8_t chunk = (len>128)?128:len;
    memcpy(&pkt[1], data, chunk);
    HAL_I2C_Master_Transmit(&hi2c1, SSD1306_ADDR, pkt, chunk+1, 100);
    data += chunk;
    len  -= chunk;
  }
}

// Funções da API do SSD1306 (interface simples para usar o display).
// SSD1306_Init: Inicializa o display com configurações padrão.
void SSD1306_Init(void){
  HAL_Delay(100);  // Espera o display estabilizar.
  ssd1306_cmd(0xAE);  // Desliga o display.
  ssd1306_cmd(0x20); ssd1306_cmd(0x00);  // Modo de endereçamento horizontal.
  ssd1306_cmd(0xB0);  // Página inicial.
  ssd1306_cmd(0xC8);  // Direção vertical invertida.
  ssd1306_cmd(0x00);  // Coluna baixa.
  ssd1306_cmd(0x10);  // Coluna alta.
  ssd1306_cmd(0x40);  // Linha inicial.
  ssd1306_cmd(0x81); ssd1306_cmd(0x7F);  // Contraste.
  ssd1306_cmd(0xA1);  // Segmento remapeado.
  ssd1306_cmd(0xA6);  // Display normal (não invertido).
  ssd1306_cmd(0xA8); ssd1306_cmd(0x3F);  // Multiplex ratio.
  ssd1306_cmd(0xD3); ssd1306_cmd(0x00);  // Offset de display.
  ssd1306_cmd(0xD5); ssd1306_cmd(0x80);  // Clock divide.
  ssd1306_cmd(0xD9); ssd1306_cmd(0xF1);  // Pre-charge.
  ssd1306_cmd(0xDA); ssd1306_cmd(0x12);  // Configuração de COM.
  ssd1306_cmd(0xDB); ssd1306_cmd(0x40);  // VCOM deselect.
  ssd1306_cmd(0x8D); ssd1306_cmd(0x14);  // Charge pump on.
  ssd1306_cmd(0xAF);  // Liga o display.
}

// SSD1306_Clear: Limpa o buffer do display (tudo preto).
void SSD1306_Clear(void){
  memset(oled_buf, 0x00, sizeof(oled_buf));  // Zera o buffer.
  curX = 0; curY = 0;  // Reseta o cursor.
}

// SSD1306_Update: Envia o buffer para o display para mostrar na tela.
void SSD1306_Update(void){
  for(uint8_t p=0; p<SSD1306_PAGES; p++){  // Para cada página (linha de 8 pixels).
    ssd1306_cmd(0xB0 + p);  // Seleciona a página.
    ssd1306_cmd(0x00);  // Coluna baixa.
    ssd1306_cmd(0x10);  // Coluna alta.
    ssd1306_data(&oled_buf[p*SSD1306_WIDTH], SSD1306_WIDTH);  // Envia os dados da página.
  }
}

// SSD1306_GotoXY: Move o cursor para uma posição específica (x: coluna, y: página).
void SSD1306_GotoXY(uint8_t x, uint8_t y){ curX = x; curY = y; }

// SSD1306_DrawChar: Desenha um caractere no buffer na posição do cursor.
static void SSD1306_DrawChar(char ch){
  if(ch < 32 || ch > 126) ch='?';  // Se não for um caractere válido, usa '?'.
  uint8_t idx = ch - 32;  // Índice na tabela de fonte.
  if(curX + 6 >= SSD1306_WIDTH) return;  // Se não couber, ignora.
  if(curY >= SSD1306_PAGES) return;
  for(uint8_t i=0;i<5;i++){  // Copia os 5 bytes do caractere para o buffer.
    oled_buf[curY*SSD1306_WIDTH + curX + i] = font5x7[idx][i];
  }
  oled_buf[curY*SSD1306_WIDTH + curX + 5] = 0x00;  // Espaço entre caracteres.
  curX += 6;  // Avança o cursor.
}

// SSD1306_WriteString: Escreve uma string inteira no display.
void SSD1306_WriteString(const char* s){
  while(*s){  // Para cada caractere da string.
    if(*s=='\n'){ curY++; curX=0; }  // Se for nova linha, pula para a próxima página.
    else SSD1306_DrawChar(*s);  // Senão, desenha o caractere.
    s++;  // Próximo caractere.
  }
}

// Funções para o sensor de temperatura MLX90614.
// MLX90614_ReadReg: Lê um registro (valor bruto) do sensor via I2C.
// Retorna o valor lido ou 0xFFFF se erro.
uint16_t MLX90614_ReadReg(uint8_t reg){
  uint8_t data[3];  // O sensor retorna 3 bytes: dados LSB, MSB e checksum.
  if(HAL_I2C_Mem_Read(&hi2c1, MLX90614_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 3, 100)!=HAL_OK)
    return 0xFFFF;  // Erro na leitura.
  return (uint16_t)(data[1]<<8) | data[0];  // Combina MSB e LSB.
}

// MLX90614_GetTemp: Converte o valor bruto em temperatura em Celsius.
// Fórmula: temp = raw * 0.02 - 273.15 (conversão de Kelvin para Celsius).
float MLX90614_GetTemp(uint8_t reg){
  uint16_t raw = MLX90614_ReadReg(reg);  // Lê o valor bruto.
  if(raw==0xFFFF) return -273.15f;  // Erro: retorna valor inválido.
  return (float)raw * 0.02f - 273.15f;  // Calcula a temperatura.
}

// Função principal do programa (loop infinito).
// Aqui o microcontrolador executa o código repetidamente.
int main(void){
  HAL_Init();  // Inicializa a biblioteca HAL (Hardware Abstraction Layer).
  SystemClock_Config();  // Configura o clock do sistema.
  MX_GPIO_Init();  // Inicializa os pinos GPIO.
  MX_I2C1_Init();  // Inicializa a comunicação I2C para sensores.
  MX_USART2_UART_Init();  // Inicializa a UART (serial, não usada aqui).

  SSD1306_Init();  // Prepara o display OLED para uso.
  SSD1306_Clear();  // Limpa a tela.
  SSD1306_Update();  // Atualiza a tela (mostra a tela limpa).

  char line[32];  // Buffer para formatar textos (temperaturas).

  while(1){  // Loop infinito: o programa roda para sempre.
    // Lê as temperaturas do sensor.
    float temp_ambient = MLX90614_GetTemp(MLX90614_TA);  // Temperatura ambiente.
    float temp_object = MLX90614_GetTemp(MLX90614_TOBJ1);  // Temperatura do objeto.

    SSD1306_Clear();  // Limpa a tela antes de desenhar novo conteúdo.
    SSD1306_GotoXY(0,0);  // Posiciona o cursor no topo esquerdo.
    SSD1306_WriteString("MLX90614 Monitor");  // Escreve o título.
    SSD1306_GotoXY(0,2);  // Pula para a segunda linha (página 2).
    snprintf(line,sizeof(line),"Amb: %.2f C", temp_ambient);  // Formata a temperatura ambiente.
    SSD1306_WriteString(line);  // Escreve no display.
    SSD1306_GotoXY(0,4);  // Pula para a quarta linha.
    snprintf(line,sizeof(line),"Obj: %.2f C", temp_object);  // Formata a temperatura do objeto.
    SSD1306_WriteString(line);  // Escreve no display.
    SSD1306_Update();  // Atualiza a tela para mostrar as mudanças.

    HAL_Delay(500);  // Espera 500ms (meio segundo) antes de repetir.
  }
}

// Funções de inicialização dos periféricos (geradas pelo STM32CubeMX).
// MX_I2C1_Init: Configura o barramento I2C para comunicação com sensores.
static void MX_I2C1_Init(void){
  hi2c1.Instance = I2C1;  // Usa o periférico I2C1.
  hi2c1.Init.Timing = 0x40B285C2;  // Configuração de timing (velocidade).
  hi2c1.Init.OwnAddress1 = 0;  // Endereço próprio (não usado como escravo).
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;  // Modo de endereçamento de 7 bits.
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;  // Desabilita endereço duplo.
  hi2c1.Init.OwnAddress2 = 0;  // Segundo endereço.
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;  // Máscara.
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;  // Desabilita chamada geral.
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;  // Permite clock stretching.
  if(HAL_I2C_Init(&hi2c1)!=HAL_OK) Error_Handler();  // Inicializa e verifica erro.
  HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE);  // Filtro analógico.
  HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0);  // Filtro digital.
}

// MX_USART2_UART_Init: Configura a UART para comunicação serial (USB).
static void MX_USART2_UART_Init(void){
  huart2.Instance = USART2;  // Usa o periférico USART2.
  huart2.Init.BaudRate = 115200;  // Velocidade de transmissão (bits por segundo).
  huart2.Init.WordLength = UART_WORDLENGTH_8B;  // 8 bits por palavra.
  huart2.Init.StopBits = UART_STOPBITS_1;  // 1 bit de parada.
  huart2.Init.Parity = UART_PARITY_NONE;  // Sem paridade.
  huart2.Init.Mode = UART_MODE_TX_RX;  // Modo transmissão e recepção.
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;  // Sem controle de fluxo.
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;  // Oversampling.
  if(HAL_UART_Init(&huart2)!=HAL_OK) Error_Handler();  // Inicializa e verifica erro.
}

// MX_GPIO_Init: Configura os pinos GPIO (entradas/saídas digitais).
static void MX_GPIO_Init(void){
  __HAL_RCC_GPIOC_CLK_ENABLE();  // Habilita o clock para o GPIO C.
  GPIO_InitTypeDef g={0};  // Estrutura de configuração.
  g.Pin = GPIO_PIN_6;  // Pino PC6.
  g.Mode = GPIO_MODE_OUTPUT_PP;  // Modo saída push-pull.
  g.Pull = GPIO_NOPULL;  // Sem resistor de pull-up/down.
  g.Speed = GPIO_SPEED_FREQ_LOW;  // Velocidade baixa.
  HAL_GPIO_Init(GPIOC,&g);  // Inicializa o pino.
}

// SystemClock_Config: Configura o clock do sistema (gerado pelo STM32CubeMX).
void SystemClock_Config(void){
  RCC_OscInitTypeDef RCC_OscInitStruct={0};  // Configuração dos osciladores.
  RCC_ClkInitTypeDef RCC_ClkInitStruct={0};  // Configuração do clock.
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);  // Escala de tensão.

  RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSI;  // Oscilador interno HSI.
  RCC_OscInitStruct.HSIState=RCC_HSI_ON;  // HSI ligado.
  RCC_OscInitStruct.HSICalibrationValue=RCC_HSICALIBRATION_DEFAULT;  // Calibração padrão.
  RCC_OscInitStruct.PLL.PLLState=RCC_PLL_ON;  // PLL ligado.
  RCC_OscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSI;  // Fonte HSI.
  RCC_OscInitStruct.PLL.PLLM=RCC_PLLM_DIV4;  // Divisor M.
  RCC_OscInitStruct.PLL.PLLN=85;  // Multiplicador N.
  RCC_OscInitStruct.PLL.PLLP=RCC_PLLP_DIV2;  // Divisor P.
  RCC_OscInitStruct.PLL.PLLQ=RCC_PLLQ_DIV2;  // Divisor Q.
  RCC_OscInitStruct.PLL.PLLR=RCC_PLLR_DIV2;  // Divisor R.
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct)!=HAL_OK) Error_Handler();  // Configura e verifica erro.

  RCC_ClkInitStruct.ClockType=RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|
                              RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;  // Tipos de clock.
  RCC_ClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;  // Fonte do clock sistema.
  RCC_ClkInitStruct.AHBCLKDivider=RCC_SYSCLK_DIV1;  // Divisor AHB.
  RCC_ClkInitStruct.APB1CLKDivider=RCC_HCLK_DIV1;  // Divisor APB1.
  RCC_ClkInitStruct.APB2CLKDivider=RCC_HCLK_DIV1;  // Divisor APB2.
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4)!=HAL_OK) Error_Handler();  // Configura e verifica erro.
}

// Error_Handler: Função chamada quando há erro de inicialização.
// Pisca um LED para indicar erro.
void Error_Handler(void){
  __disable_irq();  // Desabilita interrupções.
  while(1){  // Loop infinito.
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_6);  // Inverte o estado do pino PC6 (LED).
    HAL_Delay(100);  // Espera 100ms.
  }
}
