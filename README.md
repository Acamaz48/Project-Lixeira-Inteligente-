## Lixeira Inteligente - Controle via Sensor HC-SR04 e MQTT ##
Objetivo
Este projeto tem como finalidade o monitoramento do nível de lixo em uma lixeira utilizando um sensor ultrassônico HC-SR04, um sistema de sinalização com LEDs e um buzzer, e a comunicação via MQTT para envio de alertas.

### Componentes Utilizados :
- HC-SR04 → Mede a distância do lixo ao topo da lixeira.
- MQTT → Comunicação de status da lixeira via rede.
- LED Verde → Indica que a lixeira está abaixo do limite.
- LED Vermelho → Indica que a lixeira está com 80% da capacidade cheia.
- Buzzer → Emite sinal sonoro quando a lixeira atinge o nível crítico.
  
### Funcionamento
### 1. Medição da Distância 

- O HC-SR04 mede a distância entre o topo da lixeira e o lixo.
- Se a distância for maior que 15 cm, a lixeira é considerada vazia e os LEDs e o buzzer são desligados.

### 2. Monitoramento do Nível do Lixo
- Se a distância for menor ou igual a 80% da capacidade cheia, acende o LED Vermelho e aciona o buzzer.
- Se a distância for maior que o limite de 80%, mas menor que 15 cm, a lixeira ainda não está cheia, então mantém o LED Verde aceso.
  
### 3. Publicação via MQTT
- A distância medida é enviada para um tópico MQTT, permitindo o monitoramento remoto do nível de lixo.
  
### Conclusão
- A Lixeira Inteligente oferece uma solução eficiente para o gerenciamento de resíduos, possibilitando alertas automatizados e controle remoto via MQTT, promovendo maior eficiência na coleta de lixo.
