# **Tracking-Trilha**

**Descrição**

Tracking-Trilha é um sistema de monitoramento de saúde e atividade física desenvolvido para Raspberry Pi Pico W, focado na coleta e análise de dados biométricos em tempo real. O sistema integra sensores de oxigenação sanguínea (SpO2), frequência cardíaca e acelerômetro, fornecendo análise inteligente dos dados através de algoritmos de machine learning e exibindo informações em display OLED SSD1306.

---

## 📋 Requisitos

- Raspberry Pi Pico W
- Sensor MAX3010X (oxímetro e monitor cardíaco)
- Sensor IMU6050 (acelerômetro e giroscópio)
- Display OLED SSD1306 (128x64 pixels)
- Ambiente de desenvolvimento com Pico SDK (versão ≥ 1.5.1)
- Compilador C++17 compatível
- Hardware I2C para comunicação com sensores

---

## ⚙️ Estrutura do Projeto

**src/**

- `main.cpp` — inicializa sensores, analisadores e coordena o loop principal do sistema
- `sensors/oximeter.cpp` — driver para sensor MAX3010X (SpO2, frequência cardíaca, temperatura)
- `sensors/accelerometer.cpp` — driver para sensor IMU6050 (aceleração X, Y, Z)
- `analyzers/analyzer.cpp` — sistema de análise de dados com thresholds configuráveis
- `state/state_collect.cpp` — gerenciamento de estado e coleta de dados dos sensores
- `display/oled.cpp` — controle do display OLED SSD1306
- `drivers/oximeter/MAX3010X.cpp` — implementação de baixo nível do sensor MAX3010X
- `drivers/oximeter/algorithm_by_RF.cpp` — algoritmos de processamento de sinais cardíacos
- `drivers/accelerometer/imu6050.cpp` — implementação de baixo nível do sensor IMU6050
- `drivers/display_oled/ssd1306_i2c.cpp` — comunicação I2C com display OLED
- `drivers/display_oled/display_oled.cpp` — abstração de alto nível para display
- `utils/utils.cpp` — funções utilitárias para manipulação de buffers e dados

**include/**

- `sensors/sensor.h` — interface base para todos os sensores
- `sensors/oximeter.h` — definições específicas do oxímetro
- `sensors/accelerometer.h` — definições específicas do acelerômetro
- `analyzers/analyzer.h` — definições do sistema de análise
- `state/state_collect.h` — gerenciamento de estado do sistema
- `display/oled.h` — interface para display OLED
- `drivers/oximeter/MAX3010X.h` — header do driver MAX3010X
- `drivers/accelerometer/imu6050.h` — header do driver IMU6050
- `drivers/display_oled/ssd1306.h` — definições do display SSD1306
- `utils/utils.h` — funções utilitárias

**lib/**

- `SD-master/` — biblioteca para cartão SD (opcional para logging)

---

## ▶️ Como usar

1. Clone o repositório e navegue até a pasta do projeto:

   ```bash
   git clone <url-do-repositorio>
   cd tracking-trilha
   ```

2. Certifique-se de que o Pico SDK está configurado corretamente no seu ambiente.

3. Conecte os sensores ao Raspberry Pi Pico W:

   - **MAX3010X**: Conecte SDA e SCL aos pinos I2C configurados
   - **IMU6050**: Conecte SDA e SCL aos pinos I2C configurados
   - **Display OLED**: Conecte SDA e SCL aos pinos I2C configurados

4. Compile o projeto com CMake:

   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

5. Flash no Pico W:

   ```bash
   cp tracking-trilha.uf2 /media/<seu-usuario>/RPI-RP2
   ```

6. O sistema iniciará automaticamente, coletando dados dos sensores e exibindo informações no display OLED.

---

## 🔧 Configuração dos Sensores

### Oxímetro MAX3010X

- **Frequência de amostragem**: 400 Hz
- **Largura de pulso**: 411 μs
- **Faixa ADC**: 4096
- **Modo LED**: Red + IR + Green
- **Nível de potência**: 50mA

### Acelerômetro IMU6050

- **Faixa de aceleração**: ±2g, ±4g, ±8g, ±16g
- **Frequência de amostragem**: configurável
- **Calibração automática** para compensar offset

### Display OLED SSD1306

- **Resolução**: 128x64 pixels
- **Interface**: I2C
- **Fonte**: embutida para exibição de texto

---

## 📊 Sistema de Análise

O sistema implementa um analisador inteligente que categoriza os dados dos sensores em 5 níveis de saúde:

- **CRÍTICO BAIXO** (vermelho) - Valores abaixo do normal
- **BAIXO** (laranja) - Valores ligeiramente abaixo do normal
- **NORMAL** (verde) - Valores dentro da faixa saudável
- **ALTO** (amarelo) - Valores ligeiramente acima do normal
- **CRÍTICO ALTO** (vermelho) - Valores muito acima do normal

### Thresholds Configuráveis

- **SpO2**: 90%, 98% (saturação de oxigênio)
- **Frequência Cardíaca**: 60, 100, 140, 180 bpm
- **Aceleração**: 0.5g, 0.75g, 1.2g, 1.5g

---

## 📱 Interface do Usuário

O display OLED apresenta informações em tempo real:

- **Linha 1**: Status do SpO2 com indicador visual
- **Linha 2**: Frequência cardíaca atual
- **Linha 3**: Dados de aceleração (X, Y, Z)
- **Linha 4**: Status geral do sistema

---

## 🎥 Demonstração

Para ver o **Tracking-Trilha** em ação, confira o vídeo abaixo:

[Video de demonstração](https://youtu.be/l7InYfwbljI)

---

## 📝 Comentários e Melhorias Futuras

> **Espaço para observações pessoais:**
>
> - Implementar armazenamento de dados em cartão SD para análise posterior
> - Adicionar conectividade WiFi para envio de dados para servidor remoto
> - Desenvolver aplicativo móvel para visualização dos dados
> - Implementar algoritmos de detecção de quedas baseados no acelerômetro
> - Adicionar sensor de temperatura ambiente para correção de calibração
> - Implementar modo de economia de energia para uso com bateria
> - Adicionar alarmes sonoros para valores críticos
> - Desenvolver interface de configuração via botões físicos
> - Implementar calibração automática dos sensores
> - Adicionar suporte para múltiplos perfis de usuário

---

## 🚀 Funcionalidades Principais

- **Monitoramento contínuo** de SpO2 e frequência cardíaca
- **Análise de movimento** através de acelerômetro triaxial
- **Interface visual** em display OLED com informações em tempo real
- **Sistema de alertas** baseado em thresholds configuráveis
- **Arquitetura modular** para fácil expansão e manutenção
- **Processamento eficiente** com buffers circulares otimizados

---

## 🔍 Troubleshooting

### Problemas Comuns

1. **Sensor não detectado**: Verifique as conexões I2C e tensão de alimentação
2. **Leituras instáveis**: Aguarde a calibração automática (500ms)
3. **Display não funciona**: Confirme endereço I2C e conexões
4. **Compilação falha**: Verifique versão do Pico SDK e compilador C++

### Logs de Debug

O sistema imprime mensagens de debug via UART/USB para facilitar a depuração.

---

Autores: _Miguel Carvalho e Thiago Carrijo_

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, 25 de Junho de 2025

---

**Licença**

Este projeto é distribuído sob a licença GNU GPL-3.0.
