## 🚀 Instalação e Configuração

### 1. Instalação do Raspberry Pi Pico SDK

#### Windows:

```bash
# Clone o SDK do Pico
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# Configure a variável de ambiente
setx PICO_SDK_PATH "C:\caminho\para\pico-sdk"
```

#### Linux/macOS:

```bash
# Clone o SDK do Pico
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init

# Configure a variável de ambiente
echo 'export PICO_SDK_PATH=/caminho/para/pico-sdk' >> ~/.bashrc
source ~/.bashrc
```

### 2. Instalação do Toolchain ARM

#### Windows:

- Baixe o ARM GNU Toolchain do site oficial
- Instale e adicione ao PATH do sistema

#### Linux:

```bash
sudo apt update
sudo apt install gcc-arm-none-eabi
```

#### macOS:

```bash
brew install arm-none-eabi-gcc
```

### 3. Instalação do CMake

#### Windows:

- Baixe do site oficial: https://cmake.org/download/
- Instale e adicione ao PATH

#### Linux:

```bash
sudo apt install cmake
```

#### macOS:

```bash
brew install cmake
```

## Dependências/ações necessárias para funcionamento do código

### 1. FreeRTOS Kernel

```bash
# Clone o FreeRTOS Kernel
git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git lib/FreeRTOS
cd lib/FreeRTOS
git checkout V10.6.1
```

### 2. FreeRTOS+FAT+CLI (Biblioteca SD)

```bash
# Clone a biblioteca FreeRTOS+FAT+CLI
git clone https://github.com/carlk3/FreeRTOS-FAT-CLI-for-RPi-Pico lib/sd
cd lib/sd
git submodule update --init
```

### 3. Coloque o inicio de caminho para lib/sd/src/FreeRTOS+FAT+CLI/CMakeLists.txt

Faça como essa PR: https://github.com/carlk3/FreeRTOS-FAT-CLI-for-RPi-Pico/pull/49

### 4. Vá no arquivo lib/FreeRTOS/portable/ThirdParty/GCC/RP2040/include/portmacro.h e troque:

```c
    #define portCHECK_IF_IN_ISR() ({ \
    uint32_t ulIPSR;                                                  \
    __asm volatile ("mrs %0, IPSR" : "=r" (ulIPSR)::);             \
    ((uint8_t)ulIPSR)>0;})
```

por

```c
    static inline uint8_t portCHECK_IF_IN_ISR_impl(void) {
        uint32_t ulIPSR;
        __asm volatile ("mrs %0, IPSR" : "=r" (ulIPSR) ::);
        return (uint8_t)ulIPSR > 0u;
    }

    /* Keep the original macro name for compatibility */
    #define portCHECK_IF_IN_ISR() (portCHECK_IF_IN_ISR_impl())
```

### 5. Remova todas as importações do sdk 2.2.0 sobre runtime_init.h

~/.pico-sdk/sdk/2.0.0/src/rp2_common/hardware_irq/irq.c
~/.pico-sdk/sdk/2.0.0/src/common/pico_sync/mutex.c
~/.pico-sdk/sdk/2.0.0/src/common/pico_time/time.c

```c
#include "pico/runtime_init.h"
```

### 6. Vá até o arquivo lib/FreeRTOS/portable/ThirdParty/GCC/RP2040/include/portmacro.h e coloque, nas linhas com warning:

```c
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
// linha problemática
#pragma GCC diagnostic pop
```

### 7. Remova um ; do arquivo ~/.pico-sdk/sdk/2.0.0/src/common/pico_util/datetime.c após a definição de uma função

```c
void datetime_to_str(char *buf, uint buf_size, const datetime_t *t) {
    snprintf(buf,
             buf_size,
             "%s %d %s %d:%02d:%02d %d",
             DATETIME_DOWS[t->dotw],
             t->day,
             DATETIME_MONTHS[t->month - 1],
             t->hour,
             t->min,
             t->sec,
             t->year);
}; // <--- remova esse ;

```

### 8. Remova a importação de ~/.pico-sdk/sdk/2.0.0/src/rp2_common/hardware_watchdog/watchdog.c:

```c
#include "pico/bootrom.h"
```

### 9. No arquivo lib/FreeRTOS/event_groups.c, faça o seguinte:

Na linha:

```c
volatile size_t xSize = sizeof( StaticEventGroup_t );
```

acrescente abaixo:

```c
xSize;
```

Na outra linha>

```c
volatile size_t xSize = sizeof( StaticStreamBuffer_t );
```

acrescente abaixo:

```c
xSize;
```

## 🔧 Compilação do Projeto

### 1. Preparação do Ambiente de Build

```bash
# Navegue até o diretório do projeto
cd tracking-trilha

# Crie o diretório de build
mkdir build
cd build
```

### 2. Configuração do CMake

#### Windows (PowerShell):

```powershell
cmake -G "MinGW Makefiles" -DPICO_SDK_PATH=$env:PICO_SDK_PATH ..
```

#### Linux/macOS:

```bash
cmake -DPICO_SDK_PATH=$PICO_SDK_PATH ..
```

### 3. **Checklist de Modificações Obrigatórias**

Antes de compilar, certifique-se de que as seguintes modificações foram feitas:

- [ ] **Biblioteca SD modificada**: Todas as chamadas `xCreateMutexStatic` foram substituídas por `xCreateMutex`
- [ ] **Argumentos removidos**: O último argumento (buffer de memória estática) foi removido de cada chamada
- [ ] **FreeRTOS baixado**: A biblioteca FreeRTOS está na pasta `lib/FreeRTOS/`
- [ ] **SD baixada**: A biblioteca FreeRTOS+FAT+CLI está na pasta `lib/sd/`
- [ ] **Variáveis de ambiente**: `PICO_SDK_PATH` está configurada corretamente

### 4. Compilação

```bash
# Compile o projeto
make -j4

# Ou no Windows com MinGW
mingw32-make -j4
```

### 5. Arquivos Gerados

Após a compilação bem-sucedida, os seguintes arquivos serão gerados no diretório `build/`:

- `tracking-trilha.uf2` - Arquivo para upload no Pico
- `tracking-trilha.elf` - Arquivo executável
- `tracking-trilha.hex` - Arquivo em formato hexadecimal
- `tracking-trilha.bin` - Arquivo binário

## 📤 Upload para o Raspberry Pi Pico

### 1. Preparação do Pico

1. Mantenha pressionado o botão BOOTSEL
2. Conecte o Pico ao computador via USB
3. Solte o botão BOOTSEL
4. O Pico aparecerá como um drive USB (RPI-RP2)

### 2. Upload do Firmware

1. Copie o arquivo `tracking-trilha.uf2` para o drive RPI-RP2
2. O Pico reiniciará automaticamente e executará o programa

## 🔌 Conexões do Hardware

### Sensor MAX3010X (Oxímetro)

- VCC → 3.3V
- GND → GND
- SDA → GP0 (I2C)
- SCL → GP1 (I2C)

### Sensor IMU6050 (Acelerômetro)

- VCC → 3.3V
- GND → GND
- SDA → GP0 (I2C)
- SCL → GP1 (I2C)

### Display OLED SSD1306

- VCC → 3.3V
- GND → GND
- SDA → GP0 (I2C)
- SCL → GP1 (I2C)

### Cartão SD

- VCC → 3.3V
- GND → GND
- MISO → GP16 (SPI)
- MOSI → GP19 (SPI)
- SCK → GP18 (SPI)
- CS → GP17 (SPI)

## 🐛 Solução de Problemas

### Erro de Compilação: "PICO_SDK_PATH not found"

```bash
# Verifique se a variável de ambiente está configurada
echo $PICO_SDK_PATH  # Linux/macOS
echo %PICO_SDK_PATH% # Windows
```

### Erro de Compilação: "FreeRTOS not found"

```bash
# Verifique se o FreeRTOS foi baixado corretamente
ls lib/FreeRTOS/
```

### Erro de Compilação: "SD library not found"

```bash
# Verifique se a biblioteca SD foi baixada
ls lib/sd/
```

### Erro de Compilação: "xCreateMutexStatic not declared"

**Este é um erro comum e crítico!**

Se você encontrar erros relacionados a `xCreateMutexStatic`, significa que a modificação na biblioteca SD não foi feita corretamente.

**Solução:**

1. Localize todos os arquivos `.c` na pasta `lib/sd/` que contenham `xCreateMutexStatic`
2. Substitua todas as ocorrências por `xCreateMutex`
3. Remova o último argumento (buffer de memória estática) de cada chamada

**Comando para encontrar os arquivos:**

```bash
# Linux/macOS
grep -r "xCreateMutexStatic" lib/sd/

# Windows (PowerShell)
Select-String -Path "lib\sd\*" -Pattern "xCreateMutexStatic" -Recurse
```

**Exemplo de correção:**

```c
// Encontre esta linha:
pxMutex = xCreateMutexStatic(&xMutex, "SD_Mutex", &xMutexBuffer);

// Substitua por:
pxMutex = xCreateMutex(&xMutex, "SD_Mutex");
```

### Problemas com Cartão SD

1. Certifique-se de que o cartão está formatado em FAT32
2. Verifique as conexões dos pinos SPI
3. Teste com um cartão SD diferente

### Problemas de Memória

Se houver erros de memória insuficiente:

1. Aumente `configTOTAL_HEAP_SIZE` no FreeRTOSConfig.h
2. Reduza o tamanho dos buffers no código
3. Otimize o uso de variáveis globais

## 📊 Funcionalidades do Sistema

- **Monitoramento de Frequência Cardíaca**: Usando sensor MAX3010X
- **Monitoramento de SpO2**: Oxigenação do sangue
- **Monitoramento de Temperatura**: Temperatura corporal
- **Monitoramento de Movimento**: Acelerômetro e giroscópio
- **Display OLED**: Visualização dos dados em tempo real
- **Armazenamento em SD**: Log dos dados coletados
- **Sistema de Análise**: Classificação dos dados em categorias

## 📁 Estrutura do Projeto

```
tracking-trilha/
├── include/                 # Arquivos de cabeçalho
│   ├── drivers/            # Drivers de hardware
│   ├── sensors/            # Interfaces dos sensores
│   ├── state/              # Máquinas de estado
│   └── utils/              # Utilitários
├── src/                    # Código fonte
├── lib/                    # Bibliotecas externas
│   ├── FreeRTOS/          # FreeRTOS Kernel
│   └── sd/                # FreeRTOS+FAT+CLI
├── build/                  # Arquivos compilados
└── CMakeLists.txt         # Configuração do CMake
```

## 🤝 Contribuição

1. Faça um fork do projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

---

**Nota**: Este projeto foi desenvolvido para fins educacionais e de pesquisa. Certifique-se de seguir as boas práticas de segurança ao trabalhar com dados de saúde.
