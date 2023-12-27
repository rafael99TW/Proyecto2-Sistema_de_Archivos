#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100


void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int main()
{
	char *comando[LONGITUD_COMANDO];
	char *orden[LONGITUD_COMANDO];
	char *argumento1[LONGITUD_COMANDO];
	char *argumento2[LONGITUD_COMANDO];
	
	int i,j;
	unsigned long int m;
    EXT_SIMPLE_SUPERBLOCK ext_superblock;
    EXT_BYTE_MAPS ext_bytemaps;
    EXT_BLQ_INODOS ext_blq_inodos;
    EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
    EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
    EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
    int entradadir;
    int grabardatos;
    FILE *fent;
    
    // Lectura del fichero completo de una sola vez
    
    
    fent = fopen("particion.bin","r+b");
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
    
    
    memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
    memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
    memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
    memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
    memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
    
	PrintMenu();

     // Buce de tratamiento de comandos
      for (;;){
         do {
            printf (">> ");
            fflush(stdin);
            fgets(comando, LONGITUD_COMANDO, stdin);
         } while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);

        if (strcmp(orden,"help") == 0) {
            PrintMenu();
            continue;
        }else if (strcmp(orden,"info") == 0) {
            LeeSuperBloque(&ext_superblock);
            continue;
        }else if (strcmp(orden,"bytemaps") == 0) {
            Printbytemaps(&ext_bytemaps);
            continue;
        }else if (strcmp(orden,"dir") == 0) {
            Directorio(&directorio,&ext_blq_inodos);
            continue;
        }else if (strcmp(orden,"rename") == 0) {
			Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);         
			continue;
        } else if (strcmp(orden, "remove") == 0) {
            Borrar(&directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, fent);
            continue;
        }else if (strcmp(orden,"copy") == 0) {
            Copiar(&directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2, fent);
            grabardatos = 1; // Necesario para grabar cambios en datos al final del bucle
            continue;
        }
        else if (strcmp(orden,"imprimir") == 0) {
            Imprimir(directorio, &ext_blq_inodos, datosfich, argumento1);
            continue;
        }
         
         // Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(&memdatos,fent);
         grabardatos = 0;
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         if (strcmp(orden,"salir")==0){
            GrabarDatos(&memdatos,fent);
            fclose(fent);
            return 0;
         }
      }
}

void PrintMenu() {
    printf("===== Menú =====\n");
	printf("help      - Mostrar el menu de comandos\n");
    printf("info      - Mostrar información del superbloque\n");
    printf("bytemaps  - Mostrar información de los bytemaps\n");
    printf("dir       - Mostrar contenido del directorio\n");
    printf("rename    - Renombrar un archivo\n");
    printf("imprimir  - Imprimir contenido de un archivo\n");
    printf("remove    - Eliminar un archivo\n");
    printf("copy      - Copiar un archivo\n");
    printf("salir     - Salir del programa\n");
    printf("================\n\n");
}

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps) {
	printf("Inodos : ");
	for (int i = 0; i < MAX_INODOS; i++) {
		if (ext_bytemaps->bmap_inodos[i] == 1) {
      		printf("1 ");
    	} else {
     		printf("0 ");
    	}
  	}

	printf("\n");

	printf("Bloques [0-25] : ");
	for (int i = 0; i < 25; i++) {
		if (ext_bytemaps->bmap_bloques[i] == 1) {
			printf("1 ");
		} else {
			printf("0 ");
		}
	}

	printf("\n\n");
}

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {

	strcpy(orden, "");
	strcpy(argumento1, "");
	strcpy(argumento2, "");	
	
	char aux[strlen(strcomando)-1];
	int contador = 0;
	strncpy(aux, strcomando, strlen(strcomando)-1);
	aux[strlen(strcomando)-1] = '\0';
	
	char *token=strtok(aux, " ");		
		
	while(token!=NULL){		
		if(contador==0){
			strcpy(orden,token);			
		}else if(contador==1){
			strcpy(argumento1,token);			
		}else if(contador==2){
			strcpy(argumento2,token);
		}
		token=strtok(NULL, " ");
		contador++;
	}
	
	if(strcmp(orden,"help")==0 || strcmp(orden,"info")==0 || strcmp(orden,"dir")==0 || strcmp(orden,"bytemaps")==0){
		return 0;
	} else if (strcmp(orden, "rename") == 0) {
		if ((strlen(argumento1) > 0) && (strlen(argumento2) > 0)) {
			return 0;
		} else {
			printf("ERROR. Comprueba los argumentos del comando [renombrar archivo_anterior nuevo_nombre_archivo]\n");
			return -1;
		}
	} else if(strcmp(orden,"imprimir")==0){
		if(strlen(argumento1)>0){
			if(strlen(argumento2)>0){
				printf("ERROR. Comprueba los argumentos del comando [imprimir nombre_archivo]\n");
				return -1;
			}
			return 0;
		}else{
			printf("ERROR. Comprueba los argumentos del comando [imprimir nombre_archivo]\n");
			return -1;
		}
	} else if(strcmp(orden,"remove")==0){
		if(strlen(argumento1)>0){
			if(strlen(argumento2)>0){
				printf("Argumentos incorrectos\nComando correcto \"eliminar archivo.txt\"\n");
				return -1;
			}
			return 0;
		}else{
			printf("Argumentos incorrectos\nComando correcto \"eliminar archivo.txt\"\n");
			return -1;
		}
	} else if(strcmp(orden,"copy")==0){
		if(strlen(argumento1)>0 && strlen(argumento2)>0){
			return 0;
		} else {
			printf("ERROR. Comprueba los argumentos del comando [copiar fichero_origen fichero_destino]\n");
			return -1;
		}
	} else if(strcmp(orden,"salir")==0){
		return 0;
	} else {
		printf("ERROR: Comando incorrecto.\n");
		return -1;	
	}
}


void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup) {
   printf("Bloque %u Bytes\n", psup->s_block_size);
   printf("inodos particion: %u\n", psup->s_inodes_count);
   printf("indoso libres: %u\n", psup->s_free_inodes_count);
   printf("Bloques particion: %u\n", psup->s_blocks_count);
   printf("Bloques libres: %u\n", psup->s_free_blocks_count);
   printf("Primer bloque de datos: %u\n", psup->s_first_data_block);
   printf("\n");
}

int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre) {
    for (int i = 1; i < MAX_FICHEROS; i++) {
        if ((directorio + i)->dir_inodo != NULL_INODO) {
            if (strcmp(nombre, (directorio + i)->dir_nfich) == 0) {
                return (directorio + i)->dir_inodo;
            }
        }
    }
    return -1; // No se encontró el archivo
}


void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) {
    for (int i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo != NULL_INODO) {
            EXT_SIMPLE_INODE inodo = inodos->blq_inodos[directorio[i].dir_inodo];

            printf("%s\ttamaño: %u bytes\tinodo: %u\tbloques: ", directorio[i].dir_nfich, inodo.size_fichero, directorio[i].dir_inodo);

            for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++) {
                if (inodo.i_nbloque[j] != NULL_BLOQUE) {
                    printf("%u ", inodo.i_nbloque[j]);
                }
            }
            printf("\n");
        }
    }

	printf("\n");
}


int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo) {
   int i;

    for (i = 0; i < MAX_FICHEROS; i++) {
        if (directorio[i].dir_inodo != NULL_INODO) {
            if (strcmp(nombreantiguo, directorio[i].dir_nfich) == 0) {
                // Encontramos el archivo a renombrar
                strcpy(directorio[i].dir_nfich, nombrenuevo);
                return 0;  // Operación exitosa
            }
        }
    }

    // Si llegamos aquí, no se encontró el archivo
    printf("ERROR. El archivo %s no fue encontrado.\n", nombreantiguo);
    return -1;
}

int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {
	int i, j;
	for(i = 1; i < MAX_FICHEROS; i++){
		if((directorio+i)->dir_inodo != NULL_INODO){
			
			if(strcmp(nombre,(directorio+i)->dir_nfich) == 0){
				for(j = 0; inodos->blq_inodos[(directorio+i)->dir_inodo].i_nbloque[j] != NULL_INODO; j++){//
                  char aux[SIZE_BLOQUE];
                  memcpy(aux,memdatos[inodos->blq_inodos[(directorio+i)->dir_inodo].i_nbloque[j]].dato,SIZE_BLOQUE);
				  aux[SIZE_BLOQUE]='\0';
				  printf("%s", aux);
                }
				printf("\n");
			}	
		}
	}
	return 0;
}

int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich) {
    
    return 0;
}

int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich) {
    
    return 0;
}

void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich) {
    fseek(fich, SIZE_BLOQUE * 3, SEEK_SET);
    fwrite(directorio, SIZE_BLOQUE, 1, fich);
    fwrite(inodos, SIZE_BLOQUE, 1, fich);
}

void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich) {
    fseek(fich, SIZE_BLOQUE, SEEK_SET);
    fwrite(ext_bytemaps, SIZE_BLOQUE, 1, fich);
}

void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich) {

}

void GrabarDatos(EXT_DATOS *memdatos, FILE *fich) {

}
