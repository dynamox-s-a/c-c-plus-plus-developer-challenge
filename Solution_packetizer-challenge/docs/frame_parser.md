# Analisador de frames

#### Visão Geral

A camada de transporte utilizada (TCP) transmite um fluxo contínuo de bytes, ela não preserva os limites entre os pacotes.

O objetivo do Analisador de frames é reconstruir pacotes completos a partir desse fluxo de bytes antes que eles sejam desserializados e processados ​​pela aplicação.


<br>

#### Por que o analisador de frames é necessário?

Quando é transmitido mais de um pacote, o remetente pode considerar como dois pacotes independentes, mas o receptor pode receber os dados de maneiras diferentes, como:

1. Dois pacotes recebidos juntos
2. Pacote dividido em múltiplas leituras
3. Múltiplos pacotes parciais

Sem isso, o receptor não saberia onde um pacote começa e termina ou se foram recebidos bytes suficientes para reconstruir um pacote.