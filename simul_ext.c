#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cabeceras.h"

void PrintPrompt();
void ProcessCommand(char *command, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_BYTE_MAPS *ext_bytemaps,
                    EXT_BLQ_INODOS *ext_blq_inodos, EXT_ENTRADA_DIR *directorio, EXT_DATOS *memdatos, FILE *fich);


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

    EXT_SIMPLE_SUPERBLOCK ext_superblock;
    EXT_BYTE_MAPS ext_bytemaps;
    EXT_BLQ_INODOS ext_blq_inodos;
    EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
    EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
    FILE *fich;

    fich = fopen("particion.bin", "r+b");
    if (fich == NULL) {
        printf("Error al abrir el archivo particion.bin\n");
        return 1;
    }

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
        }else if (strcmp(comando, "ProcessCommand") == 0) {
            char command[100];
            for (;;) {
                PrintPrompt();  // Muestra el prompt al usuario
                fgets(command, sizeof(command), stdin);
                command[strcspn(command, "\n")] = '\0';  // Elimina el salto de línea del final

                // Procesa el comando ingresado
                ProcessCommand(command, &ext_superblock, &ext_bytemaps, &ext_blq_inodos, directorio, memdatos, fich);
            }
        }else if (strcmp(comando, "salir") == 0) {
            fclose(fich);
            return 0;
        } else {
            printf("Comando erroneo\n");
        }
    }

    return 0;
}
