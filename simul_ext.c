//Definicion de las librerias que vamos a usar
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cabeceras.h"
//----------------------------------------------


//Declaracion de las constantes que vamos a usar
#define LONGITUD_COMANDO 100
//----------------------------------------------


//Declaracion de los metodos que vamos a utilizar a lo largo del codigo.
void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre, FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);
int BuscarBloqueLibre(EXT_BYTE_MAPS *ext_bytemaps);
void PrintMenu();
//----------------------------------------------


//Funcion principal main, con ella inicializamos el programa para permitir al usuario visualizar el directorio y reazliar los comandos pertinentes.
int main()
{
    //Declaracion de las variables locales que vamos a utilizar para los comandos, para dividir dicho comando en la orden y los argumentos en caso necesario (remove, imprimir, copy)
    char comando[LONGITUD_COMANDO];
    char orden[LONGITUD_COMANDO];
    char argumento1[LONGITUD_COMANDO];
    char argumento2[LONGITUD_COMANDO];

    //Declaracion de las variables a utilizar junto a cabeceras.h
    int i, j;
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

    // Lectura del fichero completo de una sola vez, almacenandolo en una variable para su posteiror uso.
    fent = fopen("particion.bin", "r+b");
    fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);

    //Dividimos el fichero en distintas variables para poder operar con ellas y con el contenido.
    memcpy(&ext_superblock, (EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
    memcpy(&directorio, (EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
    memcpy(&ext_bytemaps, (EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
    memcpy(&ext_blq_inodos, (EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
    memcpy(&memdatos, (EXT_DATOS *)&datosfich[4], MAX_BLOQUES_DATOS * SIZE_BLOQUE);

    //Mostramos al principio un menu de inicio con los comandos principales del programa
    PrintMenu();

    // Buce de tratamiento de comandos
    for (;;)
    {
        //En este bucle se le pide la usuario el comando que quiera utilizar y se mantiene en el hasta introducir un comando correcto
        //En caso de ser erroneo se le indicara y pedira de nuevo.
        do
        {
            printf(">> ");
            fflush(stdin);
            fgets(comando, LONGITUD_COMANDO, stdin);
        } while (ComprobarComando(comando, orden, argumento1, argumento2) != 0);

        //Utilizacion de los metodos correspondientes al comando utilizado por el usuario
        if (strcmp(orden, "help") == 0)
        {
            PrintMenu();
            continue;
        }
        else if (strcmp(orden, "info") == 0)
        {
            LeeSuperBloque(&ext_superblock);
            continue;
        }
        else if (strcmp(orden, "bytemaps") == 0)
        {
            Printbytemaps(&ext_bytemaps);
            continue;
        }
        else if (strcmp(orden, "dir") == 0)
        {
            Directorio(&directorio, &ext_blq_inodos);
            continue;
        }
        else if (strcmp(orden, "rename") == 0)
        {
            Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);
            continue;
        }
        else if (strcmp(orden, "remove") == 0)
        {
            Borrar(&directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, argumento1, fent);
            continue;
        }
        else if (strcmp(orden, "copy") == 0)
        {
            Copiar(&directorio, &ext_blq_inodos, &ext_bytemaps, &ext_superblock, memdatos, argumento1, argumento2, fent);
            grabardatos = 1;
            continue;
        }
        else if (strcmp(orden, "imprimir") == 0)
        {
            Imprimir(directorio, &ext_blq_inodos, datosfich, argumento1);
            continue;
        }

        // Escritura de metadatos en comandos rename, remove, copy
        Grabarinodosydirectorio(&directorio, &ext_blq_inodos, fent);
        GrabarByteMaps(&ext_bytemaps, fent);
        GrabarSuperBloque(&ext_superblock, fent);
        if (grabardatos)
            GrabarDatos(&memdatos, fent);
        grabardatos = 0;
        // Si el comando es salir se habrán escrito todos los metadatos
        // faltan los datos y cerrar
        if (strcmp(orden, "salir") == 0)
        {
            GrabarDatos(&memdatos, fent);
            fclose(fent);
            return 0;
        }
    }
}

//Metodo utilizado para mostrar un pequeño menu con los comandos posibles y sus funciones.
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

//Metodo utilizado para imprirmir el mapa de bytes del "sistema de archivos" de nuestro programa
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

//Con este metodo comprobamos que el comando utilizado sea uno correcto y que este bien escrito
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2) {

    //Dividimos el comando en las distintas partes pertinentes.
	strcpy(orden, "");
	strcpy(argumento1, "");
	strcpy(argumento2, "");	
	
    //Comprobamos si el comando contiene mas variables para imprimir, remove o copy
	char aux[strlen(strcomando)-1];
	int contador = 0;
	strncpy(aux, strcomando, strlen(strcomando)-1);
	aux[strlen(strcomando)-1] = '\0';
	
	char *token=strtok(aux, " ");		
	
    //Asignamos las variables del comando a las variables que usaremos para comprobar que el comando es correcto
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
	
    //Comprobamos si los el comando utilizado existe, y en caso afirmativo, si contiene los datos necesarios para su uso
	if(strcmp(orden,"help")==0 || strcmp(orden,"info")==0 || strcmp(orden,"dir")==0 || strcmp(orden,"bytemaps")==0) {
		return 0;
	} else if (strcmp(orden, "rename") == 0) {
        //En los metodos que usan argumentos, comprobamos que tiene los argumentos correctos y bien escritos
		if ((strlen(argumento1) > 0) && (strlen(argumento2) > 0)) {
			return 0;
		} else {
			printf("ERROR. La orden debe ser \"rename nombre_archivo nuevo_nombre\".\n");
			return -1;
		}
	} else if(strcmp(orden,"imprimir")==0){
        //En los metodos que usan argumentos, comprobamos que tiene los argumentos correctos y bien escritos
		if(strlen(argumento1)>0){
			if(strlen(argumento2)>0){
				printf("ERROR. La orden debe ser \"imprimir nombre_archivo\"\n");
				return -1;
			}
			return 0;
		}else{
			printf("ERROR. La orden debe ser \"imprimir nombre_archivo\"\n");
			return -1;
		}
	} else if(strcmp(orden,"remove")==0){
        //En los metodos que usan argumentos, comprobamos que tiene los argumentos correctos y bien escritos
		if(strlen(argumento1)>0){
			if(strlen(argumento2)>0){
				printf("ERROR. La orden debe ser \"remove nombre_archivo\"\n");
				return -1;
			}
			return 0;
		}else{
				printf("ERROR. La orden debe ser \"remove nombre_archivo\"\n");
			return -1;
		}
	} else if(strcmp(orden,"copy")==0){
        //En los metodos que usan argumentos, comprobamos que tiene los argumentos correctos y bien escritos
		if(strlen(argumento1)>0 && strlen(argumento2)>0){
			return 0;
		} else {
			printf("ERROR. La orden debe ser \"copy nombre_archivo nuevo_nombre_archivo\"\n");
			return -1;
		}
	} else if(strcmp(orden,"salir")==0){
		return 0;
	} else {
		printf("ERROR: Comando incorrecto. Usa help para comprobar los comandos posibles.\n");
		return -1;	
	}
}

//Mostramos la informacion del superbloque utilizando cabeceras.h y los datos dividos en la funcion main.
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup) {
   printf("Bloque %u Bytes\n", psup->s_block_size);
   printf("inodos particion: %u\n", psup->s_inodes_count);
   printf("indoso libres: %u\n", psup->s_free_inodes_count);
   printf("Bloques particion: %u\n", psup->s_blocks_count);
   printf("Bloques libres: %u\n", psup->s_free_blocks_count);
   printf("Primer bloque de datos: %u\n", psup->s_first_data_block);
   printf("\n");
}

//Muestra los archivos que contiene el directorio al usuario, con informacion de cada archivo.
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

//Metodo para renombrar el archivo dado por el usuario por otro nuevo manteniendo sus caracteristicas
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

//Permite Imprimir el contenido del archivo dado por el usuario contenido en el directorio por pantalla.
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

//Metodo utilizado para borrar el fichero del directorio dado por el usuario.
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, char *nombre,  FILE *fich) {
    int i, j;
    int pos_fichero = -1;

    // Buscar el fichero en el directorio y guardar su posición
    for (i = 1; i < MAX_FICHEROS; i++) {
        if ((directorio + i)->dir_inodo != NULL_INODO) {
            if (strcmp(nombre, (directorio + i)->dir_nfich) == 0) {
                pos_fichero = i;
                break;
            }
        }
    }

    // Verificar si se encontró el archivo
    if (pos_fichero == -1) {
        printf("ERROR. El archivo %s no fue encontrado.\n", nombre);
        return -1;
    }

    // Operaciones de borrado
    for (int j = 0; inodos->blq_inodos[(directorio + pos_fichero)->dir_inodo].i_nbloque[j] != NULL_BLOQUE; j++) {
        int bloque_actual = inodos->blq_inodos[(directorio + pos_fichero)->dir_inodo].i_nbloque[j];
        ext_bytemaps->bmap_bloques[bloque_actual] = 0; // Borrar bloque del bytemap de bloques
    }

    ext_bytemaps->bmap_inodos[(directorio + pos_fichero)->dir_inodo] = 0; // Borrar bytemap de inodos

    inodos->blq_inodos[(directorio + pos_fichero)->dir_inodo].size_fichero = 0; // Borrar el tamaño del fichero

    ext_superblock->s_free_blocks_count += j; // Liberar bloques que se han borrado
    ext_superblock->s_free_inodes_count++; // Liberar el inodo que se ha borrado

    // Borrar la entrada del directorio
    for (int i = pos_fichero; i < MAX_FICHEROS - 1; i++) {
        directorio[i] = directorio[i + 1];
    }

}

//Metodo utilizado para copiar un archivo ya existente en uno nuevo.
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino, FILE *fich) {
    int i, j;
    int pos_origen = -1;
    int pos_destino = -1;

    // Buscar el archivo de origen en el directorio y guardar su posición
    for (i = 1; i < MAX_FICHEROS; i++) {
        if ((directorio + i)->dir_inodo != NULL_INODO) {
            if (strcmp(nombreorigen, (directorio + i)->dir_nfich) == 0) {
                pos_origen = i;
                break;
            }
        }
    }

    // Verificar si se encontró el archivo de origen
    if (pos_origen == -1) {
        printf("ERROR. El archivo %s no fue encontrado.\n", nombreorigen);
        return -1;
    }

    // Buscar un inodo y entrada de directorio libre para el archivo de destino
    for (i = 1; i < MAX_FICHEROS; i++) {
        if ((directorio + i)->dir_inodo == NULL_INODO) {
            pos_destino = i;
            break;
        }
    }

    // Verificar si se encontró un espacio para el archivo de destino
    if (pos_destino == -1) {
        printf("ERROR. No hay espacio para el archivo de destino.\n");
        return -1;
    }

    // Copiar información del inodo y directorio
    (directorio + pos_destino)->dir_inodo = (directorio + pos_origen)->dir_inodo;
    strcpy((directorio + pos_destino)->dir_nfich, nombredestino);

    // Copiar bloques de datos
    for (j = 0; inodos->blq_inodos[(directorio + pos_origen)->dir_inodo].i_nbloque[j] != NULL_BLOQUE; j++) {
        int bloque_origen = inodos->blq_inodos[(directorio + pos_origen)->dir_inodo].i_nbloque[j];
        int bloque_destino = BuscarBloqueLibre(ext_bytemaps);
        if (bloque_destino == -1) {
            printf("ERROR. No hay bloques libres para copiar el archivo.\n");
            return -1;
        }

        // Reservar un nuevo bloque de datos para el archivo de destino
        ext_bytemaps->bmap_bloques[bloque_destino] = 1;

        // Copiar el contenido del bloque original al nuevo bloque
        memcpy(memdatos[bloque_destino].dato, memdatos[bloque_origen].dato, SIZE_BLOQUE);

        // Actualizar el bloque en el inodo del archivo de destino
        inodos->blq_inodos[(directorio + pos_destino)->dir_inodo].i_nbloque[j] = bloque_destino;

        // Actualizar contadores del superbloque
        ext_superblock->s_free_blocks_count--;
    }

    // Actualizar el tamaño del fichero en el inodo del archivo de destino
    inodos->blq_inodos[(directorio + pos_destino)->dir_inodo].size_fichero =
        inodos->blq_inodos[(directorio + pos_origen)->dir_inodo].size_fichero;

    // Actualizar contadores del superbloque
    ext_superblock->s_free_inodes_count--;

    return 0;
}
int BuscarBloqueLibre(EXT_BYTE_MAPS *ext_bytemaps) {
    for (int i = 0; i < MAX_BLOQUES_PARTICION; i++) {
        if (ext_bytemaps->bmap_bloques[i] == 0) {
            ext_bytemaps->bmap_bloques[i] = 1; // Marcar bloque como utilizado
            return i;
        }
    }
    return -1; // No se encontraron bloques libres
}

//Estos cuatro métodos se utilizan para escribir información específica en el archivo de partición. Cada uno de ellos se encarga de grabar un tipo específico de datos en una posición determinada de la partición.
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich) {
    fseek(fich, SIZE_BLOQUE*2, SEEK_SET);	
	fwrite((EXT_DATOS *)inodos, SIZE_BLOQUE, 1, fich);		
	fseek(fich, SIZE_BLOQUE*3, SEEK_SET);
	fwrite((EXT_DATOS *)directorio, SIZE_BLOQUE, 1, fich);
}
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich) {
    fseek(fich, SIZE_BLOQUE*1, SEEK_SET);		
	fwrite((EXT_DATOS *)ext_bytemaps, SIZE_BLOQUE, 1, fich);
}
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich) {
    fseek(fich, SIZE_BLOQUE*0, SEEK_SET );
	fwrite((EXT_DATOS *)ext_superblock, SIZE_BLOQUE, 1,fich);
}
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich) {
    fseek(fich, SIZE_BLOQUE*4, SEEK_SET);		
	fwrite((EXT_DATOS *)memdatos, MAX_BLOQUES_DATOS*SIZE_BLOQUE, 1, fich);	
}
