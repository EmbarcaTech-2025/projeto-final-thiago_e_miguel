#!/usr/bin/env python3
"""
Script para criar diagrama focado no fluxo de trilha do sistema Tracking-Trilha
Demonstra o uso prático durante uma trilha de monitoramento
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch

def create_trail_flow():
    """Cria diagrama focado no fluxo de trilha"""
    
    fig, ax = plt.subplots(figsize=(20, 10))
    ax.set_xlim(0, 18)
    ax.set_ylim(0, 10)
    ax.axis('off')
    
    # Título
    ax.text(9, 9.5, 'Sistema Tracking-Trilha - Fluxo de Utilização', 
            fontsize=22, fontweight='bold', ha='center')
    ax.text(9, 9, 'Monitoramento de Saúde e Atividade Física em Tempo Real', 
            fontsize=16, style='italic', ha='center', color='gray')
    
    # Fluxo principal - horizontal
    flow_steps = [
        (1, 7.5, "1. CONFIGURAR\nHARDWARE", "Conectar sensores\nPico W + Display + SD\nCompilar e fazer flash", '#FF6B6B'),
        (2, 7.5, "2. CONFIGURAR\nTHRESHOLDS", "Interface web\nEditar config.txt\nDefinir alertas", '#4ECDC4'),
        (3, 7.5, "3. INICIAR\nTRILHA", "Ligar sistema\nVerificar sensores\nIniciar coleta", '#45B7D1'),
        (4, 7.5, "4. COLETA\nCONTÍNUA", "SpO2, HR, GPS\nAceleração\nArmazenamento SD", '#96CEB4'),
        (5, 7.5, "5. ANÁLISE\nTEMPO REAL", "Classificação\nAlertas sonoros\nDisplay OLED", '#FFEAA7'),
        (6, 7.5, "6. FINALIZAR\nTRILHA", "Parar coleta\nSalvar dados\nDesligar", '#DDA0DD')
    ]
    
    # Desenhar caixas do fluxo principal
    for i, (step, y, title, desc, color) in enumerate(flow_steps):
        x = 0.5 + i * 2.5
        
        # Caixa principal
        box = FancyBboxPatch((x, y-0.6), 2, 1.2, 
                            boxstyle="round,pad=0.1", 
                            facecolor=color, alpha=0.3, 
                            edgecolor=color, linewidth=3)
        ax.add_patch(box)
        
        # Título
        ax.text(x+1, y+0.2, title, fontsize=11, fontweight='bold', ha='center')
        
        # Descrição
        ax.text(x+1, y-0.2, desc, fontsize=8, ha='center', va='center')
        
        # Seta para direita
        if i < len(flow_steps) - 1:
            ax.arrow(x+2.1, y, 0.2, 0, head_width=0.1, head_length=0.05, 
                    fc='black', ec='black', linewidth=2)
        else:
          ax.arrow(x+2.1, y, 1.1, 0, head_width=0.0, head_length=0.0, 
                      fc='black', ec='black', linewidth=2)
          ax.arrow(x+3.2, y, 0, -0.8, head_width=0.1, head_length=0.05, 
                      fc='black', ec='black', linewidth=2)
    
    # Análise pós-trilha - posicionada mais à direita
    analysis_box = FancyBboxPatch((15, 5), 2.5, 1.5, 
                                 boxstyle="round,pad=0.1", 
                                 facecolor='#FFB6C1', alpha=0.3, 
                                 edgecolor='#FF69B4', linewidth=3)
    ax.add_patch(analysis_box)
    ax.text(16.25, 5.8, "7. ANÁLISE\nPÓS-TRILHA", fontsize=11, fontweight='bold', ha='center')
    ax.text(16.25, 5.5, "Interface web\nGráficos\nRelatórios", fontsize=8, ha='center', va='center')
    
    # Loop de retorno
    #ax.arrow(16.25, 6.5, 0, -1, head_width=0.1, head_length=0.05, fc='red', ec='red', linewidth=2)
    ax.arrow(14.9, 5.75, -13.4, 0, head_width=0.0, head_length=0.0, fc='red', ec='red', linewidth=2)
    ax.arrow(1.5, 5.75, 0, 1, head_width=0.1, head_length=0.05, fc='red', ec='red', linewidth=2)
    
    ax.text(8.5, 5.5, 'Nova Trilha', fontsize=12, ha='center', style='italic', 
            color='red', fontweight='bold')
    
    # Seção de detalhes técnicos
    ax.text(1, 4.5, 'Sensores em Ação:', fontsize=14, fontweight='bold', color='#2E86AB')
    
    # Sensores
    sensors = [
        (1, 4, "MAX3010X", "SpO2, Frequência Cardíaca, Temperatura", '#FF6B6B'),
        (1, 3.2, "IMU6050", "Aceleração e Giroscópio", '#4ECDC4'),
        (1, 2.4, "GPS", "Localização e Velocidade", '#45B7D1'),
        (1, 1.6, "Display OLED", "Interface Visual", '#96CEB4'),
        (1, 0.8, "Buzzer", "Alertas Sonoros", '#FFEAA7')
    ]
    
    for x, y, name, desc, color in sensors:
        # Caixa do sensor
        sensor_box = FancyBboxPatch((x-0.4, y-0.3), 2.4, 0.5, 
                                   boxstyle="round,pad=0.05", 
                                   facecolor=color, alpha=0.2, 
                                   edgecolor=color, linewidth=1)
        ax.add_patch(sensor_box)
        
        ax.text(x+0.8, y+0.1, name, fontsize=10, fontweight='bold', ha='center')
        ax.text(x+0.8, y-0.1, desc, fontsize=8, ha='center', color='gray')
    
    # Funcionalidades da interface web
    ax.text(5, 4.5, 'Interface Web:', fontsize=14, fontweight='bold', color='#F18F01')
    
    web_features = [
        (5, 4, "Upload de Dados", "samples.csv do SD card"),
        (5, 3.5, "Visualização", "Gráficos Chart.js interativos"),
        (5, 3, "Configuração", "Edição de thresholds"),
        (5, 2.5, "Relatórios", "Exportação e análise"),
        (5, 2, "Dashboard", "Monitoramento em tempo real")
    ]
    
    for x, y, feature, desc in web_features:
        ax.text(x, y+0.15, f"• {feature}", fontsize=10, color='#F18F01')
        ax.text(x+1.3, y+0.15, desc, fontsize=8, color='gray')
    
    # Dados coletados
    ax.text(10, 4.5, 'Dados Coletados:', fontsize=14, fontweight='bold', color='#6A994E')
    
    data_types = [
        (10, 4, "Biométricos", "SpO2, HR, Temperatura"),
        (10, 3.5, "Movimento", "Aceleração X, Y, Z"),
        (10, 3, "Localização", "Latitude, Longitude, Altitude"),
        (10, 2.5, "Velocidade", "Velocidade em km/h"),
        (10, 2, "Timestamp", "Data e hora de cada amostra")
    ]
    
    for x, y, data_type, desc in data_types:
        ax.text(x, y+0.15, f"• {data_type}", fontsize=10, color='#6A994E')
        ax.text(x+1, y+0.15, desc, fontsize=8, color='gray')
    
    # Destaque para o loop
    ax.text(9, 1.2, 'CICLO CONTÍNUO DE MONITORAMENTO', fontsize=14, 
            ha='center', style='italic', color='red', fontweight='bold')
    
    # Nota importante
    ax.text(9, 0.5, 'Sistema projetado para uso contínuo durante trilhas e atividades físicas', 
            fontsize=10, ha='center', style='italic', color='gray')
    
    plt.tight_layout()
    return fig

if __name__ == "__main__":
    print("Criando diagrama de fluxo de trilha...")
    fig = create_trail_flow()
    fig.savefig('tracking_trilha_trail_flow.png', dpi=300, bbox_inches='tight')
    print("Diagrama salvo como: tracking_trilha_trail_flow.png")
    plt.show()
