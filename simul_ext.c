#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cabeceras.h"

void PrintPrompt() {
    printf(">> ");
}

void PrintMenu() {
    printf("===== Menú =====\n");
    printf("info      - Mostrar información del superbloque\n");
    printf("bytemaps  - Mostrar información de los bytemaps\n");
    printf("dir       - Mostrar contenido del directorio\n");
    printf("rename    - Renombrar un archivo\n");
    printf("imprimir  - Imprimir contenido de un archivo\n");
    printf("remove    - Eliminar un archivo\n");
    printf("copy      - Copiar un archivo\n");
    printf("salir     - Salir del programa\n");
    printf("================\n");
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
    printf("Listado del directorio:\n");
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
    EXT_BYTE_MAPS ext_bytemaps;
    EXT_BLQ_INODOS ext_blq_inodos;
    EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
    EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
    EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
    int entradadir;
    int grabardatos;
    char comando[20];

    fich = fopen("particion.bin", "r+b");
    if (fich == NULL) {
        printf("Error al abrir el archivo particion.bin\n");
        return 1;
    }


   
    
    while (1) {
        PrintMenu();
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
            printf("Comando erroneo. Comandos utilizables; info, dir, bytemaps, rename, imprimir, remove, salir, copy,\n");
        }
    }

    return 0;
}
