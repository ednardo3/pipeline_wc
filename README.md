# Programação Concorrente e Sistemas Operacionais - Trabalho-01

**Profª Raquel Coelho Gomes Pinto**

# pipeline_wc

**Trabalho de Sistemas Operacionais**  
**Aluno:** Ednardo Miranda  
**Curso:** PGSC SE/9  
**E-mail:** ednardo.miranda@ime.eb.br

## Descrição

Este programa implementa um pipeline entre dois processos que simula o comportamento do comando shell:
```bash
cat <arquivo> | wc -l
```

O programa cria dois processos filhos conectados por um pipe:
- **Processo Produtor**: Executa `cat <arquivo>` e envia o conteúdo para o pipe
- **Processo Consumidor**: Executa `wc -l` e recebe dados do pipe para contar as linhas

## Compilação

Para compilar o programa, use o comando:
```bash
gcc -o pipeline_wc pipeline_wc.c
```

## Execução

### Sintaxe
```bash
./pipeline_wc <arquivo>
```

### Opções
```bash
./pipeline_wc -h          # Exibe ajuda
./pipeline_wc --help      # Exibe ajuda
```

### Exemplos de Uso
```bash
# Contar linhas de um arquivo texto
./pipeline_wc dados.txt

# Contar linhas do arquivo de usuários do sistema
./pipeline_wc /etc/passwd

# Contar linhas de um arquivo criado no momento
echo -e "linha1\nlinha2\nlinha3" > teste.txt && ./pipeline_wc teste.txt
```

## Exemplo de Saída

### Execução Normal
```bash
$ ./pipeline_wc /etc/passwd
[PRODUTOR] [PID 12345] (cat): Terminou com status 0
[CONSUMIDOR] [PID 12346] (wc -l) Terminou com status 0
     42
```

### Arquivo Inexistente
```bash
$ ./pipeline_wc arquivo_inexistente.txt
cat: arquivo_inexistente.txt: No such file or directory
[PRODUTOR] [PID 12347] (cat): Terminou com status 1
[CONSUMIDOR] [PID 12348] (wc -l) Terminou com status 0
       0
```

### Ajuda
```bash
$ ./pipeline_wc -h
pipeline_wc - Trabalho de Sistemas Operacionais
Autor: Ednardo Miranda - Sistemas de Computação SE/9
E-mail: ednardo.miranda@ime.eb.br
USO: ./pipeline_wc <arquivo>
Implementacao do comando: cat <arquivo> | wc -l
Exemplo: ./pipeline_wc /etc/passwd
Syscalls: fork(), pipe(), dup2(), execlp(), wait()
```

## System Calls Utilizadas

O programa utiliza as seguintes chamadas de sistema Unix/Linux:

| Syscall | Função |
|---------|--------|
| `fork()` | Cria processos filhos idênticos ao pai |
| `pipe()` | Cria um canal de comunicação unidirecional entre processos |
| `dup2()` | Duplica descritores de arquivo (redirecionamento de E/S) |
| `execlp()` | Substitui a imagem do processo atual por um novo programa |
| `wait()` | Faz o processo pai aguardar a finalização dos processos filhos |
| `access()` | Verifica permissões de acesso a arquivos |

### Outras Funções Importantes
- `WIFEXITED()` - Verifica se o processo terminou normalmente
- `WEXITSTATUS()` - Obtém o código de saída do processo
- `WIFSIGNALED()` - Verifica se o processo foi terminado por sinal
- `WTERMSIG()` - Obtém o número do sinal que terminou o processo

## Códigos de Saída

| Código | Significado |
|--------|-------------|
| 0 | Sucesso (mesmo código de saída do `wc`) |
| 1 | Erro nos argumentos ou erro geral |
| 2 | Erro na criação de processos (`fork`) |
| 3 | Arquivo inacessível |
| 6 | Falha na execução do `cat` |
| 7 | Falha na execução do `wc` |

## Arquitetura do Programa

```
[PROCESSO PAI]
    │
    ├─ cria pipe(fd[2])
    │
    ├─ fork() → [FILHO 1 - PRODUTOR]
    │              │
    │              ├─ close(fd[0])
    │              ├─ dup2(fd[1], STDOUT)
    │              └─ execlp("cat", arquivo)
    │
    ├─ fork() → [FILHO 2 - CONSUMIDOR]  
    │              │
    │              ├─ close(fd[1])
    │              ├─ dup2(fd[0], STDIN)
    │              └─ execlp("wc", "-l")
    │
    ├─ close(fd[0], fd[1])
    └─ wait() × 2 (aguarda ambos os filhos)
```

## Observações Técnicas

1. **Sincronização**: O programa aguarda ambos os processos filhos terminarem antes de finalizar
2. **Gerenciamento de Recursos**: Todos os descritores de arquivo são fechados adequadamente
3. **Tratamento de Erros**: Mensagens claras para diferentes tipos de erro
4. **Portabilidade**: Usa `execlp()` para localizar comandos automaticamente no PATH

## Requisitos do Sistema

- Sistema operacional Unix/Linux
- Compilador GCC
- Comandos `cat` e `wc` disponíveis no sistema
