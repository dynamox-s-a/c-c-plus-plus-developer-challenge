# Design

A solução Packetizer é responsável pelo envio de mensagens byte por byte.
Nela pode-se: 
- Detectar corropção de mensagens.
- Garantia de entrega com aviso de recebimento.
- Transferência menagens e arquivos.

<br>

## Formato do pacote

Os dados são enviados via pacotes, que, por simplicidade está definido como:
- S:     Início do pacote, que é representado pelo valor 0xAABB.
- T:     Tipo de mensagem, dados ou aviso de recebimento ou não recebimento.
- L:     Tamanho dos dados a serem enviados e recebidos.
- P:     Dados.
- SP:    Sequência de pacotes, número dado a cada pacote para verifica se há dados duplicados ou algum dado faltando.
- FI:    Ìndice de cada fragmento de mensagem em cada pacote.
- TF:    Tamanho total de fragmentos (partes da mensagem).
- CRC:   Teste de integridade para saber se há dados corrompidos.


<br>

## Envio das mensagens

#### Texto
Se a menssagem a ser enviada e recebida for maior do que o tamanho máximo do pacote, essa mensagem é dividida em vários pacotes e ao final do recebimento, a mensagem é reconstruída.


#### Arquivo
Um arquivo nada mais é que uma série de bytes, logo, terá o mesmo comportamento que descrito acima, mas antes o arquivo terá que ser lido, se envia byte por byte, e o repector, monta novamente esses bytes em ordem.