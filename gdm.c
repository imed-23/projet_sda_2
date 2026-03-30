// ============================================================================
// gdm.c - Graphe de Mots Projet SDA2
// ============================================================================


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <locale.h>

// ============================================================================
// CONSTANTES ET TYPES
// ============================================================================

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
#define CALLOCN(type, n)    ((type*)calloc(n, sizeof(type)))
#define MALLOCN(type, n)    ((type*)malloc((n) * sizeof(type)))
#define REALLOC(t, type, n) ((type*)realloc(t, (n) * sizeof(type)))
#define FREE(t) free(t)

#define NatToChaine(x,s) sprintf(s,"%u",x)
#define EntToChaine(x,s) sprintf(s,"%d",x)
#define ReelToChaine(x,s) sprintf(s,"%g",x)
#define chaineLongueur(s) strlen((char*)(s))
#define chaineConcat(sa,sb) strcat((char*)(sa),(char*)(sb))
#define chaineCompare(sa,sb) strcmp((char*)(sa),(char*)(sb))
#define chaineCopie(sa,sb) strcpy((char*)(sa),(char*)(sb))

// ============================================================================
// UTF-8
// ============================================================================

Bool enCarUTF8(Car s[4], CarUTF8* u, Nat* nc);
CarUTF8* enChaineUTF8(Chaine str, Nat* length, Nat* n_invalides);
Nat chaineUTF8Longueur(CarUTF8* s);
Bool estAlpha(CarUTF8 c);
Bool estNum(CarUTF8 c);
Ent compareCarUTF8(CarUTF8 x, CarUTF8 y);

Bool enCarUTF8(Car s[4], CarUTF8* u, Nat *nc) {
    if ((s[0] & 0x80) == 0x00) {
        *u = s[0]; *nc = 1; return true;
    }
    else if ((s[0] & 0xE0) == 0xC0) {
        if (s[1] == 0 || (s[1] & 0xC0) != 0x80) { *u = 0x20; *nc = 1; return false; }
        if (s[0] != 0xC3) { *u = 0x20; *nc = 2; return false; }
        *u = (s[0] << 8) | s[1]; *nc = 2; return true;
    }
    else if ((s[0] & 0xF0) == 0xE0) {
        if ((s[1] & 0xC0) == 0x80) {
            if ((s[2] & 0xC0) == 0x80) { *nc = 3; } else { *nc = 2; }
        } else { *nc = 1; }
        *u = 0x20; return false;
    }
    else if ((s[0] & 0xF8) == 0xF0) {
        if ((s[1] & 0xC0) == 0x80) {
            if ((s[2] & 0xC0) == 0x80) {
                if ((s[3] & 0xC0) == 0x80) { *nc = 4; } else { *nc = 3; }
            } else { *nc = 2; }
        } else { *nc = 1; }
        *u = 0x20; return false;
    }
    *u = 0x20; *nc = 1; return false;
}

CarUTF8* enChaineUTF8(Chaine str, Nat* length, Nat* n_invalides) {
    *length = 0; *n_invalides = 0;
    if (str == NULL) return NULL;
    Nat str_len = chaineLongueur(str);
    Nat capacity = str_len + 1;
    CarUTF8* utf8_chain = MALLOCN(CarUTF8, capacity);
    Nat i = 0, j = 0;
    while (i < str_len) {
        CarUTF8 u_char; Nat nc = 0;
        if (enCarUTF8(&str[i], &u_char, &nc) && u_char != 0x20AC) {
            utf8_chain[j++] = u_char; i += nc;
        } else {
            (*n_invalides)++; utf8_chain[j++] = 0x20;
            if (nc > 0) { i += nc; } else { i++; }
        }
    }
    utf8_chain[j] = 0; *length = j; return utf8_chain;
}

Nat chaineUTF8Longueur(CarUTF8* s) {
    if (s == NULL) return 0;
    Nat length = 0;
    while (s[length] != 0) length++;
    return length;
}

Bool estAlpha(CarUTF8 c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
    if ((c >> 8) == 0xC3) return true;
    return false;
}

Bool estNum(CarUTF8 c) { return (c >= '0' && c <= '9'); }

Ent compareCarUTF8(CarUTF8 x, CarUTF8 y) {
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

// ============================================================================
// LISTEG (Liste générique)
// ============================================================================

typedef struct s_maillong { Joker data; struct s_maillong* succ; } *listeg;

listeg nouvlg() { return NULL; }

None detruirelg(listeg l) {
    while (l) { listeg n = l->succ; FREE(l); l = n; }
}

listeg clonelg(listeg l) {
    if (l == NULL) return NULL;
    listeg new_list = MALLOC(struct s_maillong);
    if (new_list == NULL) return NULL;
    new_list->data = l->data; new_list->succ = NULL;
    listeg current_old = l->succ, current_new = new_list;
    while (current_old != NULL) {
        current_new->succ = MALLOC(struct s_maillong);
        if (current_new->succ == NULL) { detruirelg(new_list); return NULL; }
        current_new = current_new->succ;
        current_new->data = current_old->data; current_new->succ = NULL;
        current_old = current_old->succ;
    }
    return new_list;
}

listeg adjtetelg(listeg l, Joker x) {
    listeg m = MALLOC(struct s_maillong); m->data = x; m->succ = l; return m;
}

listeg supkiemelg(listeg l, Nat k) {
    if (l == NULL) return NULL;
    if (k == 0) { listeg t = l->succ; FREE(l); return t; }
    listeg cur = l;
    for (Nat i = 0; cur && i < k - 1; i++) cur = cur->succ;
    if (!cur || !cur->succ) return l;
    listeg t = cur->succ; cur->succ = t->succ; FREE(t); return l;
}

listeg supelemg(listeg l, Joker x, fcomp comp) {
    if (l == NULL) return NULL;
    if (comp(x, l->data) == 0) { listeg t = l->succ; FREE(l); return t; }
    listeg cur = l;
    while (cur->succ && comp(x, cur->succ->data) != 0) cur = cur->succ;
    if (cur->succ) { listeg t = cur->succ; cur->succ = t->succ; FREE(t); }
    return l;
}

listeg rechlg(listeg l, Joker x, fcomp comp) {
    while (l) { if (comp(x, l->data) == 0) return l; l = l->succ; }
    return NULL;
}

listeg nextlg(listeg l) { return l ? l->succ : NULL; }

listeg elemlg(listeg l, Joker x) {
    while (l) { if (l->data == x) return l; l = l->succ; }
    return NULL;
}

Joker tetelg(listeg l) { return l ? l->data : NULL; }
Bool videlg(listeg l) { return l == NULL; }

Nat longueurlg(listeg l) {
    Nat c = 0;
    while (l) { c++; l = l->succ; }
    return c;
}

// ============================================================================
// NOEUD
// ============================================================================

typedef struct snoeud {
    CarUTF8 car;
    Nat count;
    listeg lsucc, lpred;
} *Noeud;

Noeud nouvNoeud(CarUTF8 c) {
    Noeud n = MALLOC(struct snoeud);
    if (n == NULL) return NULL;
    n->car = c; n->count = 0;
    n->lsucc = nouvlg(); n->lpred = nouvlg();
    return n;
}

Nat hashNoeud(Noeud nx) { return nx->car % N_ASCII; }

Ent compareNoeudCar(Joker x, Joker y) {
    Noeud nx = (Noeud)x, ny = (Noeud)y;
    return compareCarUTF8(nx->car, ny->car);
}

Ent compareNoeud(Joker x, Joker y) {
    Noeud nx = (Noeud)x, ny = (Noeud)y;
    if (nx == ny) return 0;
    return (nx < ny) ? -1 : 1;
}

Nat degi(Noeud x) { return x ? longueurlg(x->lpred) : 0; }
Nat dege(Noeud x) { return x ? longueurlg(x->lsucc) : 0; }
Bool estRacine(Noeud x) { return x && videlg(x->lpred); }
Bool estFeuille(Noeud x) { return x && videlg(x->lsucc); }
Bool estIsole(Noeud x) { return x && videlg(x->lpred) && videlg(x->lsucc); }

// ============================================================================
// GDM (Graphe de Mots)
// ============================================================================

typedef struct sgdm {
    listeg racines[N_ASCII], feuilles[N_ASCII], isole[N_ASCII], interne[N_ASCII];
} GDM;

None initGDM(GDM* g) {
    for (Nat i = 0; i < N_ASCII; i++) {
        g->racines[i] = nouvlg(); g->feuilles[i] = nouvlg();
        g->isole[i] = nouvlg(); g->interne[i] = nouvlg();
    }
}

None _detruire(listeg tab[N_ASCII]) {
    for (Nat i = 0; i < N_ASCII; i++) {
        listeg cur = tab[i];
        while (cur) { listeg n = cur->succ; FREE(cur); cur = n; }
        tab[i] = nouvlg();
    }
}

None detruireGDM(GDM* g) {
    listeg all_nodes = nouvlg();
    for (Nat i = 0; i < N_ASCII; i++) {
        listeg lists[] = {g->racines[i], g->feuilles[i], g->isole[i], g->interne[i]};
        for (int j = 0; j < 4; j++) {
            listeg cur = lists[j];
            while (cur) {
                Noeud n = (Noeud)cur->data;
                if (!elemlg(all_nodes, n)) all_nodes = adjtetelg(all_nodes, n);
                cur = cur->succ;
            }
        }
    }
    _detruire(g->racines); _detruire(g->feuilles);
    _detruire(g->isole); _detruire(g->interne);
    listeg cur = all_nodes;
    while (cur) {
        Noeud n = (Noeud)cur->data;
        detruirelg(n->lsucc); detruirelg(n->lpred);
        FREE(n);
        cur = cur->succ;
    }
    detruirelg(all_nodes);
}

None adjNoeud(GDM* g, Noeud n, Bool est_racine, Bool est_feuille) {
    Nat h = hashNoeud(n);
    if (est_racine && !elemlg(g->racines[h], n)) g->racines[h] = adjtetelg(g->racines[h], n);
    if (est_feuille && !elemlg(g->feuilles[h], n)) g->feuilles[h] = adjtetelg(g->feuilles[h], n);
    if (!est_racine && !est_feuille && estIsole(n) && !elemlg(g->isole[h], n)) g->isole[h] = adjtetelg(g->isole[h], n);
    if (!estRacine(n) && !estFeuille(n) && !elemlg(g->interne[h], n)) g->interne[h] = adjtetelg(g->interne[h], n);
}

None adjArc(GDM* g, Noeud nx, Noeud ny) {
    if (!elemlg(nx->lsucc, ny)) nx->lsucc = adjtetelg(nx->lsucc, ny);
    if (!elemlg(ny->lpred, nx)) ny->lpred = adjtetelg(ny->lpred, nx);
    (void)g;
}

Noeud existeNoeud(GDM* g, CarUTF8 c, Bool est_racine, Bool est_feuille) {
    Nat h = c % N_ASCII;
    if (est_racine || (!est_racine && !est_feuille)) {
        listeg cur = g->racines[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
    }
    if (est_feuille || (!est_racine && !est_feuille)) {
        listeg cur = g->feuilles[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
    }
    if (!est_racine && !est_feuille) {
        listeg cur = g->isole[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
        cur = g->interne[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
    }
    return NULL;
}

Bool existeArc(GDM* g, Noeud nx, Noeud ny) {
    (void)g;
    return nx && ny && elemlg(nx->lsucc, ny) != NULL;
}

Nat nbFeuilles(GDM* g) {
    Nat c = 0;
    for (Nat i = 0; i < N_ASCII; i++) c += longueurlg(g->feuilles[i]);
    return c;
}

Nat nbRacines(GDM* g) {
    Nat c = 0;
    for (Nat i = 0; i < N_ASCII; i++) c += longueurlg(g->racines[i]);
    return c;
}

Nat nbIsoles(GDM* g) {
    Nat c = 0;
    for (Nat i = 0; i < N_ASCII; i++) c += longueurlg(g->isole[i]);
    return c;
}

Nat nbInternes(GDM* g) {
    Nat c = 0;
    for (Nat i = 0; i < N_ASCII; i++) c += longueurlg(g->interne[i]);
    return c;
}

Nat nbArcs(GDM* g) {
    Nat count = 0;
    listeg all_nodes = nouvlg();
    for (Nat i = 0; i < N_ASCII; i++) {
        listeg lists[] = {g->racines[i], g->feuilles[i], g->isole[i], g->interne[i]};
        for (int j = 0; j < 4; j++) {
            listeg cur = lists[j];
            while (cur) {
                Noeud n = (Noeud)cur->data;
                if (!elemlg(all_nodes, n)) all_nodes = adjtetelg(all_nodes, n);
                cur = cur->succ;
            }
        }
    }
    listeg cur_node = all_nodes;
    while (cur_node) {
        Noeud n = (Noeud)cur_node->data;
        count += longueurlg(n->lsucc);
        cur_node = cur_node->succ;
    }
    detruirelg(all_nodes);
    return count;
}

None _updateNodeCategory(GDM* g, Noeud n) {
    if (n == NULL) return;
    Nat h = hashNoeud(n);
    g->racines[h] = supelemg(g->racines[h], n, compareNoeud);
    g->feuilles[h] = supelemg(g->feuilles[h], n, compareNoeud);
    g->isole[h] = supelemg(g->isole[h], n, compareNoeud);
    g->interne[h] = supelemg(g->interne[h], n, compareNoeud);
    Bool is_root = estRacine(n), is_leaf = estFeuille(n);
    if (is_root && is_leaf) g->isole[h] = adjtetelg(g->isole[h], n);
    else if (is_root) g->racines[h] = adjtetelg(g->racines[h], n);
    else if (is_leaf) g->feuilles[h] = adjtetelg(g->feuilles[h], n);
    else g->interne[h] = adjtetelg(g->interne[h], n);
}

// ============================================================================
// adjMot - Ajoute un mot
// ============================================================================

None adjMot(GDM* g, CarUTF8* s) {
    if (g == NULL || s == NULL) return;
    Noeud prev_node = NULL;
    Nat mot_len = chaineUTF8Longueur(s);
    
    for (Nat i = 0; i < mot_len; i++) {
        CarUTF8 c = s[i];
        Noeud next_node = NULL;
        Nat h = c % N_ASCII;
        
        // Premier caractère: chercher dans TOUS les nœuds
        if (i == 0) {
            listeg lists[] = {g->interne[h], g->racines[h], g->isole[h], g->feuilles[h]};
            for (int j = 0; j < 4 && next_node == NULL; j++) {
                listeg cur = lists[j];
                while (cur && !next_node) {
                    Noeud t = (Noeud)cur->data;
                    if (t->car == c) next_node = t;
                    cur = cur->succ;
                }
            }
        }
        // Caractères suivants: chercher UNIQUEMENT dans les successeurs
        else {
            listeg cur = prev_node->lsucc;
            while (cur && !next_node) {
                Noeud t = (Noeud)cur->data;
                if (t->car == c) next_node = t;
                cur = cur->succ;
            }
        }
        
        // Créer si non trouvé
        if (next_node == NULL) {
            next_node = nouvNoeud(c);
            if (next_node == NULL) return;
            g->isole[h] = adjtetelg(g->isole[h], next_node);
        }
        
        // Ajouter l'arc
        if (prev_node != NULL) {
            Bool arc_added = false;
            if (elemlg(prev_node->lsucc, next_node) == NULL) {
                prev_node->lsucc = adjtetelg(prev_node->lsucc, next_node);
                arc_added = true;
            }
            if (elemlg(next_node->lpred, prev_node) == NULL) {
                next_node->lpred = adjtetelg(next_node->lpred, prev_node);
                arc_added = true;
            }
            if (arc_added) {
                _updateNodeCategory(g, prev_node);
                _updateNodeCategory(g, next_node);
            }
        }
        
        next_node->count++;
        prev_node = next_node;
    }
}

// ============================================================================
// existeMot - Recherche un mot 
// ============================================================================

listeg _existeMot_rec(Noeud current, CarUTF8* s, Nat pos, Nat len) {
    // Fin du mot: retourner sentinelle non-NULL pour indiquer succès
    if (pos >= len) return adjtetelg(NULL, (Joker)0x1);
    
    CarUTF8 c = s[pos];
    listeg cur = current->lsucc;
    while (cur) {
        Noeud next = (Noeud)cur->data;
        if (next && next->car == c) {
            listeg result = _existeMot_rec(next, s, pos + 1, len);
            if (result != NULL) {
                // Retirer sentinelle si présente
                if (result->data == (Joker)0x1) {
                    listeg sentinel = result;
                    result = result->succ;
                    FREE(sentinel);
                }
                result = adjtetelg(result, next);
                return result;
            }
        }
        cur = cur->succ;
    }
    return NULL;
}

listeg existeMot(GDM* g, CarUTF8* s) {
    if (g == NULL || s == NULL) return NULL;
    Nat mot_len = chaineUTF8Longueur(s);
    if (mot_len == 0) return NULL;
    
    CarUTF8 c0 = s[0];
    Nat h = c0 % N_ASCII;
    
    // Essayer TOUS les nœuds avec le premier caractère (backtracking)
    listeg lists[] = {g->interne[h], g->racines[h], g->isole[h], g->feuilles[h]};
    for (int j = 0; j < 4; j++) {
        listeg cur = lists[j];
        while (cur) {
            Noeud n = (Noeud)cur->data;
            if (n->car == c0) {
                listeg result = _existeMot_rec(n, s, 1, mot_len);
                if (result != NULL) {
                    if (result->data == (Joker)0x1) {
                        listeg sentinel = result;
                        result = result->succ;
                        FREE(sentinel);
                    }
                    result = adjtetelg(result, n);
                    listeg reversed = NULL;
                    listeg tmp = result;
                    while (tmp) {
                        reversed = adjtetelg(reversed, tmp->data);
                        tmp = tmp->succ;
                    }
                    detruirelg(result);
                    listeg tmp2 = reversed;
                    while (tmp2 && tmp2->succ) tmp2 = tmp2->succ;
                    if (tmp2 && ((Noeud)tmp2->data)->count > 0) return reversed;
                    detruirelg(reversed);
                }
            }
            cur = cur->succ;
        }
    }
    return NULL;
}

// ============================================================================
// completion
// ============================================================================

None _dfs_completion(Noeud n, listeg prefix, Nat depth, listeg mots[], Nat* nc, Nat max);

Nat completion(GDM* g, CarUTF8* s, listeg mots[], Nat max) {
    if (g == NULL || s == NULL || mots == NULL || max == 0) return 0;
    Noeud current_node = NULL;
    Nat mot_len = chaineUTF8Longueur(s);
    listeg prefix = NULL;

    for (Nat i = 0; i < mot_len; i++) {
        CarUTF8 c = s[i];
        Noeud next_node = NULL;

        if (i == 0) {
            Nat h = c % N_ASCII;
            listeg lists[] = {g->interne[h], g->racines[h], g->isole[h], g->feuilles[h]};
            for (int j = 0; j < 4 && next_node == NULL; j++) {
                listeg cur = lists[j];
                while (cur && !next_node) {
                    Noeud t = (Noeud)cur->data;
                    if (t->car == c) next_node = t;
                    cur = cur->succ;
                }
            }
        } else {
            if (current_node == NULL) { detruirelg(prefix); return 0; }
            listeg cur = current_node->lsucc;
            while (cur && !next_node) {
                Noeud t = (Noeud)cur->data;
                if (t->car == c) next_node = t;
                cur = cur->succ;
            }
        }

        if (next_node == NULL) { detruirelg(prefix); return 0; }
        current_node = next_node;
        prefix = adjtetelg(prefix, current_node);
    }

    listeg rev_prefix = NULL;
    listeg cur = prefix;
    while (cur) { rev_prefix = adjtetelg(rev_prefix, cur->data); cur = cur->succ; }
    detruirelg(prefix);

    Nat nc = 0;
    _dfs_completion(current_node, rev_prefix, mot_len, mots, &nc, max);
    detruirelg(rev_prefix);
    return nc;
}

None _dfs_completion(Noeud n, listeg prefix, Nat depth, listeg mots[], Nat* nc, Nat max) {
    if (n == NULL || *nc >= max) return;
    (void)depth;
    
    if (n->count > 0) {
        mots[*nc] = clonelg(prefix);
        (*nc)++;
        if (*nc >= max) return;
    }
    
    listeg succ = n->lsucc;
    while (succ != NULL && *nc < max) {
        Noeud child = (Noeud)succ->data;
        listeg extended = clonelg(prefix);
        listeg tail = extended;
        while (tail != NULL && tail->succ != NULL) tail = tail->succ;
        listeg new_node = adjtetelg(NULL, child);
        if (tail == NULL) extended = new_node;
        else tail->succ = new_node;
        _dfs_completion(child, extended, depth + 1, mots, nc, max);
        detruirelg(extended);
        succ = succ->succ;
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    printf("TEST CarUTF8\n----------------\n");
    Nat lg, ni;
    unsigned char* utfm[3] = { 
        (unsigned char*)"ùéäçà€", 
        (unsigned char*)"\xC3\xA9motions",
        (unsigned char*)"\xC3\xB1\xE2\x82\xAC\0" 
    };
    for (Nat i = 0; i < 3; i++) {
        CarUTF8* s = enChaineUTF8(utfm[i], &lg, &ni);
        printf("%s - len:%d, invalid:%d\n", utfm[i], lg, ni);
        Nat j = 0; 
        while (s[j] != 0) { 
            printf("%0x(%s) ", s[j], estAlpha(s[j]) ? "a" : " "); 
            j++; 
        }
        printf("\n");
        FREE(s);
    }

    printf("\nTEST BASIQUE\n----------------\n");
    unsigned char* m[15] = { 
        (unsigned char*)"fiction", (unsigned char*)"lotion", (unsigned char*)"notion", 
        (unsigned char*)"nation", (unsigned char*)"locomotion", (unsigned char*)"unions", 
        (unsigned char*)"\xC3\xA9motions", (unsigned char*)"punition",
        (unsigned char*)"natation", (unsigned char*)"pions", (unsigned char*)"unir",
        (unsigned char*)"punir", (unsigned char*)"station", (unsigned char*)"pion", 
        (unsigned char*)"location" 
    };

    GDM g; 
    initGDM(&g);
    for (Nat i = 0; i < 15; i++) {
        CarUTF8* s = enChaineUTF8(m[i], &lg, &ni);
        printf("adj   %d: %s -> %d (invalid:%d)\n", i, m[i], lg, ni);
        adjMot(&g, s);
        FREE(s);
    }
    printf("GDM nracines:%d\n", nbRacines(&g));
    printf("GDM nfeuilles:%d\n", nbFeuilles(&g));
    printf("GDM nInterne:%d\n", nbInternes(&g));
    printf("GDM nArcs: %d\n", nbArcs(&g));

    unsigned char* rech[5] = { 
        (unsigned char*)"passage", (unsigned char*)"vague",
        (unsigned char*)"punition", (unsigned char*)"Homme", (unsigned char*)"permis" 
    };
    for (Nat i = 0; i < 5; i++) {
        CarUTF8* s = enChaineUTF8(rech[i], &lg, &ni);
        listeg mot = existeMot(&g, s);
        if (videlg(mot)) printf("le mot: %s n'existe pas\n", rech[i]);
        else printf("le mot: %s existe\n", rech[i]);
        listeg tmp = mot;
        while (tmp != NULL) {
            Noeud x = (Noeud)tmp->data;
            if ((x->car >> 8) == 0) 
                printf("'%c'0x%x(%d) ", (char)x->car, x->car, x->count);
            else 
                printf("'%c%c'0x%x(%d) ", (char)(x->car >> 8), (char)(x->car & 0xff), x->car, x->count);
            tmp = nextlg(tmp);
        }
        if (!videlg(mot)) printf("\n");
        detruirelg(mot);
        FREE(s);
    }
    detruireGDM(&g);

    printf("\nTEST COMPLET\n----------------\n");
    initGDM(&g);
    FILE* fd = fopen("texte.txt", "rb");
    if (fd == NULL) { perror("cannot open file:"); return 1; }
    Nat capacite = 1000, k = 0;
    unsigned char* txt = MALLOCN(unsigned char, capacite);
    while (!feof(fd)) {
        if (k == capacite) {
            capacite += 1000;
            txt = REALLOC(txt, unsigned char, capacite);
        }
        unsigned char c;
        fread(&c, 1, 1, fd);
        txt[k++] = c;
    }
    txt[--k] = 0;
    fclose(fd);
    printf("Lecture du fichier texte.txt: %u octets\n", k);
    
    CarUTF8* s = enChaineUTF8(txt, &lg, &ni);
    printf("Longueur %d - dont %d caracteres invalides\n", lg, ni);
    FREE(txt);

    k = 0;
    CarUTF8 mot[26];
    Nat i = 0, count = 0;
    while (s[k] != 0) {
        CarUTF8 c = s[k];
        if (estAlpha(c) || estNum(c)) {
            mot[i++] = c;
            if (i == 25) { printf("mot trop long!\n"); return 0; }
        } else {
            mot[i] = 0;
            if (i > 0) {
                if (count % 100 == 0) {
                    printf("mot %d -> len=%d (to position %d)\n", count, i, k);
                    for (Nat q = 0; q < i; q++) {
                        if ((mot[q] >> 8) == 0) 
                            printf("'%c'0x%x ", (char)mot[q], mot[q]);
                        else 
                            printf("'%c%c'0x%x ", (char)(mot[q] >> 8), (char)(mot[q] & 0xff), mot[q]);
                    }
                    printf("\n");
                }
                adjMot(&g, mot);
                count++;
            }
            i = 0;
        }
        k++;
    }
    FREE(s);
    
    printf("GDM nracines:%d\n", nbRacines(&g));
    printf("GDM nfeuilles:%d\n", nbFeuilles(&g));
    printf("GDM nInterne:%d\n", nbInternes(&g));
    printf("GDM nArcs: %d\n", nbArcs(&g));
    
    for (Nat i = 0; i < 5; i++) {
        CarUTF8* s = enChaineUTF8(rech[i], &lg, &ni);
        listeg mot = existeMot(&g, s);
        if (videlg(mot)) printf("le mot: %s n'existe pas\n", rech[i]);
        else printf("le mot: %s existe\n", rech[i]);
        listeg tmp = mot;
        while (tmp != NULL) {
            Noeud x = (Noeud)tmp->data;
            if ((x->car >> 8) == 0) 
                printf("'%c'0x%x(%d) ", (char)x->car, x->car, x->count);
            else 
                printf("'%c%c'0x%x(%d) ", (char)(x->car >> 8), (char)(x->car & 0xff), x->car, x->count);
            tmp = nextlg(tmp);
        }
        if (!videlg(mot)) printf("\n");
        detruirelg(mot);
        FREE(s);
    }

    printf("\nTEST COMPLETION\n----------------\n");
    listeg complet[50];
    CarUTF8 motc[] = { 0x65, 0x73, 0x70, 0 };
    i = 0;  
    while (motc[i] != 0) {
        if ((motc[i] >> 8) == 0) 
            printf("'%c'0x%x ", (char)motc[i], motc[i]);
        else 
            printf("'%c%c'0x%x ", (char)(motc[i] >> 8), (char)(motc[i] & 0xff), motc[i]);
        i++;
    }
    printf(" ... \n");
    
    Nat nc = completion(&g, motc, complet, 50);
    for (Nat i = 0; i < nc; i++) {
        listeg tmp = complet[i];
        printf("mot %d:", i);
        while (tmp != NULL) {
            Noeud x = (Noeud)tmp->data;
            if ((x->car >> 8) == 0) 
                printf("'%c'0x%x ", (char)x->car, x->car);
            else 
                printf("'%c%c'0x%x ", (char)(x->car >> 8), (char)(x->car & 0xff), x->car);
            tmp = nextlg(tmp);
        }
        printf("\n");
        detruirelg(complet[i]);
    }

    detruireGDM(&g);
    return 0;
}
