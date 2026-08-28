#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t terminado = 0;

void sigint_handler(int sig) {
    (void)sig;
    terminado = 1;
}

int main(void) {
    signal(SIGINT, sigint_handler);

    while (!terminado) {
        printf("Trabajando...\n");
        sleep(1);
    }

    printf("Cerrando programa prolijamente...\n");
    return 0;
}