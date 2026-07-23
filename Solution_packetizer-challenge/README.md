## Aplicação

O design e arcquitetura da solução pensada segue o fluxo abaixo

![Alt text for screen readers](img/flow.png)



## Arquivos

- No diretório `docs` estão todos os documentos explicando a aplicação e decisões.

- No diretório `core/inc` e `core/src` estão todos os arquivos necessário para construir a solução para o desafio.

- No diretório `app` estão os arquivos usados para implementar a aplicação de transmissão e recepção.

- No diretório `bin` (que aparece logo em seguida que a aplicação é compilada), estão os executáveis usados para a aplicação.



# Executando

Abra um terminal no diretório `core` e execute o comando `make`.
Logo após, execute nessa ordem:
- `./bin/receiver`
- `./bin/transmitter`

E a aplicação mostrará o envio e recebimento de mensagens