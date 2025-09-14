// Configurações dos sensores
const SENSOR_TYPES = {
    0: 'OXIMETER',
    1: 'ACCELEROMETER', 
    2: 'GPS'
};

const SAMPLE_TYPES = {
    0: 'SPO2',
    1: 'HEART_RATE',
    2: 'TEMPERATURE',
    3: 'LATITUDE',
    4: 'LONGITUDE',
    5: 'ALTITUDE',
    6: 'SATELLITES',
    7: 'SPEED_KPH',
    8: 'ACCEL'
};

const HEALTH_STATUS = {
    0: 'EXTREMELY LOW',
    1: 'LOW', 
    2: 'NORMAL',
    3: 'HIGH',
    4: 'EXTREMELY HIGH',
};

// Variáveis globais
let configData = [];
let sensorData = [];
let mainChart = null;
let healthStatusChart = null;

// Inicialização
document.addEventListener('DOMContentLoaded', function() {
    initializeEventListeners();
    loadSampleData();
});

function initializeEventListeners() {
    // Upload de arquivos
    document.getElementById('configFile').addEventListener('change', handleConfigFileUpload);
    document.getElementById('dataFile').addEventListener('change', handleDataFileUpload);
    
    // Botões de configuração
    document.getElementById('addConfigBtn').addEventListener('click', addConfigItem);
    document.getElementById('saveConfigBtn').addEventListener('click', saveConfig);
    document.getElementById('downloadConfigBtn').addEventListener('click', downloadConfig);
    
    // Filtros de visualização
    document.getElementById('applyFiltersBtn').addEventListener('click', applyFilters);
    document.getElementById('clearFiltersBtn').addEventListener('click', clearFilters);
}

// Parser para arquivo de configuração
function parseConfigFile(content) {
    const lines = content.split('\n');
    const configs = [];
    
    for (let line of lines) {
        line = line.trim();
        
        // Pular linhas vazias e comentários
        if (!line || line.startsWith('#')) {
            continue;
        }
        
        // Parse da linha de configuração
        const parts = line.split(',');
        if (parts.length === 7) {
            const config = {
                sensorType: parseInt(parts[0]),
                sampleType: parseInt(parts[1]),
                threshold1: parseFloat(parts[2]),
                threshold2: parseFloat(parts[3]),
                threshold3: parseFloat(parts[4]),
                threshold4: parseFloat(parts[5]),
                threshold5: parseFloat(parts[6])
            };
            configs.push(config);
        }
    }
    
    return configs;
}

// Parser para dados dos sensores
function parseSensorData(content) {
    const lines = content.split('\n');
    const data = [];
    
    for (let line of lines) {
        line = line.trim();
        
        // Pular linhas vazias
        if (!line) {
            continue;
        }
        
        // Parse da linha de dados
        const parts = line.split(',');
        if (parts.length === 5) {
            const sensorData = {
                sensorType: parseInt(parts[0]),
                sampleType: parseInt(parts[1]),
                timestamp: parseInt(parts[2]),
                data: parseFloat(parts[3]),
                healthStatus: parseInt(parts[4])
            };
            data.push(sensorData);
        }
    }
    
    return data;
}

// Upload de arquivo de configuração
function handleConfigFileUpload(event) {
    const file = event.target.files[0];
    if (!file) return;
    
    const reader = new FileReader();
    reader.onload = function(e) {
        try {
            configData = parseConfigFile(e.target.result);
            displayConfigEditor();
            showNotification('Arquivo de configuração carregado com sucesso!', 'success');
        } catch (error) {
            showNotification('Erro ao processar arquivo de configuração: ' + error.message, 'error');
        }
    };
    reader.readAsText(file);
}

// Upload de arquivo de dados
function handleDataFileUpload(event) {
    const file = event.target.files[0];
    if (!file) return;
    
    const reader = new FileReader();
    reader.onload = function(e) {
        try {
            sensorData = parseSensorData(e.target.result);
            displayVisualization();
            updateDataSummary();
            showNotification('Dados dos sensores carregados com sucesso!', 'success');
        } catch (error) {
            showNotification('Erro ao processar dados dos sensores: ' + error.message, 'error');
        }
    };
    reader.readAsText(file);
}

// Exibir editor de configuração
function displayConfigEditor() {
    const configSection = document.getElementById('configSection');
    configSection.style.display = 'block';
    
    renderConfigList();
}

// Renderizar lista de configurações
function renderConfigList() {
    const configList = document.getElementById('configList');
    configList.innerHTML = '';
    
    configData.forEach((config, index) => {
        const configItem = createConfigItem(config, index);
        configList.appendChild(configItem);
    });
}

// Criar item de configuração
function createConfigItem(config, index) {
    const div = document.createElement('div');
    div.className = 'config-item';
    div.innerHTML = `
        <select class="sensor-type" data-index="${index}">
            <option value="0" ${config.sensorType === 0 ? 'selected' : ''}>Oximeter</option>
            <option value="1" ${config.sensorType === 1 ? 'selected' : ''}>Accelerometer</option>
            <option value="2" ${config.sensorType === 2 ? 'selected' : ''}>GPS</option>
        </select>
        <select class="sample-type" data-index="${index}">
            <option value="0" ${config.sampleType === 0 ? 'selected' : ''}>SpO2</option>
            <option value="1" ${config.sampleType === 1 ? 'selected' : ''}>Heart Rate</option>
            <option value="2" ${config.sampleType === 2 ? 'selected' : ''}>Temperature</option>
            <option value="3" ${config.sampleType === 3 ? 'selected' : ''}>Latitude</option>
            <option value="4" ${config.sampleType === 4 ? 'selected' : ''}>Longitude</option>
            <option value="5" ${config.sampleType === 5 ? 'selected' : ''}>Altitude</option>
            <option value="6" ${config.sampleType === 6 ? 'selected' : ''}>Satellites</option>
            <option value="7" ${config.sampleType === 7 ? 'selected' : ''}>Speed KPH</option>
            <option value="8" ${config.sampleType === 8 ? 'selected' : ''}>Acceleration</option>
        </select>
        <input type="number" step="0.1" class="threshold1" data-index="${index}" value="${config.threshold1}" placeholder="Threshold 1">
        <input type="number" step="0.1" class="threshold2" data-index="${index}" value="${config.threshold2}" placeholder="Threshold 2">
        <input type="number" step="0.1" class="threshold3" data-index="${index}" value="${config.threshold3}" placeholder="Threshold 3">
        <input type="number" step="0.1" class="threshold4" data-index="${index}" value="${config.threshold4}" placeholder="Threshold 4">
        <input type="number" step="0.1" class="threshold5" data-index="${index}" value="${config.threshold5}" placeholder="Threshold 5">
        <button class="remove-btn" onclick="removeConfigItem(${index})">🗑️</button>
    `;
    
    // Adicionar event listeners para atualização em tempo real
    div.querySelectorAll('input, select').forEach(input => {
        input.addEventListener('change', updateConfigData);
    });
    
    return div;
}

// Adicionar nova configuração
function addConfigItem() {
    const newConfig = {
        sensorType: 0,
        sampleType: 0,
        threshold1: 0.0,
        threshold2: 0.0,
        threshold3: 0.0,
        threshold4: 0.0,
        threshold5: 0.0
    };
    
    configData.push(newConfig);
    renderConfigList();
}

// Remover configuração
function removeConfigItem(index) {
    configData.splice(index, 1);
    renderConfigList();
}

// Atualizar dados de configuração
function updateConfigData(event) {
    const index = parseInt(event.target.dataset.index);
    const field = event.target.className;
    const value = event.target.type === 'number' ? parseFloat(event.target.value) : parseInt(event.target.value);
    
    configData[index][field] = value;
}

// Salvar configuração
function saveConfig() {
    // Validar dados
    for (let i = 0; i < configData.length; i++) {
        const config = configData[i];
        if (isNaN(config.threshold1) || isNaN(config.threshold2) || 
            isNaN(config.threshold3) || isNaN(config.threshold4) || isNaN(config.threshold5)) {
            showNotification('Por favor, preencha todos os thresholds com valores válidos.', 'error');
            return;
        }
    }
    
    showNotification('Configuração salva com sucesso!', 'success');
}

// Download do arquivo de configuração
function downloadConfig() {
    let content = '# Configurações dos Analyzers\n';
    content += '# Formato: sensorType,sampleType,threshold1,threshold2,threshold3,threshold4,threshold5\n';
    content += '# sensorType: 0=OXIMETER, 1=ACCELEROMETER, 2=GPS\n';
    content += '# sampleType: 0=SPO2, 1=HEART_RATE, 2=TEMPERATURE, 3=LATITUDE, 4=LONGITUDE, 5=ALTITUDE, 6=SATELLITES, 7=SPEED_KPH, 8=ACCEL\n\n';
    
    configData.forEach(config => {
        const sensorName = SENSOR_TYPES[config.sensorType];
        const sampleName = SAMPLE_TYPES[config.sampleType];
        content += `# ${sensorName} - ${sampleName}\n`;
        content += `${config.sensorType},${config.sampleType},${config.threshold1},${config.threshold2},${config.threshold3},${config.threshold4},${config.threshold5}\n\n`;
    });
    
    downloadFile(content, 'config.txt', 'text/plain');
}

// Exibir visualização
function displayVisualization() {
    const visualizationSection = document.getElementById('visualizationSection');
    visualizationSection.style.display = 'block';
    
    createCharts();
}

// Criar gráficos
function createCharts() {
    createMainChart();
    createHealthStatusChart();
}

// Gráfico principal
function createMainChart() {
    const ctx = document.getElementById('mainChart').getContext('2d');
    
    if (mainChart) {
        mainChart.destroy();
    }
    
    const filteredData = getFilteredData();
    const datasets = createDatasets(filteredData);
    
    mainChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: filteredData.map(d => new Date(d.timestamp).toLocaleTimeString()),
            datasets: datasets
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                title: {
                    display: true,
                    text: 'Dados dos Sensores ao Longo do Tempo'
                },
                legend: {
                    display: true,
                    position: 'top'
                }
            },
            scales: {
                x: {
                    title: {
                        display: true,
                        text: 'Tempo'
                    }
                },
                y: {
                    title: {
                        display: true,
                        text: 'Valor'
                    }
                }
            }
        }
    });
}

// Gráfico de status de saúde
function createHealthStatusChart() {
    const ctx = document.getElementById('healthStatusChart').getContext('2d');
    
    if (healthStatusChart) {
        healthStatusChart.destroy();
    }
    
    const healthCounts = getHealthStatusCounts();
    const filteredData = getFilteredData();
    
    // Se não há dados filtrados, mostrar mensagem
    if (filteredData.length === 0) {
        healthStatusChart = new Chart(ctx, {
            type: 'doughnut',
            data: {
                labels: ['Nenhum dado'],
                datasets: [{
                    data: [1],
                    backgroundColor: ['#e2e8f0']
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    title: {
                        display: true,
                        text: 'Distribuição do Status de Saúde (Filtrado)'
                    },
                    legend: {
                        display: false
                    }
                }
            }
        });
        return;
    }
    
    healthStatusChart = new Chart(ctx, {
        type: 'doughnut',
        data: {
            labels: Object.keys(healthCounts).map(status => HEALTH_STATUS[status]),
            datasets: [{
                data: Object.values(healthCounts),
                backgroundColor: [
                    '#e53e3e', // EXTREMELY LOW
                    '#ed8936', // LOW
                    '#68d391', // NORMAL
                    '#48bb78', // HIGH
                    '#a0aec0'  // EXTREMELY HIGH
                ]
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            plugins: {
                title: {
                    display: true,
                    text: 'Distribuição do Status de Saúde (Filtrado)'
                },
                legend: {
                    display: true,
                    position: 'bottom'
                }
            }
        }
    });
}

// Criar datasets para o gráfico principal
function createDatasets(data) {
    const datasets = [];
    const sensorGroups = {};
    
    // Agrupar dados por tipo de sensor e amostra
    data.forEach(d => {
        const key = `${d.sensorType}-${d.sampleType}`;
        if (!sensorGroups[key]) {
            sensorGroups[key] = [];
        }
        sensorGroups[key].push(d);
    });
    
    // Criar dataset para cada grupo
    Object.keys(sensorGroups).forEach((key, index) => {
        const group = sensorGroups[key];
        const sensorType = group[0].sensorType;
        const sampleType = group[0].sampleType;
        
        datasets.push({
            label: `${SENSOR_TYPES[sensorType]} - ${SAMPLE_TYPES[sampleType]}`,
            data: group.map(d => d.data),
            borderColor: getColor(index),
            backgroundColor: getColor(index, 0.1),
            tension: 0.1,
            fill: false
        });
    });
    
    return datasets;
}

// Obter dados filtrados
function getFilteredData() {
    let filtered = sensorData;
    
    const sensorTypeFilter = document.getElementById('sensorTypeFilter').value;
    const sampleTypeFilter = document.getElementById('sampleTypeFilter').value;
    
    if (sensorTypeFilter !== 'all') {
        filtered = filtered.filter(d => d.sensorType === parseInt(sensorTypeFilter));
    }
    
    if (sampleTypeFilter !== 'all') {
        filtered = filtered.filter(d => d.sampleType === parseInt(sampleTypeFilter));
    }
    
    return filtered;
}

// Contar status de saúde
function getHealthStatusCounts() {
    const counts = {};
    const filteredData = getFilteredData();
    filteredData.forEach(d => {
        counts[d.healthStatus] = (counts[d.healthStatus] || 0) + 1;
    });
    return counts;
}

// Aplicar filtros
function applyFilters() {
    createCharts();
    updateDataSummary();
}

// Limpar filtros
function clearFilters() {
    document.getElementById('sensorTypeFilter').value = 'all';
    document.getElementById('sampleTypeFilter').value = 'all';
    applyFilters();
}

// Atualizar resumo dos dados
function updateDataSummary() {
    const summary = document.getElementById('dataSummary');
    const filteredData = getFilteredData();
    
    if (filteredData.length === 0) {
        summary.innerHTML = '<p>Nenhum dado disponível</p>';
        return;
    }
    
    const stats = calculateStatistics(filteredData);
    
    summary.innerHTML = `
        <div class="summary-item">
            <h4>Total de Amostras</h4>
            <div class="value">${filteredData.length}</div>
        </div>
        <div class="summary-item">
            <h4>Valor Médio</h4>
            <div class="value">${stats.mean.toFixed(2)}</div>
        </div>
        <div class="summary-item">
            <h4>Valor Mínimo</h4>
            <div class="value">${stats.min.toFixed(2)}</div>
        </div>
        <div class="summary-item">
            <h4>Valor Máximo</h4>
            <div class="value">${stats.max.toFixed(2)}</div>
        </div>
        <div class="summary-item">
            <h4>Desvio Padrão</h4>
            <div class="value">${stats.stdDev.toFixed(2)}</div>
        </div>
        <div class="summary-item">
            <h4>Período (min)</h4>
            <div class="value">${stats.duration.toFixed(1)}</div>
        </div>
    `;
}

// Calcular estatísticas
function calculateStatistics(data) {
    if (data.length === 0) {
        return { mean: 0, min: 0, max: 0, stdDev: 0, duration: 0 };
    }
    
    const values = data.map(d => d.data);
    const mean = values.reduce((a, b) => a + b, 0) / values.length;
    const min = Math.min(...values);
    const max = Math.max(...values);
    
    const variance = values.reduce((acc, val) => acc + Math.pow(val - mean, 2), 0) / values.length;
    const stdDev = Math.sqrt(variance);
    
    const timestamps = data.map(d => d.timestamp);
    const duration = (Math.max(...timestamps) - Math.min(...timestamps)) / 1000 / 60; // em minutos
    
    return { mean, min, max, stdDev, duration };
}

// Utilitários
function getColor(index, alpha = 1) {
    const colors = [
        `rgba(102, 126, 234, ${alpha})`,
        `rgba(118, 75, 162, ${alpha})`,
        `rgba(56, 178, 172, ${alpha})`,
        `rgba(245, 101, 101, ${alpha})`,
        `rgba(237, 137, 54, ${alpha})`,
        `rgba(72, 187, 120, ${alpha})`
    ];
    return colors[index % colors.length];
}

function downloadFile(content, filename, mimeType) {
    const blob = new Blob([content], { type: mimeType });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
}

function showNotification(message, type = 'info') {
    // Criar elemento de notificação
    const notification = document.createElement('div');
    notification.className = `notification notification-${type}`;
    notification.textContent = message;
    
    // Estilos
    notification.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        padding: 15px 20px;
        border-radius: 8px;
        color: white;
        font-weight: 600;
        z-index: 1000;
        max-width: 300px;
        box-shadow: 0 4px 12px rgba(0,0,0,0.15);
        transform: translateX(100%);
        transition: transform 0.3s ease;
    `;
    
    // Cores por tipo
    const colors = {
        success: '#48bb78',
        error: '#e53e3e',
        warning: '#ed8936',
        info: '#667eea'
    };
    notification.style.backgroundColor = colors[type] || colors.info;
    
    document.body.appendChild(notification);
    
    // Animar entrada
    setTimeout(() => {
        notification.style.transform = 'translateX(0)';
    }, 100);
    
    // Remover após 3 segundos
    setTimeout(() => {
        notification.style.transform = 'translateX(100%)';
        setTimeout(() => {
            if (notification.parentNode) {
                notification.parentNode.removeChild(notification);
            }
        }, 300);
    }, 3000);
}

// Carregar dados de exemplo
function loadSampleData() {
    // Dados de exemplo para demonstração
    const sampleConfig = [
        { sensorType: 0, sampleType: 0, threshold1: 0.0, threshold2: 90.0, threshold3: 98.0, threshold4: 200.0, threshold5: 300.0 },
        { sensorType: 0, sampleType: 1, threshold1: 0.0, threshold2: 60.0, threshold3: 100.0, threshold4: 140.0, threshold5: 180.0 },
        { sensorType: 0, sampleType: 2, threshold1: 0.0, threshold2: 35.0, threshold3: 37.0, threshold4: 39.0, threshold5: 42.0 },
        { sensorType: 1, sampleType: 8, threshold1: 0.0, threshold2: 0.5, threshold3: 0.75, threshold4: 1.2, threshold5: 2.0 }
    ];
    
    configData = sampleConfig;
    displayConfigEditor();
    
    // Dados de exemplo dos sensores
    const sampleSensorData = [
        { sensorType: 1, sampleType: 8, timestamp: 8370, data: 1.008798, healthStatus: 4 },
        { sensorType: 1, sampleType: 8, timestamp: 8381, data: 1.003714, healthStatus: 4 },
        { sensorType: 1, sampleType: 8, timestamp: 8398, data: 1.006280, healthStatus: 4 },
        { sensorType: 0, sampleType: 0, timestamp: 13199, data: 86.511078, healthStatus: 0 },
        { sensorType: 0, sampleType: 1, timestamp: 13208, data: 65.000000, healthStatus: 1 },
        { sensorType: 0, sampleType: 2, timestamp: 13214, data: 34.625000, healthStatus: 0 }
    ];
    
    sensorData = sampleSensorData;
    displayVisualization();
    updateDataSummary();
}

