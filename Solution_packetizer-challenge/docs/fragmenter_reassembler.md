# Fragmentação e Reconstituição


Um dos requisitos do projeto é oferecer suporte a mensagens de qualquer tamanho, incluindo arquivos muito maiores do que um único pacote.

Como cada pacote possui um tamanho limitado, mensagens grandes não podem ser transmitidas em um único pacote, então precisam ser divididas em partes menores antes da transmissão e reconstruídas.

<br>

### Fragmentação

Para permitir que o receptor reconstrua a mensagem original, cada fragmento tem informações suficientes para identificar sua posição.

A implementação usada inclui:

* Número de sequência
* Índice do fragmento
* Número total de fragmentos

Com essas informações, o receptor sabe:

* quantos fragmentos são esperados;
* qual fragmento acabou de chegar;
* quando a mensagem completa foi recebida.

<br>

### Reconstituição

A reconstituição é o processo inverso.

O receptor coleta todos os fragmentos da mensagem enviada reconstrói os dados originais.

<br>

### Transferência de Arquivos

Da perspectiva do componente de fragmentação, um arquivo é simplesmente uma sequência de bytes. O mecanismo de fragmentação é igual tanto para mensagens de texto quanto para arquivos.
