#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
    char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value)
{
    //toma el hash de la key para tener la posicion
    long position = hash(key, map->capacity);

    int i = 0;
    //mientras la casilla esté ocupada
    while(map->buckets[position] != NULL && map->buckets[position]->key != NULL && i < map->capacity)
    {
        if(is_equal(key, map->buckets[position]->key) == 0)
        {
            position = (position+1) % map->capacity;
        }
        i++;
    }
    
    //inserta el nuevo dato
    map->buckets[position] = createPair(key, value);

    //actualiza current a la nueva posicion
    map->current = position;
    map->size++;
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    //map->buckets = (Pair **) realloc (map->buckets, map->capacity * sizeof(Pair));
    HashMap * auxMap = createMap(map->capacity);
    auxMap->buckets = map->buckets;

    map->capacity *= 2;
    map->buckets = (Pair **) calloc (map->capacity, sizeof(Pair));
    map->size = 0;

    auxMap->buckets = firstMap(auxMap);

    long position = auxMap->current;

    while(auxMap->buckets != NULL)
    {
        insertMap(map, auxMap->buckets[position]->key, auxMap->buckets[position]->value);
        nextMap(auxMap);
    }
}


HashMap * createMap(long capacity)
{
  HashMap * newMap = (HashMap *) malloc (sizeof(HashMap));
  newMap->buckets = (Pair **) calloc (capacity, sizeof(Pair));
  newMap->capacity = capacity;
  newMap->size = 0;
  newMap->current = -1;
  return newMap;
}

void eraseMap(HashMap * map,  char * key)
{
    long position = hash(key, map->capacity);
    if(map->buckets[position] == NULL) return;

    if(is_equal(key, map->buckets[position]->key) == 1)
    {
        map->buckets[position]->key = NULL;
        map->size--;
    }
    else
    {
        while(is_equal(key, map->buckets[position]->key) == 0)
        {
            position = (position+1) % map->capacity;
            if(map->buckets[position] == NULL) return;

            if(is_equal(key, map->buckets[position]->key) == 1)
            {
                map->buckets[position]->key = NULL;
                map->size--;
                return;
            }
        }
    }
}   

Pair * searchMap(HashMap * map,  char * key)
{
    //sacar posicion
    long position = hash(key, map->capacity);

    if(map->buckets[position] == NULL) return NULL;
    if(map->buckets[position]->key == NULL) return NULL;
    
    //si no se encuentra, resolucion de colisiones
    while(is_equal(key, map->buckets[position]->key) == 0)
    {
        position = (position+1) % map->capacity;
        if(map->buckets[position] == NULL) return NULL;
    }

    map->current = position;
    return map->buckets[position];
}

Pair * firstMap(HashMap * map)
{
    if(map->size < 1) return NULL;

    for(int i = 0 ; i < map->capacity ; i++)
    {
        if(map->buckets[i] != NULL && map->buckets[i]->key != NULL)
        {
            map->current = i;
            return map->buckets[i];
        }
    }
    
    return NULL;
}

Pair * nextMap(HashMap * map)
{
    for(int i = map->current+1 ; i < map->capacity ; i++)
    {
        if(map->buckets[i] != NULL && map->buckets[i]->key != NULL)
        {
            map->current = i;
            return map->buckets[i];
        }
    }
    
    return NULL;
}
