# Transporte TCP

Um dos requisitos do projeto é que o núcleo do packetizer seja independente do transporte. Por esse motivo, o código específico para TCP está isolado.

<br>

#### Responsabilidades

O módulo de Transporte TCP é responsável por:

- Criar sockets TCP.
- Conectar a um par remoto (modo cliente).
- Aguardar conexões de entrada (modo servidor).
- Enviar e receber fluxos de bytes brutos.
- Receber fluxos de bytes brutos.
- Fechar sockets e liberar recursos.

<br>

#### Modo Cliente

O transporte no modo cliente executa as seguintes etapas:

1. Cria um socket TCP.
2. Conecta-se ao servidor remoto.
3. Aguarda até que a conexão seja estabelecida com sucesso.
4. Envia e recebe bytes.

#### Modo Servidor

O transporte no modo servidor executa as seguintes etapas:

1. Cria um socket TCP.
2. Se associa a uma porta local (bind).
3. Aguarda conexões de entrada.
4. Aceita uma conexão de cliente.
5. Envia e recebe bytes