#include "state_collect.h"
#include "oximeter.h"
#include "accelerometer.h"
#include "hardware/clocks.h"
#include "../../include/utils/debug_printf.h"
#include <string.h>
#include <stdlib.h>

#define MAX_CONFIG_LINE_LENGTH 1024
#define MAX_THRESHOLDS 5
#define MAX_ANALYZERS 12

// Função para ler configurações do arquivo config.txt
bool readAnalyzerConfigs(SD* sd, analyzerConfig_t* configs, int* configCount, int maxConfigs) {
    PRINTF_DEBUG("Reading analyzer configs\n");
    if (!sd || !sd->isReady()) {
        PRINTF_DEBUG("SD card not ready\n");
        return false;
    }
    
    if (!sd->fileExists("config.txt")) {
        PRINTF_DEBUG("config.txt not found\n");
        return false;
    }
    
    // Obter tamanho do arquivo
    size_t fileSize = sd->getFileSize("config.txt");
    PRINTF_DEBUG("Config file size: %d bytes\n", fileSize);
    
    if (fileSize == 0) {
        PRINTF_DEBUG("Config file is empty\n");
        return false;
    }
    
    *configCount = 0;
    size_t offset = 0;
    const size_t BUFFER_SIZE = 1024; // Buffer menor para leitura em chunks
    char buffer[BUFFER_SIZE];
    
    PRINTF_DEBUG("SENSOR_TYPE_QTT: %d, SAMPLE_TYPE_QTT: %d\n", SENSOR_TYPE_QTT, SAMPLE_TYPE_QTT);
    
    // Ler arquivo em chunks
    while (offset < fileSize && *configCount < maxConfigs) {
        size_t bytesToRead = (fileSize - offset > BUFFER_SIZE - 1) ? BUFFER_SIZE - 1 : fileSize - offset;
        size_t bytesRead = 0;
        
        if (!sd->readFromFileWithOffset("config.txt", buffer, bytesToRead + 1, offset, &bytesRead)) {
            PRINTF_DEBUG("Failed to read config chunk at offset %d\n", offset);
            break;
        }
        
        PRINTF_DEBUG("Read %d bytes at offset %d: %s\n", bytesRead, offset, buffer);
        
        // Encontrar o último \n no buffer para determinar onde parar o processamento
        size_t lastNewlinePos = 0;
        bool foundNewline = false;
        for (size_t i = bytesRead; i > 0; i--) {
            if (buffer[i-1] == '\n') {
                lastNewlinePos = i-1;
                foundNewline = true;
                PRINTF_DEBUG("Found newline at position %d\n", lastNewlinePos);
                break;
            }
        }
        
        // Se encontrou \n, processar apenas até essa posição
        size_t processLength = foundNewline ? lastNewlinePos + 1 : bytesRead;
        
        // Temporariamente terminar o buffer no último \n para strtok funcionar corretamente
        char originalChar = buffer[processLength];
        buffer[processLength] = '\0';
        
        PRINTF_DEBUG("Processing %d bytes (up to last \\n): %s\n", processLength, buffer);
        
        // Processar linhas no buffer (apenas até o último \n)
        char* lineStart = buffer;
        char* lineEnd = buffer;
        
        while (lineStart < buffer + processLength && *configCount < maxConfigs) {
            // Encontrar o próximo \n ou fim do buffer
            lineEnd = lineStart;
            while (lineEnd < buffer + processLength && *lineEnd != '\n') {
                lineEnd++;
            }
            
            // Criar uma cópia da linha para processar
            size_t lineLength = lineEnd - lineStart;
            char line[256];
            if (lineLength >= sizeof(line)) {
                lineLength = sizeof(line) - 1;
            }
            strncpy(line, lineStart, lineLength);
            line[lineLength] = '\0';
            
            PRINTF_DEBUG("Processing line: '%s'\n", line);
            
            // Pular linhas vazias ou comentários
            // Remover espaços em branco do início e fim da linha
            char* linePtr = line;
            while (*linePtr == ' ' || *linePtr == '\t' || *linePtr == '\r') {
                linePtr++;
            }
            
            if (strlen(linePtr) == 0 || linePtr[0] == '#' || linePtr[0] == '\n' || linePtr[0] == '\0') {
                PRINTF_DEBUG("Skipping empty/comment line\n");
                lineStart = lineEnd + 1;
                continue;
            }
            
            // Parse da linha: sensorType,sampleType,threshold1,threshold2,threshold3,threshold4,threshold5
            char* token = strtok(linePtr, ",");
            if (token == NULL) {
                PRINTF_DEBUG("Invalid config line: %s\n", linePtr);
                lineStart = lineEnd + 1;
                continue;
            }
            
            // Parse sensorType
            int sensorTypeInt = atoi(token);
            PRINTF_DEBUG("Parsed sensor type: %d\n", sensorTypeInt);
            if (sensorTypeInt < 0 || sensorTypeInt >= SENSOR_TYPE_QTT) {
                PRINTF_DEBUG("Invalid sensor type: %d (max: %d)\n", sensorTypeInt, SENSOR_TYPE_QTT-1);
                lineStart = lineEnd + 1;
                continue;
            }
            
            // Parse sampleType
            token = strtok(NULL, ",");
            if (token == NULL) {
                PRINTF_DEBUG("Missing sample type in line: %s\n", linePtr);
                lineStart = lineEnd + 1;
                continue;
            }
            
            int sampleTypeInt = atoi(token);
            PRINTF_DEBUG("Parsed sample type: %d\n", sampleTypeInt);
            if (sampleTypeInt < 0 || sampleTypeInt >= SAMPLE_TYPE_QTT) {
                PRINTF_DEBUG("Invalid sample type: %d (max: %d)\n", sampleTypeInt, SAMPLE_TYPE_QTT-1);
                lineStart = lineEnd + 1;
                continue;
            }
            
            // Parse thresholds
            float thresholds[MAX_THRESHOLDS] = {0.0f};
            int thresholdCount = 0;
            
            for (int i = 0; i < MAX_THRESHOLDS; i++) {
                token = strtok(NULL, ",");
                if (token == NULL) {
                    break; // Não há mais thresholds
                }
                thresholds[i] = atof(token);
                thresholdCount++;
            }
            
            // Criar o analyzerConfig
            analyzerConfig_t config;
            config.sensorType = (sensor_t)sensorTypeInt;
            config.sampleType = (sample_t)sampleTypeInt;
            
            // Copiar thresholds
            for (int i = 0; i < MAX_THRESHOLDS; i++) {
                config.thresholds[i] = thresholds[i];
            }
            
            configs[*configCount] = config;
            (*configCount)++;
            
            PRINTF_DEBUG("Loaded config: sensor=%d, sample=%d, thresholds=[%.2f,%.2f,%.2f,%.2f,%.2f]\n",
                        sensorTypeInt, sampleTypeInt, 
                        thresholds[0], thresholds[1], thresholds[2], thresholds[3], thresholds[4]);
            
            // Avançar para próxima linha
            lineStart = lineEnd + 1;
        }
        
        // Restaurar o caractere original
        buffer[processLength] = originalChar;
        
        // Atualizar offset para próxima leitura
        // Se encontrou \n, avançar até após o último \n processado
        // Se não encontrou \n, avançar todo o buffer lido (último chunk)
        if (foundNewline) {
            offset += lastNewlinePos + 1; // Avançar até após o último \n
            PRINTF_DEBUG("Found newline at position %d, advancing offset to %d\n", lastNewlinePos, offset);
        } else {
            offset += bytesRead; // Último chunk, avançar tudo
            PRINTF_DEBUG("No newline found, advancing offset to %d (end of file)\n", offset);
        }
    }
    
    PRINTF_DEBUG("Loaded %d analyzer configurations\n", *configCount);
    return true;
}


// insert here all wanted_samples
sample_t StateCollect::wanted_samples[] = {
    SAMPLE_TYPE_SPO2,
    SAMPLE_TYPE_HEART_RATE,
    SAMPLE_TYPE_TEMPERATURE,
    SAMPLE_TYPE_ACCEL
};

Oled* StateCollect::oled = nullptr;
SD* StateCollect::sd = nullptr;
Buzzer* StateCollect::buzzer = nullptr;
char StateCollect::samples_filename[SAMPLES_FILENAME_SIZE] = SAMPLES_FILE_NAME;


StateCollect::StateCollect() : State() {
   // Initialize FreeRTOS components
   taskHandle = nullptr;
   taskRunning = false;
}

StateCollect::~StateCollect() {
   StopTask();
}

void StateCollect::PrintOled(int line_index, const char* text) {
    if (oled != nullptr) {
        oled->PrintText(line_index, text);
    }
}

void StateCollect::Update() {
    // This method is now deprecated - use StartTask() instead
    // For backward compatibility, call UpdateInternal directly
    UpdateInternal();
}

void StateCollect::UpdateInternal() {
    PrintOled(0, "Coletando...     ");
    
    for (size_t i = 0; i < SENSOR_TYPE_QTT; i++) {
        if (sensorArray[i] != nullptr) {
            // Skip oximeter update as it runs in its own task
            if (i != SENSOR_TYPE_OXIMETER) {
                sensorArray[i]->Update();
            }
        }
    }
    
    PRINTF_DEBUG("Collect Time: %llu\n", get_absolute_time());
    char sd_data_str[32];
    
    PRINTF_DEBUG("Collect Time: %llu\n", get_absolute_time());
    // Print wanted_samples
    for (size_t sensor_type = 0; sensor_type < SENSOR_TYPE_QTT; sensor_type++) {
        Sensor* sensor = GetSensor((sensor_t)sensor_type);
        if (sensor != nullptr) {
            Data_t data;
            for (size_t sample_index = 0; sample_index < SAMPLE_TYPE_QTT; sample_index++) {
              Analyzer* analyzer = GetAnalyzer((sensor_t)sensor_type, StateCollect::wanted_samples[sample_index]);
              data.type = StateCollect::wanted_samples[sample_index];
              // PRINTF_DEBUG("Trying to get data for sensor %d, sample type %d\n", sensor_type, data.type);
              if (sensor->getData(&data)) {
                  char data_str[17];
                  sprintf(data_str, "S%2d T%2d V%2.1f", sensor_type, data.type, data.data[0]);
                  PrintOled(sample_index + 1, data_str);
                //printf("Sensor Type: %d, Sample Type: %d, Data: ", sensor_type, data.type);
                  for (size_t buffer_index = 0; buffer_index < data.size; buffer_index++) {
                      //printf("%.3f ", data.data[buffer_index]);
                  }
                  if (analyzer != nullptr) {
                      healthStatus_t healthStatus = analyzer->Analyze(&data);
                      char health_status_str[17];
                      sprintf(health_status_str, "H%2d", healthStatus);
                      PrintOled(7, health_status_str);
                      //printf("Health Status: %d\n", healthStatus);
                      sprintf(sd_data_str, "%d,%d,%llu,%f,%d\n", sensor_type, data.type, data.timestamp, data.data[0], healthStatus);
                      if (sd != nullptr) {
                          sd->appendToFile(samples_filename, sd_data_str);
                      }
                  } else {
                    //printf("No analyzer found for sensor type: %d\n", sensor_type);
                  }
                  //printf("\n");
              }
          }
        }
    }

    if (oled != nullptr) {
        oled->Render();
    }
}

void StateCollect::Pause() {
    // Pause implementation
}

void StateCollect::Resume() {
    // Resume implementation
}

void StateCollect::StartTask() {
    if (taskHandle == nullptr) {
        taskRunning = true;
        BaseType_t result = xTaskCreate(
            StateTask,
            "StateTask",
            STATE_TASK_STACK_SIZE,
            this,
            STATE_TASK_PRIORITY,
            &taskHandle
        );
        
        if (result != pdPASS) {
            PRINTF_DEBUG("Failed to create State task\n");
            taskRunning = false;
            taskHandle = nullptr;
        } else {
            PRINTF_DEBUG("State task created successfully\n");
        }
    }
}

void StateCollect::StopTask() {
    if (taskHandle != nullptr) {
        taskRunning = false;
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
        PRINTF_DEBUG("State task stopped\n");
    }
}

void StateCollect::StateTask(void* pvParameters) {
    StateCollect* stateCollect = static_cast<StateCollect*>(pvParameters);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    if (sd != nullptr) {
        if (!sd->isReady()) {
            sd->initialize();
            sd->mount();
        }
        if (!sd->fileExists(samples_filename)) {
            sd->writeToFile(samples_filename, SAMPLES_FILE_HEADER);
        }
    }

    // Configurar analyzers a partir do arquivo config.txt
    analyzerConfig_t analyzerConfigs[MAX_ANALYZERS];
    int configCount = 0;
    Analyzer* analyzers[MAX_ANALYZERS];
    
    if (sd != nullptr && readAnalyzerConfigs(sd, analyzerConfigs, &configCount, MAX_ANALYZERS)) {
        PRINTF_DEBUG("Successfully loaded %d analyzer configurations from config.txt\n", configCount);
        
        // Criar analyzers dinamicamente
        for (int i = 0; i < configCount; i++) {
            analyzers[i] = new Analyzer(analyzerConfigs[i]);
            stateCollect->AddAnalyzer(analyzers[i]);
        }
    } else {
        PRINTF_DEBUG("Failed to read analyzer configs, using defaults\n");
        
        // Configurações padrão caso não consiga ler o arquivo
        analyzerConfigs[0] = {
            .thresholds = {0.0f, 0.5f, 0.75f, 1.2f, 1.5f},
            .sensorType = SENSOR_TYPE_ACCELEROMETER,
            .sampleType = SAMPLE_TYPE_ACCEL
        };
        
        analyzerConfigs[1] = {
            .thresholds = {0.0f, 90.0f, 98.0f, 200.0f, 200.0f},
            .sensorType = SENSOR_TYPE_OXIMETER,
            .sampleType = SAMPLE_TYPE_SPO2
        };
        
        analyzerConfigs[2] = {
            .thresholds = {0.0f, 60.0f, 100.0f, 140.0f, 180.0f},
            .sensorType = SENSOR_TYPE_OXIMETER,
            .sampleType = SAMPLE_TYPE_HEART_RATE
        };
        
        analyzerConfigs[3] = {
            .thresholds = {0.0f, 35.0f, 37.0f, 39.0f, 41.0f},
            .sensorType = SENSOR_TYPE_OXIMETER,
            .sampleType = SAMPLE_TYPE_TEMPERATURE
        };
        
#ifdef USE_GPS
        analyzerConfigs[4] = {
            .thresholds = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            .sensorType = SENSOR_TYPE_GPS,
            .sampleType = SAMPLE_TYPE_LATITUDE
        };
        
        analyzerConfigs[5] = {
            .thresholds = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            .sensorType = SENSOR_TYPE_GPS,
            .sampleType = SAMPLE_TYPE_LONGITUDE
        };
        
        analyzerConfigs[6] = {
            .thresholds = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            .sensorType = SENSOR_TYPE_GPS,
            .sampleType = SAMPLE_TYPE_ALTITUDE
        };
        
        analyzerConfigs[7] = {
            .thresholds = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            .sensorType = SENSOR_TYPE_GPS,
            .sampleType = SAMPLE_TYPE_SATELLITES
        };
        
        analyzerConfigs[8] = {
            .thresholds = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
            .sensorType = SENSOR_TYPE_GPS,
            .sampleType = SAMPLE_TYPE_SPEED_KPH
        };
        
        configCount = 9;
#else
        configCount = 4;
#endif
        
        // Criar analyzers com configurações padrão
        for (int i = 0; i < configCount; i++) {
            analyzers[i] = new Analyzer(analyzerConfigs[i]);
            stateCollect->AddAnalyzer(analyzers[i]);
        }
    }
    
    PRINTF_DEBUG("State task started with %d analyzers configured\n", configCount);
    
    while (stateCollect->taskRunning) {
        PRINTF_DEBUG("State task time: %llu\n", get_absolute_time());
        
        // Check stack usage
        UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(nullptr);
        PRINTF_DEBUG("State task stack high water mark: %d\n", stackHighWaterMark);
        
        if (stackHighWaterMark < 100) {
            PRINTF_DEBUG("WARNING: State task stack is running low!\n");
        }
        
        stateCollect->UpdateInternal();
        
        // Wait for the next period
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(STATE_UPDATE_PERIOD_MS));
    }
    
    PRINTF_DEBUG("State task ending\n");
    vTaskDelete(nullptr);
}
