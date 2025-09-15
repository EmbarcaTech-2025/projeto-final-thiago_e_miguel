# **Tracking-Trilha**

**Descrição**

Tracking-Trilha é um sistema de monitoramento de saúde e atividade física desenvolvido para Raspberry Pi Pico W, focado na coleta e análise de dados biométricos em tempo real. O sistema integra sensores de oxigenação sanguínea (SpO2), frequência cardíaca e acelerômetro, fornecendo análise dos dados e exibindo informações em display OLED SSD1306.

---

## 📋 Requisitos

- Raspberry Pi Pico W
- Sensor MAX3010X (oxímetro e monitor cardíaco)
- Sensor IMU6050 (acelerômetro e giroscópio)
- Sensor GPS GY-NEO6MV2 (opcional)
- Display OLED SSD1306 (128x64 pixels)
- Cartão SD (opcional para armazenamento)
- Buzzer para alertas sonoros
- Ambiente de desenvolvimento com Pico SDK (versão ≥ 1.5.1)
- Compilador C++17 compatível
- Hardware I2C para comunicação com sensores
- Hardware SPI para cartão SD
- Hardware UART para GPS

---

## ⚙️ Estrutura do Projeto

**src/**

- `main.cpp` — inicializa sensores, analisadores e coordena o loop principal do sistema
- `sensors/oximeter.cpp` — driver para sensor MAX3010X (SpO2, frequência cardíaca, temperatura)
- `sensors/accelerometer.cpp` — driver para sensor IMU6050 (aceleração X, Y, Z)
- `sensors/gps.cpp` — driver para sensor GPS GY-NEO6MV2 (latitude, longitude, altitude, velocidade)
- `analyzers/analyzer.cpp` — sistema de análise de dados com thresholds configuráveis
- `state/state_collect.cpp` — gerenciamento de estado e coleta de dados dos sensores
- `state/state.cpp` — máquina de estado base do sistema
- `display/oled.cpp` — controle do display OLED SSD1306
- `buzzer/buzzer.cpp` — controle do buzzer para alertas sonoros
- `drivers/oximeter/MAX3010X.cpp` — implementação de baixo nível do sensor MAX3010X
- `drivers/oximeter/algorithm_by_RF.cpp` — algoritmos de processamento de sinais cardíacos
- `drivers/accelerometer/imu6050.cpp` — implementação de baixo nível do sensor IMU6050
- `drivers/gps/GPS_GY-NEO6MV2.cpp` — implementação de baixo nível do sensor GPS
- `drivers/display_oled/ssd1306_i2c.cpp` — comunicação I2C com display OLED
- `drivers/display_oled/display_oled.cpp` — abstração de alto nível para display
- `drivers/sd_card/SD.cpp` — implementação de armazenamento em cartão SD
- `drivers/sd_card/hw_config.c` — configuração de hardware para cartão SD
- `utils/utils.cpp` — funções utilitárias para manipulação de buffers e dados

**include/**

- `sensors/sensor.h` — interface base para todos os sensores
- `sensors/oximeter.h` — definições específicas do oxímetro
- `sensors/accelerometer.h` — definições específicas do acelerômetro
- `sensors/gps.h` — definições específicas do GPS
- `analyzers/analyzer.h` — definições do sistema de análise
- `state/state_collect.h` — gerenciamento de estado do sistema
- `state/state.h` — interface base para máquinas de estado
- `display/oled.h` — interface para display OLED
- `buzzer/buzzer.h` — interface para controle do buzzer
- `drivers/oximeter/MAX3010X.h` — header do driver MAX3010X
- `drivers/accelerometer/imu6050.h` — header do driver IMU6050
- `drivers/gps/GPS_GY-NEO6MV2.h` — header do driver GPS
- `drivers/display_oled/ssd1306.h` — definições do display SSD1306
- `drivers/sd_card/SD.h` — interface para armazenamento em SD
- `utils/utils.h` — funções utilitárias
- `FreeRTOSConfig.h` — configuração do FreeRTOS

**lib/**

- `FreeRTOS/` — kernel FreeRTOS para multitarefa
- `sd/` — FreeRTOS+FAT+CLI para sistema de arquivos
- `web/` — interface web para visualização de dados

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

### Usando a Interface Web

1. **Acesse a pasta web**: Navegue até a pasta `web/` do projeto
2. **Abra o arquivo index.html**: Use qualquer navegador moderno
3. **Upload de dados**: Faça upload do arquivo `samples.csv` gerado pelo sistema
4. **Configuração**: Faça upload do arquivo `config.txt` para editar thresholds
5. **Visualização**: Use os gráficos interativos para analisar os dados
6. **Download**: Baixe os arquivos modificados após edição

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

### GPS GY-NEO6MV2

- **Frequência de operação**: 1Hz
- **Precisão**: 2.5m CEP
- **Interface**: UART
- **Dados coletados**: latitude, longitude, altitude, velocidade, número de satélites

### Display OLED SSD1306

- **Resolução**: 128x64 pixels
- **Interface**: I2C
- **Fonte**: embutida para exibição de texto

### Cartão SD

- **Sistema de arquivos**: FAT32
- **Interface**: SPI
- **Armazenamento**: dados dos sensores em formato CSV

### Buzzer

- **Controle**: PWM
- **Frequências**: configuráveis
- **Função**: alertas sonoros para valores críticos

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
- **Temperatura**: 35°C, 37°C, 39°C, 41°C
- **Aceleração**: 0.5g, 0.75g, 1.2g, 1.5g
- **GPS**: Configurável via arquivo config.txt

---

## 🔄 Sistema FreeRTOS

O sistema utiliza FreeRTOS para gerenciamento de tarefas e recursos:

### Tarefas Implementadas

- **Task do Oxímetro**: Coleta contínua de dados do sensor MAX3010X
- **Task de Coleta de Estado**: Processa dados de todos os sensores e atualiza display

### Configurações FreeRTOS

- **Heap Size**: 128KB para alocação dinâmica
- **Prioridades**: 32 níveis de prioridade
- **Stack Overflow**: Detecção habilitada
- **Mutexes**: Suporte completo para sincronização
- **Timers**: Suporte a timers de software
- **SMP**: Suporte a múltiplos cores (RP2040)

### Recursos de Sincronização

- **Mutexes**: Proteção de recursos compartilhados
- **Semáforos**: Sincronização entre tarefas
- **Queues**: Comunicação entre tarefas
- **Timers**: Execução de funções em intervalos regulares

---

## 📱 Interface do Usuário

O display OLED apresenta informações em tempo real:

- **Linha 1**: Status do SpO2 com indicador visual
- **Linha 2**: Frequência cardíaca atual
- **Linha 3**: Dados de aceleração (X, Y, Z)

---

## 💾 Armazenamento de Dados

### Cartão SD

O sistema suporta armazenamento de dados em cartão SD usando FreeRTOS+FAT:

- **Formato**: CSV com timestamp
- **Dados armazenados**: Tipo de sensor, tipo de amostra, timestamp, valor, status de saúde
- **Arquivo padrão**: `samples.csv`
- **Sistema de arquivos**: FAT32
- **Interface**: SPI

### Estrutura dos Dados

```csv
sensor_type,sample_type,timestamp,data,health_status
0,0,8370,98.5,2
0,1,8381,72.0,2
1,8,8392,1.008798,4
```

### Configuração de Thresholds

- **Arquivo**: `config.txt`
- **Formato**: `sensorType,sampleType,threshold1,threshold2,threshold3,threshold4,threshold5`
- **Edição**: Via interface web ou manualmente

---

## 🔊 Sistema de Alertas

### Buzzer

O sistema inclui um buzzer para alertas sonoros:

- **Controle**: PWM com frequência e duty cycle configuráveis
- **Ativação**: Valores críticos dos sensores
- **Frequências**: Configuráveis por tipo de alerta
- **Função**: `setTone(freq_hz, duty_percent)`

### Tipos de Alertas

- **CRÍTICO BAIXO**: Frequência alta para valores muito baixos
- **CRÍTICO ALTO**: Frequência alta para valores muito altos
- **BAIXO/ALTO**: Frequência média para valores fora do normal
- **NORMAL**: Sem alerta sonoro

---

## 🌐 Interface Web

### Visualizador de Dados

Sistema web completo para análise e configuração:

- **Upload de dados**: Arquivos CSV dos sensores
- **Gráficos interativos**: Chart.js para visualização
- **Editor de configuração**: Modificação de thresholds
- **Análise estatística**: Média, min, max, desvio padrão
- **Filtros**: Por tipo de sensor e amostra

### Funcionalidades

- **Visualização**: Gráficos dinâmicos
- **Configuração remota**: Edição de thresholds via web
- **Exportação**: Download de dados e configurações
- **Responsivo**: Funciona em desktop e mobile

---

## 🎥 Demonstração

Para ver o **Tracking-Trilha** em ação, confira o vídeo abaixo:

[Video de demonstração](https://youtu.be/l7InYfwbljI)

---

## 📝 Comentários e Melhorias Futuras

> **Funcionalidades já implementadas:**
>
> ✅ **Armazenamento de dados em cartão SD** - Implementado com FreeRTOS+FAT
> ✅ **Alarmes sonoros para valores críticos** - Sistema de buzzer implementado
> ✅ **Interface web para visualização** - Sistema completo com gráficos e configuração
> ✅ **Sistema multitarefa** - FreeRTOS com tarefas dedicadas
> ✅ **Configuração dinâmica** - Arquivo config.txt editável via web
> ✅ **Monitoramento GPS** - Rastreamento de localização e velocidade
> ✅ **Sistema de análise inteligente** - Classificação automática de dados
>
> **Melhorias futuras:**
>
> - Adicionar conectividade WiFi para envio de dados para servidor remoto
> - Desenvolver aplicativo móvel para visualização dos dados
> - Implementar algoritmos de detecção de quedas baseados no acelerômetro
> - Adicionar sensor de temperatura ambiente para correção de calibração
> - Implementar modo de economia de energia para uso com bateria
> - Desenvolver interface de configuração via botões físicos
> - Implementar calibração automática dos sensores
> - Adicionar suporte para múltiplos perfis de usuário
> - Integração com APIs de saúde para análise avançada
> - Sistema de backup automático dos dados

---

## 🚀 Funcionalidades Principais

- **Monitoramento contínuo** de SpO2, frequência cardíaca e temperatura
- **Análise de movimento** através de acelerômetro triaxial
- **Rastreamento GPS** com dados de localização e velocidade
- **Interface visual** em display OLED com informações em tempo real
- **Sistema de alertas** baseado em thresholds configuráveis
- **Armazenamento de dados** em cartão SD com formato CSV
- **Interface web** para visualização e configuração remota
- **Sistema multitarefa** com FreeRTOS para processamento eficiente
- **Arquitetura modular** para fácil expansão e manutenção
- **Processamento eficiente** com buffers circulares otimizados
- **Configuração dinâmica** de thresholds via arquivo config.txt

---

## 🔍 Troubleshooting

### Problemas Comuns

1. **Sensor não detectado**: Verifique as conexões I2C e tensão de alimentação
2. **Leituras instáveis**: Aguarde a calibração automática (500ms)
3. **Display não funciona**: Confirme endereço I2C e conexões
4. **Compilação falha**: Verifique versão do Pico SDK e compilador C++
5. **GPS não conecta**: Verifique conexões UART e antena
6. **Cartão SD não reconhece**: Formate em FAT32 e verifique conexões SPI
7. **Buzzer não funciona**: Verifique conexão PWM e configuração de frequência
8. **Interface web não carrega**: Verifique arquivos HTML/CSS/JS na pasta web/

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
