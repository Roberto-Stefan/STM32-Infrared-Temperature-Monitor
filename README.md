# STM32G474RE + MLX90614 Infrared Temperature Monitor  
# Monitor de Temperatura Infravermelho com STM32G474RE + MLX90614

[![Platform STM32](https://img.shields.io/badge/Platform-STM32-blue?style=for-the-badge&logo=stmicroelectronics&logoColor=white)](https://www.st.com/content/st_com/en.html)
[![Language C](https://img.shields.io/badge/Language-C-orange?style=for-the-badge&logo=c&logoColor=white)](https://en.cppreference.com/w/c/language)
[![Framework HAL](https://img.shields.io/badge/Framework-STM32_HAL-green?style=for-the-badge)](https://www.st.com/en/embedded-software/stm32cube-mcu-packages.html)
[![IDE STM32CubeIDE](https://img.shields.io/badge/IDE-STM32CubeIDE-lightgrey?style=for-the-badge)](https://www.st.com/en/development-tools/stm32cubeide.html)

---

## 🇺🇸 Project

Embedded system developed using the STM32 NUCLEO-G474RE board and the MLX90614 infrared sensor for non-contact temperature measurement.

The project performs temperature acquisition through I2C communication, processes the data using the STM32 microcontroller, and displays the information in real time.

---

## 🇧🇷 Projeto

Sistema embarcado desenvolvido com a placa STM32 NUCLEO-G474RE utilizando o sensor infravermelho MLX90614 para medição de temperatura sem contato.

O projeto realiza a leitura da temperatura via comunicação I2C, processa os dados utilizando o microcontrolador STM32 e exibe as informações em tempo real.

---

## Project Image | Imagem do Projeto

<img src="docs/imagens/STM32%20Temperatura%20MLX%2090614.jpg" width="700">

---

## 🇺🇸 Objective

Develop an embedded system capable of performing non-contact temperature monitoring using an infrared sensor integrated with the STM32 microcontroller.

---

## 🇧🇷 Objetivo

Desenvolver um sistema embarcado capaz de realizar monitoramento de temperatura sem contato utilizando sensor infravermelho integrado ao microcontrolador STM32.

---

## 🇺🇸 Features

- I2C communication
- Real-time temperature acquisition
- Data processing using STM32
- Real-time display visualization
- Developed using STM32 HAL
- Real hardware implementation

---

## 🇧🇷 Funcionalidades

- Comunicação I2C
- Leitura de temperatura em tempo real
- Processamento utilizando STM32
- Exibição em display
- Desenvolvimento utilizando STM32 HAL
- Projeto implementado em hardware real

---

## Hardware Used | Hardware Utilizado

### STM32 NUCLEO-G474RE

<img src="docs/imagens/STM32%20Nucleo%20G474%20RE.jpg" width="400">

### MLX90614 Infrared Sensor | Sensor Infravermelho MLX90614

<img src="docs/imagens/Sensor%20MLX%2090614.jpg" width="400">

---

## Technologies Used | Tecnologias Utilizadas

- STM32G474RE
- STM32 HAL
- C Language | Linguagem C
- I2C Communication | Comunicação I2C
- STM32CubeIDE

---

## Project Structure | Estrutura do Projeto

```text
firmware/
└── Sensor_MLX/
    ├── Core/
    ├── Drivers/
    └── STM32CubeIDE project files

docs/
├── imagens/
├── testes/
└── video/
