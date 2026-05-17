# STM32G474RE + MLX90614 Infrared Temperature Monitor

![STM32](https://img.shields.io/badge/STM32-G474RE-blue)
![Framework](https://img.shields.io/badge/Framework-STM32_HAL-green)
![Language](https://img.shields.io/badge/Language-C-orange)
![IDE](https://img.shields.io/badge/IDE-STM32CubeIDE-lightgrey)

---

## Projeto

Sistema embarcado desenvolvido com a placa STM32 NUCLEO-G474RE utilizando o sensor infravermelho MLX90614 para medição de temperatura sem contato.

O projeto realiza a leitura da temperatura via comunicação I2C, processa os dados no microcontrolador STM32 e exibe as informações em display.

---

## Imagem do Projeto

<img src="docs/imagens/STM32%20Temperatura%20MLX%2090614.jpg" width="700">

---

## Objetivo

Desenvolver um sistema embarcado capaz de realizar monitoramento de temperatura sem contato utilizando sensor infravermelho integrado ao microcontrolador STM32.

---

## Funcionalidades

- Comunicação I2C
- Leitura de temperatura em tempo real
- Processamento utilizando STM32
- Exibição em display
- Desenvolvimento utilizando STM32 HAL
- Projeto implementado em hardware real

---

## Hardware Utilizado

### STM32 NUCLEO-G474RE

<img src="docs/imagens/STM32%20Nucleo%20G474%20RE.jpg" width="400">

### Sensor Infravermelho MLX90614

<img src="docs/imagens/Sensor%20MLX%2090614.jpg" width="400">

---

## Tecnologias Utilizadas

- STM32G474RE
- STM32 HAL
- Linguagem C
- Comunicação I2C
- STM32CubeIDE

---

## Estrutura do Projeto

```text
firmware/
└── Sensor_MLX/
    ├── Core/
    ├── Drivers/
    └── Arquivos STM32CubeIDE

docs/
├── imagens/
├── testes/
└── video/
