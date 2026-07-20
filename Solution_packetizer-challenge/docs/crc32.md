# CRC32 - Verificação de integridade de pacotes

<br>

### Resumo

Um dos requisitos desse projeto é que dados corrompidos nunca sejam entregues à aplicação. Para isso, o protocolo utiliza o CRC32 (Cyclic Redundancy Check) de 32 bits.


<br>

#### Por que o CRC32 é necessário?

Sem nenhuma verificação de integridade, o receptor não teria como saber que os dados foram corrompidos e entregaria a mensagem incorreta.

Com o CRC32, o receptor consegue detectar que o pacote foi alterado e faz o descarte.


<br>

### Por que o CRC32?

O CRC32 foi escolhido porque é:

* Confiável
* Baixo custo computacional.
* Apenas 4 bytes adicionais por pacote.
* Excelente detecção de erros de transmissão.
* Muito utilizado em sistemas embarcados.
* Independente de plataforma.


<br>

### CRC32 no projeto

#### Remetente

Antes de transmitir um pacote:

1. O campo CRC é definido temporariamente como zero.
2. O pacote é serializado em um buffer de bytes.
3. O valor CRC32 é calculado sobre os bytes serializados.
4. O CRC calculado é gravado no pacote.
5. O pacote é transmitido.



#### Receptor

Ao receber um pacote:

1. O valor CRC recebido é salvo.
2. O pacote é reconstruído no mesmo formato utilizado pelo remetente.
3. O CRC32 é calculado novamente.
4. O valor calculado é comparado com o valor recebido.