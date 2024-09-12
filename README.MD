# Trabalho 2 de Software Básico 

Trabalho 2 da disciplina de Software Básico, que consiste na implementação em C++ de um tradutor para intel assembly 32 nasm do código objeto de uma linguagem assembly inventada, compilada pelo programa realizado no primeiro trabalho da disciplina.

## Alunos

- José Barbosa Araújo (Matrícula: 202014305)
- Arthur Rattes Lacava (Matrícula: 200053523)

## Compilador e SO utilizados

O sistema operacional utilizado para desenvolver o projeto foi Linux, especificamente WSL Ubuntu, compilador g++
## Como compilar o arquivo e gerar o assembly

Para compilar e criar o arquivo .s (assembly) é necessário utilizar os seguintes comandos:

```
Para compilar o tradutor;
g++ -o tradutor1 tradutor1.cpp

Para gerar o arquivo .s utiliza-se um arquivo .obj como codigo objeto fonte que gera o arquivo .s
./tradutor1 seu_arquivo.obj ;
```

## Compilação e execução do programa assembly gerado

Para compilar;
nasm -f elf32 seu_arquivo.s -o seu_arquivo.o

Para Ligar;
ld -m elf_i386 -o seu_arquivo seu_arquivo.o

Para executar;
./seu_arquivo