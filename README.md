# RCOM

## Trabalho 1

Dentro da diretoria [trabalho1](https://github.com/matilde-silva-21/trabalho1_RCOM) escrever no terminal no computador recetor ```make clean && make && clear && ./bin/main /dev/ttyS0 rx penguin-received.gif``` e no terminal no computador emissor ```make clean && make && clear && ./bin/main /dev/ttyS0 tx penguin.gif```.

Este código rebenta com ficheiros grandes e não recupera bem de interferências agressivas. Talvez mudar o tamanho dos pacotes (e torná-lo dinãmico em vez de hard-coded) ajude nesta parte.

No ano de 2022/23 não havia (alegadamente) verificação de plágio, por isso usem este código ao vosso risco.

## Trabalho 2

Dentro da diretoria [trabalho2/code](https://github.com/matilde-silva-21/trabalho2_RCOM/tree/main/code) compilar o ficheiro **main.c** (`gcc -Wall -o main main.c`) e correr ```./main ftp://[<user>:<password>@]<host>/<url-path>```.