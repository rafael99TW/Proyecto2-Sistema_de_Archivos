#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

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

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);

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
void mostrarDir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {
    printf("Listado del directorio:\n");
    
    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo != NULL_INODO) {
            // Obtener información del inodo correspondiente
            EXT_SIMPLE_INODE inodo = inodos->blq_inodos[directorio[i].dir_inodo];

            printf("Nombre: %s\n", directorio[i].dir_nfich);
            printf("Tamaño: %u bytes\n", inodo.size_fichero);

            printf("Bloques que ocupa: ");
            for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
                if (inodo.i_nbloque[j] != NULL_BLOQUE) {
                    printf("%u ", inodo.i_nbloque[j]);
                }
            }
            printf("\n");

            printf("Inodo: %u\n", directorio[i].dir_inodo);

            printf("\n");
        }
    }
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

int BuscarPrimerInodoLibre(EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock) {
    for (int i = 0; i < ext_superblock->s_inodes_count; i++) {
        if (IsFreeInode(ext_bytemaps->bmap_inodos, i)) {
            return i;
        }
    }
    return -1;  // No hay inodos libres
}

// Función auxiliar para buscar el primer bloque libre
int BuscarPrimerBloqueLibre(EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock) {
    for (int i = 0; i < ext_superblock->s_blocks_count; i++) {
        if (IsFreeBlock(ext_bytemaps->bmap_bloques, i)) {
            return i;
        }
    }
    return -1;  // No hay bloques libres
}

void Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
            EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
            EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich) {

    int inodoorigen = BuscaFich(directorio, inodos, nombreorigen);
    if (inodoorigen == -1) {
        printf("El archivo de origen no existe\n");
        return;
    }

    int inododestino = BuscaFich(directorio, inodos, nombredestino);
    if (inododestino != -1) {
        printf("El archivo de destino ya existe\n");
        return;
    }

    inododestino = BuscarPrimerInodoLibre(ext_bytemaps, ext_superblock);
    if (inododestino == -1) {
        printf("No hay inodos libres disponibles\n");
        return;
    }

    MarkInode(ext_bytemaps->bmap_inodos, inododestino);

    EXT_SIMPLE_INODE inodoorigeninfo = inodos->blq_inodos[inodoorigen];

    AsignarInodoDirectorio(directorio, nombredestino, inododestino);

    inodos->blq_inodos[inododestino].size_fichero = inodoorigeninfo.size_fichero;

    for (int i = 0; i < MAX_NUMS_BLOQUE_INODO; i++) {
        if (inodoorigeninfo.i_nbloque[i] != NULL_BLOQUE) {
            int bloquedestino = BuscarPrimerBloqueLibre(ext_bytemaps, ext_superblock);
            if (bloquedestino != -1) {
                MarkBlock(ext_bytemaps->bmap_bloques, bloquedestino);
                inodos->blq_inodos[inododestino].i_nbloque[i] = bloquedestino;
                memcpy(memdatos[bloquedestino].dato, memdatos[inodoorigeninfo.i_nbloque[i]].dato, SIZE_BLOQUE);
            } else {
                printf("No hay bloques libres disponibles\n");
                return;
            }
        }
    }

    printf("Copia realizada con éxito\n");
}
int main() {
	 //char *orden[LONGITUD_COMANDO];
	 //char *argumento1[LONGITUD_COMANDO];
	 //char *argumento2[LONGITUD_COMANDO];
	 
	 int i,j;
	 unsigned long int m;
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
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);
    
    memcpy(&ext_superblock, (EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
    memcpy(&directorio, (EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
    memcpy(&ext_bytemaps, (EXT_BYTE_MAPS *)&datosfich[1], SIZE_BLOQUE);
    memcpy(&ext_blq_inodos, (EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
    memcpy(&memdatos, (EXT_DATOS *)&datosfich[4], MAX_BLOQUES_DATOS * SIZE_BLOQUE);
    

    fich = fopen("particion.bin", "r+b");
    if (fich == NULL) {
        printf("Error al abrir el archivo particion.bin\n");
        return 1;
    }


   
    PrintMenu();
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
            mostrarDir(directorio, &ext_blq_inodos);
        } else if (strcmp(comando, "rename") == 0) {
            mostrarRename();
        } else if (strcmp(comando, "imprimir") == 0) {
            mostrarImprimir();
        } else if (strcmp(comando, "remove") == 0) {
            mostrarRemove();
        } else if (strcmp(comando, "copy") == 0) {
            mostrarCopy();
        }else if(strcmp(comando, "copiar") == 0){
            
        }else if (strcmp(comando, "salir") == 0) {
            fclose(fich);
            return 0;
        } else {
            printf("Comando erroneo. Comandos utilizables; info, dir, bytemaps, rename, imprimir, remove, salir, copy,\n");
        }
    }

    return 0;
}