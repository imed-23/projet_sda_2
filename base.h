
#ifndef BASE
#define BASE

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <stdbool.h>
#include <string.h>


#define N_ASCII (0x80)

typedef bool Bool;
typedef unsigned int Nat;
typedef int Ent;
typedef float Reel;
typedef float Rat;
typedef unsigned char Car;
typedef Car * Chaine;

typedef unsigned short CarUTF8;
typedef void* Joker;
typedef void None;

typedef Ent(*fcomp)(Joker x, Joker y);

#define MALLOC(type)        ((type*)malloc(sizeof(type)))
#define CALLOCN(type, n)    ((type*)calloc( n, sizeof(type)))
#define MALLOCN(type, n)    ((type*)malloc( (n) * sizeof(type)))
#define REALLOC(t, type, n) ((type*)realloc(t, (n) * sizeof(type)))
#define FREE(t) free(t)

// operations sur les chaines de caracteres
#define NatToChaine(x,s) sprintf(s,"%u%c",x,'\0')
#define EntToChaine(x,s) sprintf(s,"%d%c",x,'\0')
#define ReelToChaine(x,s) sprintf(s,"%g%c",x,'\0')
#define chaineLongueur(s) strlen((char*)(s))
#define chaineConcat(sa,sb) strcat((char*)(sa),(char*)(sb))
#define chaineCompare(sa,sb) strcmp((char*)(sa),(char*)(sb))
#define chaineCopie(sa,sb) strcpy((char*)(sa),(char*)(sb))

#endif
