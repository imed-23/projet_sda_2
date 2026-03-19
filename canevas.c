// canevas.c : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include "base.h"

/// Chaines UTF8
/// ///////////////////////////////////
/// 
/// // PRE: s!=NULL && u!=NULL && nc!=NULL
Bool enCarUTF8(Car s[4], CarUTF8* u, Nat* nc);
/// // PRE: s!=NULL && length!=NULL && n_invalides!=NULL
CarUTF8* enChaineUTF8(Chaine str, Nat* length, Nat* n_invalides);
/// // PRE: s!=NULL 
Nat chaineUTF8Longueur(CarUTF8* s);
Bool estAlpha(CarUTF8 c);
Bool estNum(CarUTF8 c);
Ent compareCarUTF8(CarUTF8 x, CarUTF8 y);

/// ///////////////////////////////////
Bool enCarUTF8(Car s[4], CarUTF8* u, Nat *nc)
{
	*u = 0x20; *nc = 1; (None)s;
	return false;
}
CarUTF8* enChaineUTF8(Chaine str,  Nat *length, Nat *n_invalides)
{
	(None)str; *length = 0; *n_invalides = 0;
	return NULL;
}
Nat chaineUTF8Longueur(CarUTF8* s)
{
	(None)s;
	return 0;
}
Bool estAlpha(CarUTF8 c)
{
	(None)c;
	return false;
}
Bool estNum(CarUTF8 c)
{
	(None)c;
	return false;
}
Ent compareCarUTF8(CarUTF8 x, CarUTF8 y)
{
	(None)x; (None)y;
	return 0;
}

/// Listeg
/// ///////////////////////////////////
/// 
typedef struct s_maillong {
	Joker data; // la donnée arbitraire
	struct s_maillong* succ; // pour le chainage
} *listeg;

// OPERATIONS sur listeg
// création ex nihilo d'une liste vide
listeg nouvlg();
// destruction des maillons, mais pas des données
None detruirelg(listeg l);
// clonage des maillons de la liste, mais pas des données
listeg clonelg(listeg l);

// MODIFICATEURS
// PRE: x!=NULL
// adjonction d'un maillon en tête sans clonage de x
listeg adjtetelg(listeg l, Joker x);
// supprime le maillon de rang k, le premier étant k=0
// renvoie l si k>=longueur(l)
listeg supkiemelg(listeg l, Nat k);

// ACCES et SELECTEURS
// PRE: x!=NULL && comp!=NULL
// ne crée pas de nouvelle liste, mais
// renvoie un pointeur sur le premier maillon trouvé 
// ou NULL si inexistant
listeg rechlg(listeg l, Joker x, fcomp comp);
// PRE: x!=NULL && comp!=NULL
// crée une nouvelle liste, contenant
// TOUS les éléments pour lesquels comp(x,l->data)==0 
// ou NULL si inexistant
listeg rechalllg(listeg l, Joker x, fcomp comp);
// renvoie le maillon suivant ou NULL
listeg nextlg(listeg l);
// PRE: x!=NULL
// ne crée pas de nouvelle liste, mais
// renvoie un pointeur sur le maillon contenant exactement x
// ou NULL si inexistant
listeg elemlg(listeg l, Joker x);

// AUTRES OBSERVATEURS
// renvoie la donnée du premier maillon ou NULL
Joker tetelg(listeg l);
// renvoie vrai si liste n'a aucun maillon
Bool videlg(listeg l);
// nombre de maillons dans la chaine
Nat longueurlg(listeg l);

/// ///////////////////////////////////
// OPERATIONS sur listeg
listeg nouvlg() { return NULL; }
None detruirelg(listeg l)
{
	(None)l;
}
listeg clonelg(listeg l)
{
	(None)l;
	return NULL;
}
// MODIFICATEURS
listeg adjtetelg(listeg l, Joker x)
{
	(None)l; (None)x;
	return NULL;
}
listeg supkiemelg(listeg l, Nat k)
{
	(None)l; (None)k;
	return NULL;
}
// ACCES et SELECTEURS
listeg rechlg(listeg l, Joker x, fcomp comp)
{
	(None)l; (None)x; (None)comp;
	return NULL;
}
listeg rechalllg(listeg l, Joker x, fcomp comp)
{
	(None)l; (None)x; (None)comp;
	return NULL;
}
listeg nextlg(listeg l) 
{
	(None)l;
	return NULL;
}
listeg elemlg(listeg l, Joker x)
{
	(None)l; (None)x;
	return NULL;
}

// AUTRES OBSERVATEURS
Joker tetelg(listeg l) 
{
	(None)l; 
	return NULL;
}
Bool videlg(listeg l) 
{ 
	(None)l;  
	return true; 
}
Nat longueurlg(listeg l)
{
	(None)l;
	return 0;
}

/// GRAPHE DE MOTS : Noeud
/// ///////////////////////////////////
/// 

typedef struct snoeud {
	CarUTF8 car;  // UTF-8 avec 0xc2, 0xc3
	Nat count;	  // pour compter les occurences
	listeg lsucc, lpred; // pour les arcs du graphe 
} *Noeud;

Noeud nouvNoeud(CarUTF8 c);
// PRE: x!=NULL
Nat hashNoeud(Noeud x);
// PRE: x!=NULL && y!=NULL
Ent compareNoeudCar(Joker x, Joker y);
// PRE: x!=NULL
Nat degi(Noeud x);
// PRE: x!=NULL
Nat dege(Noeud x);
// PRE: x!=NULL
Bool estRacine(Noeud x);
// PRE: x!=NULL
Bool estFeuille(Noeud x);
// PRE: x!=NULL
Bool estIsole(Noeud x);

/// ///////////////////////////////////
Noeud nouvNoeud(CarUTF8 c)
{
	(None)c;
	return NULL;
}
Nat hashNoeud(Noeud nx)
{
	(None)nx;
	return 0;
}
Ent compareNoeudCar(Joker x, Joker y)
{
	(None)x; (None)y;
	return 0;
}
Nat degi(Noeud x) { (None)x; return 0; }
Nat dege(Noeud x) { (None)x; return 0; }
Bool estRacine(Noeud x) { (None)x; return false; }
Bool estFeuille(Noeud x) { (None)x; return false; }
Bool estIsole(Noeud x) { (None)x; return false; }

/// GRAPHE DE MOTS : GDM
/// ///////////////////////////////////
/// 
typedef struct sgdm {
	listeg	racines[N_ASCII];
	listeg  feuilles[N_ASCII];
	listeg  isole[N_ASCII];
	listeg  interne[N_ASCII];
} GDM;

// OPERATIONS sur GDM
None initGDM(GDM* g);
None _detruire(listeg tab[N_ASCII]);
None detruireGDM(GDM* g);

// MODIFICATEURS
//PRE : g!=NULL && n!=NULL 
//&&((est_racine&&!est_feuille)=>videlg(n->lpred)&&!existeNoeud(g,n->car,true,false)) 
//&&((est_feuille&&!est_racine)=>videlg(n->lsucc)&&!existeNoeud(g,n->car,false,true))
//&&((est_feuille&&est_racine)=>(videlg(n->lpred)&&videlg(n->lsucc)) 
//                                           && !existeNoeud(g,n->car,true,true))
None adjNoeud(GDM* g, Noeud n, Bool est_racine, Bool est_feuille);

//PRE : g!=NULL && nx!=NULL && ny!=NULL &&
// !estRacine(g,ny) && !estFeuille(g,nx) && !existeArc(g,nx,ny)
// 
//POST: !existeCircuit(g)
None adjArc(GDM* g, Noeud nx, Noeud ny);

// OBSERVATEURS
//PRE : g!=NULL
Noeud existeNoeud(GDM* g, CarUTF8 c, Bool est_racine, Bool est_feuille);
//PRE : g!=NULL && nx!=NULL && ny!=NULL
Bool existeArc(GDM* g, Noeud nx, Noeud ny);
Nat nbFeuilles(GDM* g);
Nat nbRacines(GDM* g);
Nat nbIsoles(GDM* g);
Nat nbInternes(GDM* g);
Nat nbArcs(GDM* g);

// CHEMINS
//PRE : g!=NULL && s!=NULL && nx!=NULL && ny!=NULL
// tous les noeuds de la liste renvoyée sont internes 
// sauf éventuellement la tête et la queue
listeg existeChemin(GDM* g, CarUTF8* s, Nat nc, Noeud nx, Noeud ny);
//PRE : g!=NULL && s!=NULL
listeg existeMot(GDM* g, CarUTF8* s);
//PRE : g!=NULL && s!=NULL
listeg adjMot(GDM* g, CarUTF8* s);
//PRE : g!=NULL && s!=NULL && mots!=NULL 
Nat completion(GDM* g, CarUTF8* s, listeg mots[], Nat max);

/// ///////////////////////////////////
None initGDM(GDM* g)
{
	(None)g;
}
None detruireGDM(GDM* g)
{
	(None)g;
}

// MODIFICATEURS
None adjNoeud(GDM* g, Noeud n, Bool est_racine, Bool est_feuille)
{
	(None)g; (None)n; (None)est_racine; (None)est_feuille;
}
None adjArc(GDM* g, Noeud nx, Noeud ny)
{
	(None)g; (None)nx; (None)ny;
}

//OBSERVATEURS
Nat nbFeuilles(GDM* g) { (None)g;  return 0; }
Nat nbRacines(GDM* g) { (None)g;  return 0; }
Nat nbIsoles(GDM* g) { (None)g;  return 0; }
Nat nbInternes(GDM* g) { (None)g;  return 0; }
Nat nbArcs(GDM *g) { (None)g;  return 0; }
Noeud existeNoeud(GDM* g, CarUTF8 c, Bool est_racine, Bool est_feuille)
{
	(None)g; (None)c; (None)est_racine; (None)est_feuille;
	return false;
}
listeg tousNoeuds(GDM* g, CarUTF8 c, Bool est_racine, Bool est_feuille)
{
	(None)g; (None)c; (None)est_racine; (None)est_feuille;
	return NULL;
}
Bool existeArc(GDM* g, Noeud nx, Noeud ny)
{
	(None)g; (None)nx; (None)ny;
	return false;
}
//CHEMINS
//////////////////////////////////////////////
listeg existeChemin(GDM* g, CarUTF8* m, Nat nc, Noeud nx, Noeud ny)
{
	(None)g; (None)m;  (None)nc;  (None)nx; (None)ny;
	return NULL;
}
listeg existeMot(GDM* g, CarUTF8* s)
{
	(None)g; (None)s;
	return false;
}
listeg adjMot(GDM* g, CarUTF8* s)
{
	(None)g; (None)s;
	return NULL;
}

//COMPLETION
//////////////////////////////////////////////
Nat completion(GDM* g, CarUTF8* s, listeg mots[], Nat max)
{
	(None)g; (None)s; (None)mots; (None)max;
	return 0;
}

/// MAIN
/// ///////////////////////////////////
/// 
int main()
{
	//setlocale(LC_ALL, "");
	// TEST CARUTF8
	printf("TEST CarUTF8\n----------------\n");
	Nat lg, ni;
	unsigned char* utfm[3] = { (unsigned char*)"ùéäçà€", (unsigned char*)"\xC3\xA9motions",
		(unsigned char*)"\xC3\xB1\xE2\x82\xAC\0" };
	for (Nat i = 0; i < 3; i++)
	{
		CarUTF8* s = enChaineUTF8(utfm[i], &lg, &ni);
		printf("%s - len:%d, invalid:%d\n", utfm[i], lg, ni);
		Nat j = 0; while (s[j] != 0) { printf("%0x(%s) ", s[j],estAlpha(s[j])?"a":" "); j++; }
		printf("\n");
		FREE(s);
	}

	// TEST BASIQUE
	printf("TEST BASIQUE\n----------------\n");
	unsigned char* m[15] = { (unsigned char*)"fiction", (unsigned char*)"lotion", (unsigned char*)"notion", (unsigned char*)"nation", 
		(unsigned char*)"locomotion",(unsigned char*)"unions", (unsigned char*)"\xC3\xA9motions", (unsigned char*)"punition", 
		(unsigned char*)"natation", (unsigned char*)"pions", (unsigned char*)"unir",(unsigned char*)"punir", 
		(unsigned char*)"station", (unsigned char*)"pion", (unsigned char*)"location" };

	GDM g; initGDM(&g);
	for (Nat i = 0; i < 15; i++)
	{
		CarUTF8* s = enChaineUTF8(m[i], &lg, &ni);
		printf("adj   %d: %s -> %d (invalid:%d)\n", i, m[i], lg, ni);
		adjMot(&g, s);
		FREE(s);
	}
	printf("GDM nracines:%d\n", nbRacines(&g));
	printf("GDM nfeuilles:%d\n", nbFeuilles(&g));
	printf("GDM nInterne:%d\n", nbInternes(&g));
	printf("GDM nArcs: %d\n", nbArcs(&g));

	unsigned char* rech[5] = { (unsigned char*)"passage", (unsigned char*)"vague",
			(unsigned char*)"punition" , (unsigned char*)"Homme" , (unsigned char*)"permis" };
	for (Nat i = 0; i < 5; i++)
	{
		CarUTF8* s = enChaineUTF8(rech[i], &lg, &ni);
		listeg mot = existeMot(&g, s);
		if (videlg(mot)) printf("le mot: %s n'existe pas\n", rech[i]);
		else printf("le mot: %s existe\n", rech[i]);
		listeg tmp = mot;
		while (tmp != NULL)
		{
			Noeud x = (Noeud)tmp->data;
			if ((x->car >> 8) == 0) printf("'%c'0x%x(%d) ", (char)x->car, x->car, x->count);
			else printf("'%c%c'0x%x(%d) ", (char)(x->car >> 8), (char)(x->car & 0xff), x->car, x->count);
			tmp = nextlg(tmp);
		}
		if (!videlg(mot)) printf("\n");
		detruirelg(mot);
		FREE(s);
	}
	detruireGDM(&g);

	// TEST COMPLET
	printf("\nTEST COMPLET\n----------------\n");
	initGDM(&g);
	FILE* fd = fopen("texte.txt", "rb");
	if (fd == NULL) {
		perror("cannot open file:");  return 1;
	}
	Nat capacite = 1000;
	Nat k = 0;
	unsigned char* txt = MALLOCN(unsigned char, capacite);
	while (!feof(fd))
	{
		if (k == capacite)
		{
			capacite += 1000;
			txt = REALLOC(txt, unsigned char, capacite);
		}
		unsigned char c;
		fread(&c, 1, 1, fd);
		txt[k] = c;
		k++;
	}
	txt[k] = 0;
	fclose(fd);
	printf("Lecture du fichier texte.txt: %u octets\n", k);
	CarUTF8* s = enChaineUTF8(txt, &lg, &ni);
	printf("Longueur %d - dont %d caracteres invalides\n", lg, ni);

	k = 0;
	CarUTF8 mot[26]; //mot de longueur MAX: anticonstitutionnellement
	Nat i = 0, count=0;
	while (s[k]!=0)
	{
		CarUTF8 c = s[k];
		if (estAlpha(c) || estNum(c))
		{
			mot[i] = c; i++;
			if (i == 25) { printf("mot trop long!\n"); return 0; }
		}
		else
		{
			mot[i] = 0;
			if (i > 0)
			{
				if (count % 100 == 0)
				{
					printf("mot %d -> len=%d (to position %d)\n", count, i, k);
					for (Nat q = 0; q < i; q++) 
						if ((mot[q] >> 8) == 0) printf("'%c'0x%x ", (char)mot[q], mot[q]);
						else printf("'%c%c'0x%x ", (char)(mot[q] >> 8), (char)(mot[q] & 0xff), mot[q]);
					printf("\n");
				}
				adjMot(&g, mot);
				count++;
			}
			i = 0;
		}
		k++;
	}
	printf("GDM nracines:%d\n", nbRacines(&g));
	printf("GDM nfeuilles:%d\n", nbFeuilles(&g));
	printf("GDM nInterne:%d\n", nbInternes(&g));
	printf("GDM nArcs: %d\n", nbArcs(&g));
	FREE(txt);
	FREE(s);
	for (Nat i = 0; i < 5; i++)
	{
		CarUTF8* s = enChaineUTF8(rech[i], &lg, &ni);
		listeg mot = existeMot(&g, s);
		if (videlg(mot)) printf("le mot: %s n'existe pas\n", rech[i]);
		else printf("le mot: %s existe\n", rech[i]);
		listeg tmp = mot;
		while (tmp != NULL)
		{
			Noeud x = (Noeud)tmp->data;
			if ((x->car >> 8) == 0) printf("'%c'0x%x(%d) ", (char)x->car, x->car,x->count);
			else printf("'%c%c'0x%x(%d) ", (char)(x->car >> 8), (char)(x->car & 0xff), x->car,x->count);
			tmp = nextlg(tmp);
		}
		if (!videlg(mot)) printf("\n");
		detruirelg(mot);
		FREE(s);
	}
	// TEST DE COMPLETION
	printf("\nTEST COMPLETION\n----------------\n");
	listeg complet[50];
	CarUTF8 motc[] = { 0x65, 0x73, 0x70, 0 };
	i = 0;  while (motc[i] != 0)
	{
		if ((motc[i] >> 8) == 0) printf("'%c'0x%x ", (char)motc[i], motc[i]);
		else printf("'%c%c'0x%x ", (char)(motc[i] >> 8), (char)(motc[i] & 0xff), motc[i]);
		i++;
	}
	printf(" ... \n");
	Nat nc = completion(&g, motc, complet, 50);
	for (Nat i = 0; i < nc; i++)
	{
		listeg tmp = complet[i];
		printf("mot %u:", i);
		while (tmp != NULL)
		{
			Noeud x = (Noeud)tmp->data;
			if ((x->car>>8)==0) printf("'%c'0x%x ", (char)x->car, x->car);
			else printf("'%c%c'0x%x ", (char)(x->car>>8), (char)(x->car&0xff), x->car);
			tmp = nextlg(tmp);
		}
		printf("\n");
		detruirelg(complet[i]);
	}

	detruireGDM(&g);

	return 0;
}