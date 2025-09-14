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

## 📚 Download das Bibliotecas Necessárias

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
git clone https://github.com/FreeRTOS/FreeRTOS-Plus-FAT.git lib/sd
cd lib/sd
git checkout main
```

## ⚙️ Modificações Necessárias na Biblioteca SD

### 1. **IMPORTANTE: Modificação de Mutexes na Biblioteca SD**

**Esta é uma modificação CRÍTICA necessária para a compilação funcionar corretamente:**

Na biblioteca FreeRTOS+FAT+CLI, é necessário substituir todas as chamadas de `xCreateMutexStatic` por `xCreateMutex` e remover o último argumento (buffer de memória estática).

**Arquivos que precisam ser modificados:**
- `lib/sd/src/FreeRTOS+FAT+CLI/src/` (vários arquivos .c)
- `lib/sd/src/Lab-Project-FreeRTOS-FAT/` (vários arquivos .c)

**Modificação necessária:**
```c
// ANTES (não funciona):
xCreateMutexStatic(pxMutex, pcName, pxMutexBuffer)

// DEPOIS (correto):
xCreateMutex(pxMutex, pcName)
```

**Por que essa modificação é necessária:**
- `xCreateMutexStatic` requer que o usuário forneça um buffer de memória estática
- `xCreateMutex` aloca dinamicamente a memória necessária
- A versão estática não é compatível com a configuração atual do FreeRTOS

**⚠️ ATENÇÃO:** Mutexes criados com `xCreateMutex` não podem ser utilizados em rotinas de serviço de interrupção (ISRs). Para sincronização em ISRs, use semáforos binários.

**Script para automatizar a modificação (Linux/macOS):**
```bash
#!/bin/bash
# Script para substituir xCreateMutexStatic por xCreateMutex na biblioteca SD

echo "Modificando biblioteca SD..."

# Encontrar e substituir xCreateMutexStatic por xCreateMutex
find lib/sd -name "*.c" -type f -exec sed -i 's/xCreateMutexStatic/xCreateMutex/g' {} \;

# Remover o último argumento (buffer de memória estática)
# Este comando remove o terceiro argumento das chamadas xCreateMutex
find lib/sd -name "*.c" -type f -exec sed -i 's/xCreateMutex(\([^,]*\),\([^,]*\),[^)]*)/xCreateMutex(\1,\2)/g' {} \;

echo "Modificação concluída!"
```

**Script para Windows (PowerShell):**
```powershell
# Script para substituir xCreateMutexStatic por xCreateMutex na biblioteca SD

Write-Host "Modificando biblioteca SD..."

# Encontrar e substituir xCreateMutexStatic por xCreateMutex
Get-ChildItem -Path "lib\sd" -Filter "*.c" -Recurse | ForEach-Object {
    (Get-Content $_.FullName) -replace 'xCreateMutexStatic', 'xCreateMutex' | Set-Content $_.FullName
}

# Remover o último argumento (buffer de memória estática)
Get-ChildItem -Path "lib\sd" -Filter "*.c" -Recurse | ForEach-Object {
    (Get-Content $_.FullName) -replace 'xCreateMutex\(([^,]+),([^,]+),[^)]+\)', 'xCreateMutex($1,$2)' | Set-Content $_.FullName
}

Write-Host "Modificação concluída!"
```

### 2. Configuração do FreeRTOSFATConfig.h

O arquivo `include/FreeRTOSFATConfig.h` já está configurado corretamente para este projeto, mas se você precisar fazer modificações, certifique-se de que as seguintes configurações estejam definidas:

```c
#define ffconfigHAS_CWD 1
#define ffconfigCWD_THREAD_LOCAL_INDEX 1
#define ffconfigLFN_SUPPORT 1
#define ffconfigTIME_SUPPORT 1
#define ffconfigREMOVABLE_MEDIA 1
#define ffconfigMOUNT_FIND_FREE 1
#define ffconfigWRITE_BOTH_FATS 1
#define ffconfigCACHE_WRITE_THROUGH 1
```

### 3. Configuração do Hardware (hw_config.c)

O arquivo `src/drivers/sd_card/hw_config.c` deve ser configurado para o seu hardware específico. As principais configurações incluem:

- **Pinos SPI para o cartão SD**
- **Configuração de clock**
- **Configuração de DMA**

### 4. Configuração do FreeRTOSConfig.h

O arquivo `include/FreeRTOSConfig.h` já está configurado, mas verifique se as seguintes configurações estão adequadas:

```c
#define configTOTAL_HEAP_SIZE (128*1024)
#define configMAX_PRIORITIES 32
#define configUSE_TIMERS 1
#define configSUPPORT_PICO_SYNC_INTEROP 1
#define configSUPPORT_PICO_TIME_INTEROP 1
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
