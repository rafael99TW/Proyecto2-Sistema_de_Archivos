#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cabeceras.h"

void PrintPrompt() {
    printf(">> ");
}

void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup, FILE *fich) {
    fseek(fich, 0, SEEK_SET);
    fread(psup, sizeof(EXT_SIMPLE_SUPERBLOCK), 1, fich);
}

void mostrarInfo(EXT_SIMPLE_SUPERBLOCK *psup) {
    printf("Información del Superbloque:\n");
    printf("Número de inodos: %u\n", psup->s_inodes_count);
    printf("Número de bloques: %u\n", psup->s_blocks_count);
    printf("Bloques libres: %u\n", psup->s_free_blocks_count);
    printf("Inodos libres: %u\n", psup->s_free_inodes_count);
    printf("Primer bloque de datos: %u\n", psup->s_first_data_block);
    printf("Tamaño del bloque en bytes: %u\n", psup->s_block_size);
}
void ProcessCommand(char *command, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_BYTE_MAPS *ext_bytemaps,
                    EXT_BLQ_INODOS *ext_blq_inodos, EXT_ENTRADA_DIR *directorio, EXT_DATOS *memdatos, FILE *fich);



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

    FILE *fich;
    EXT_SIMPLE_SUPERBLOCK ext_superblock;
    char comando[20];

    fich = fopen("particion.bin", "r+b");
    if (fich == NULL) {
        printf("Error al abrir el archivo particion.bin\n");
        return 1;
    }


   

    while (1) {
        PrintPrompt();
        fgets(comando, sizeof(comando), stdin);
        comando[strcspn(comando, "\n")] = '\0';  // Elimina el salto de línea del final
        scanf("%s", comando);

        if (strcmp(comando, "info") == 0) {
            LeeSuperBloque(&ext_superblock, fich);
            mostrarInfo(&ext_superblock);
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
        }else if (strcmp(comando, "salir") == 0) {
            fclose(fich);
            return 0;
        } else {
            printf("Comando erroneo\n");
        }
    }

    return 0;
}
