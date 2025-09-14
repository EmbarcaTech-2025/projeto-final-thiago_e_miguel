# 🔬 Visualizador de Dados de Sensores

Sistema web para visualização de dados de sensores e criação de arquivos de configuração para thresholds.

## 🚀 Funcionalidades

### 📊 Visualização de Dados
- **Upload de arquivos CSV** com dados dos sensores
- **Gráficos interativos** com Chart.js
- **Filtros por tipo de sensor e amostra**
- **Resumo estatístico** dos dados (média, min, max, desvio padrão)
- **Visualização do status de saúde** dos sensores

### ⚙️ Editor de Configuração
- **Upload de arquivos config.txt** com configurações de thresholds
- **Editor visual** para modificar configurações
- **Adicionar/remover configurações** dinamicamente
- **Download do arquivo config.txt** atualizado

### 📁 Formatos Suportados

#### Arquivo de Configuração (config.txt)
```
# Configurações dos Analyzers
# Formato: sensorType,sampleType,threshold1,threshold2,threshold3,threshold4,threshold5
# sensorType: 0=OXIMETER, 1=ACCELEROMETER, 2=GPS
# sampleType: 0=SPO2, 1=HEART_RATE, 2=TEMPERATURE, 3=LATITUDE, 4=LONGITUDE, 5=ALTITUDE, 6=SATELLITES, 7=SPEED_KPH, 8=ACCEL

# Oximeter - SpO2
0,0,0.0,90.0,98.0,200.0,200.0

# Oximeter - Heart Rate
0,1,0.0,60.0,100.0,140.0,180.0
```

#### Dados dos Sensores (CSV)
```
sensor_type,sample_type,timestamp,data,health_status
1,8,8370,1.008798,4
1,8,8381,1.003714,4
0,0,13199,86.511078,0
```

## 🛠️ Tecnologias Utilizadas

- **HTML5** - Estrutura da página
- **CSS3** - Estilização responsiva
- **JavaScript (ES6+)** - Lógica da aplicação
- **Chart.js** - Gráficos interativos
- **File API** - Upload e download de arquivos

## 📋 Tipos de Sensores Suportados

### Oximeter (0)
- **SpO2** (0) - Saturação de oxigênio
- **Heart Rate** (1) - Frequência cardíaca
- **Temperature** (2) - Temperatura corporal

### Accelerometer (1)
- **Acceleration** (8) - Dados de aceleração

### GPS (2)
- **Latitude** (3) - Coordenada de latitude
- **Longitude** (4) - Coordenada de longitude
- **Altitude** (5) - Altitude
- **Satellites** (6) - Número de satélites
- **Speed KPH** (7) - Velocidade em km/h

## 🎯 Status de Saúde

- **0 - DANGER** - Perigo (vermelho)
- **1 - WARNING** - Aviso (laranja)
- **2 - GOOD** - Bom (verde claro)
- **3 - EXCELLENT** - Excelente (verde)
- **4 - UNKNOWN** - Desconhecido (cinza)

## 🚀 Como Usar

1. **Abrir o arquivo `index.html`** no navegador
2. **Upload de arquivos:**
   - Faça upload do arquivo `config.txt` para carregar configurações
   - Faça upload do arquivo CSV com dados dos sensores
3. **Editar configurações:**
   - Use o editor visual para modificar thresholds
   - Adicione ou remova configurações conforme necessário
   - Baixe o arquivo atualizado
4. **Visualizar dados:**
   - Use os filtros para focar em tipos específicos de sensores
   - Analise os gráficos e estatísticas
   - Monitore o status de saúde dos sensores

## 📁 Estrutura do Projeto

```
web/
├── index.html          # Página principal
├── styles.css          # Estilos CSS
├── script.js           # Lógica JavaScript
├── config.txt          # Arquivo de configuração de exemplo
├── sensor_data.csv     # Dados de exemplo dos sensores
└── README.md           # Documentação
```

## 🎨 Interface

- **Design responsivo** que funciona em desktop e mobile
- **Interface intuitiva** com drag-and-drop para upload
- **Gráficos interativos** com zoom e pan
- **Notificações** para feedback do usuário
- **Tema moderno** com gradientes e sombras

## 🔧 Funcionalidades Técnicas

- **Parser robusto** para arquivos de configuração e dados
- **Validação de dados** antes do processamento
- **Tratamento de erros** com mensagens informativas
- **Performance otimizada** para grandes volumes de dados
- **Compatibilidade** com navegadores modernos

## 📈 Exemplos de Uso

### Monitoramento de Saúde
- Upload de dados de oximeter para monitorar SpO2 e frequência cardíaca
- Configuração de thresholds para alertas automáticos
- Visualização de tendências temporais

### Análise de Movimento
- Upload de dados de acelerômetro
- Análise de padrões de movimento
- Detecção de atividades anômalas

### Rastreamento GPS
- Upload de dados de localização
- Visualização de trajetórias
- Análise de velocidade e altitude

## 🚀 Melhorias Futuras

- [ ] Suporte a mais tipos de sensores
- [ ] Exportação de relatórios em PDF
- [ ] Integração com APIs de sensores em tempo real
- [ ] Machine learning para detecção de anomalias
- [ ] Dashboard em tempo real
- [ ] Suporte a múltiplos usuários