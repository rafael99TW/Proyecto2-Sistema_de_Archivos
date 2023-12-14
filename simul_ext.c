#include <stdio.h>
#include <string.h>

void mostrarInfo() {
    printf("info usado\n");
}

void mostrarBytemaps() {
    printf("bytemaps usado\n");
}

void mostrarDir() {
    printf("dir usado\n");
}

void mostrarRename() {
    printf("rename usado\n");
}

void mostrarImprimir() {
    printf("imprimir usado\n");
}

void mostrarRemove() {
    printf("remove usado\n");
}

void mostrarCopy() {
    printf("copy usado\n");
}

int main() {
    char comando[20];

    while (1) {
        printf(">> ");
        scanf("%s", comando);

        if (strcmp(comando, "info") == 0) {
            mostrarInfo();
        } else if (strcmp(comando, "bytemaps") == 0) {
            mostrarBytemaps();
        } else if (strcmp(comando, "dir") == 0) {
            mostrarDir();
        } else if (strcmp(comando, "rename") == 0) {
            mostrarRename();
        } else if (strcmp(comando, "imprimir") == 0) {
            mostrarImprimir();
        } else if (strcmp(comando, "remove") == 0) {
            mostrarRemove();
        } else if (strcmp(comando, "copy") == 0) {
            mostrarCopy();
        } else if (strcmp(comando, "salir") == 0) {
            break;
        } else {
            printf("Comando erroneo\n");
        }
    }

    return 0;
}
