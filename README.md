# STM32G474RE + MLX90614 Infrared Temperature Monitor  
# Monitor de Temperatura Infravermelho com STM32G474RE + MLX90614

[![Platform STM32](https://img.shields.io/badge/Platform-STM32-blue?style=for-the-badge&logo=stmicroelectronics&logoColor=white)](https://www.st.com/content/st_com/en.html)
[![Language C](https://img.shields.io/badge/Language-C-orange?style=for-the-badge&logo=c&logoColor=white)](https://en.cppreference.com/w/c/language)
[![Framework HAL](https://img.shields.io/badge/Framework-STM32_HAL-green?style=for-the-badge)](https://www.st.com/en/embedded-software/stm32cube-mcu-packages.html)
[![IDE STM32CubeIDE](https://img.shields.io/badge/IDE-STM32CubeIDE-lightgrey?style=for-the-badge)](https://www.st.com/en/development-tools/stm32cubeide.html)

---

## 🇺🇸 Project

Embedded system developed using the STM32 NUCLEO-G474RE board and the MLX90614 infrared sensor for non-contact temperature measurement.

The system is capable of monitoring both ambient temperature and the temperature of nearby heat-emitting objects in real time.

Temperature acquisition is performed through I2C communication, while the STM32 microcontroller processes the data and displays the information.

---

## 🇧🇷 Projeto

Sistema embarcado desenvolvido com a placa STM32 NUCLEO-G474RE utilizando o sensor infravermelho MLX90614 para medição de temperatura sem contato.

O sistema é capaz de monitorar tanto a temperatura ambiente quanto a temperatura de objetos emissores de calor próximos ao sensor em tempo real.

A leitura da temperatura é realizada via comunicação I2C, enquanto o microcontrolador STM32 processa os dados e exibe as informações no display.

---

## Project Image | Imagem do Projeto

<img src="docs/imagens/STM32%20Temperatura%20MLX%2090614.jpg" width="700">

---

## 🇺🇸 Objective

Develop an embedded system capable of performing real-time non-contact temperature monitoring using an infrared sensor integrated with the STM32 microcontroller.

---

## 🇧🇷 Objetivo

Desenvolver um sistema embarcado capaz de realizar monitoramento de temperatura sem contato em tempo real utilizando sensor infravermelho integrado ao microcontrolador STM32.

---

## 🇺🇸 Features

- I2C communication
- Real-time ambient temperature monitoring
- Detection of nearby heat-emitting objects
- Non-contact temperature measurement
- Data processing using STM32
- Real-time display visualization
- Developed using STM32 HAL
- Real hardware implementation

---

## 🇧🇷 Funcionalidades

- Comunicação I2C
- Monitoramento da temperatura ambiente em tempo real
- Detecção de objetos emissores de calor próximos ao sensor
- Medição de temperatura sem contato
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
