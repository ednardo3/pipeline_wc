#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // ==========================================
    // Interface linha de comando
    // ==========================================
    
    // Verifica ajuda
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        printf("pipeline_wc - Trabalho de Sistemas Operacionais\n");
        printf("Autor: Ednardo Miranda - Sistemas de Computação SE/9\n");
        printf("E-mail: ednardo.miranda@ime.eb.br\n");
        printf("USO: %s <arquivo>\n", argv[0]);
        printf("Implementacao do comando: cat <arquivo> | wc -l\n");
        printf("Exemplo: %s /etc/passwd\n", argv[0]);
        printf("Syscalls: fork(), pipe(), dup2(), execlp(), wait()\n\n");
        return 0;
    }
    
    // Verifica argumentos
    if (argc != 2) {
        fprintf(stderr, "Erro: Use %s <arquivo> ou %s -h para ajuda\n", argv[0], argv[0]);
        return 1;
    }
    
    char *arquivo = argv[1];
    
    // Verifica se arquivo é acessível
    if (access(arquivo, R_OK) != 0) {
        perror("Erro ao acessar arquivo");
        return 3;
    }
    
    // ==========================================
    // PIPELINE 
    // ==========================================
    
    int fd[2];
    if (pipe(fd) == -1) {
        perror("Erro ao criar pipe");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        return 2;
    }

    if (pid1 == 0) {
        // Filho 1: cat
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        execlp("cat", "cat", arquivo, (char *)NULL);
        perror("execlp cat");
        exit(6);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        return 2;
    }

    if (pid2 == 0) {
        // Filho 2: wc -l
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        execlp("wc", "wc", "-l", (char *)NULL);
        perror("execlp wc");
        exit(7);
    }

    // Pai fecha pipe e espera filhos
    close(fd[0]);
    close(fd[1]);

    int status, wc_exit_code = 0;
    pid_t wpid;

    //espera os dois filhos
    for (int i = 0; i < 2; i++) {
        wpid = wait(&status);
        if (wpid == -1) {
            perror("wait");
            return 1;
        }
        // Verifica o modo de término do processo filho
        if (WIFEXITED(status)) {
            int return_code = WEXITSTATUS(status);

            if (wpid == pid1) {
                 // Primeiro processo: produtor (cat)
                printf("[PRODUTOR] [PID %d] (cat): Terminou com status %d\n", wpid, return_code);
            } else if (wpid == pid2) {
                // Segundo processo: consumidor (wc)
                printf("[CONSUMIDOR] [PID %d] (wc -l) Terminou com status %d\n", wpid, return_code);
                wc_exit_code = return_code;
            }
        } else if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);

            if (wpid == pid1) {
                printf("PRODUTOR [PID %d]: Interrompido por sinal %d\n", wpid, signal);
                printf("                    Causa: Terminação externa do processo cat\n");
            } else if (wpid == pid2) {
                printf("CONSUMIDOR [PID %d]: Interrompido por sinal %d\n", wpid, signal);
                printf("                     Causa: Terminação externa do processo wc\n");
                wc_exit_code = 1;
            }
        }
    }

    return wc_exit_code;
}