# Serialização e Desserialização

Usando a estrutura packet descrita em [protocol](./packet.md), ela não pode ser transmitida diretamente porque o payload. é um ponteiro, então se a estrutura fosse enviada diretamente,apenas o endereço seria enviados e não os dados reais daquele endereço.

O pacote deve ser convertido primeiro em uma sequência de bytes antes de ser transmitido, e para isso é feito o processo de serialização.

<br>

#### Serialização

A serialização converte um Packet em um buffer de bytes.
A camada de transporte envia apenas esse buffer de bytes.

<br>

#### Desserialização

Quando o buffer the bytes for recebido, o receptor deve reconstruir o Pacote original, feito no processo de desserialização.