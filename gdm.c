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
    if (s[0] == 0) {
        *u = 0; *nc = 1; return true;
    }
    if (s[0] >= 0x20 && s[0] <= 0x7E) {
        *u = s[0]; *nc = 1; return true;
    }
    else if ((s[0] & 0xE0) == 0xC0) {
        if (s[1] == 0 || (s[1] & 0xC0) != 0x80) { *u = 0x20; *nc = 1; return false; }
        if (s[0] == 0xC2) {
            if (s[1] < 0xA0 || s[1] > 0xBF) { *u = 0x20; *nc = 2; return false; }
        }
        else if (s[0] == 0xC3) {
            if (s[1] < 0x80) { *u = 0x20; *nc = 2; return false; }
        }
        else {
            *u = 0x20; *nc = 2; return false;
        }
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
    if (c >= 0xC380 && c <= 0xC396) return true;
    if (c >= 0xC398 && c <= 0xC3B6) return true;
    if (c >= 0xC3B9 && c <= 0xC3BF) return true;
    return false;
}

Bool estNum(CarUTF8 c) { return (c >= '0' && c <= '9'); }

Ent compareCarUTF8(CarUTF8 x, CarUTF8 y) {
    return (Ent)x - (Ent)y;
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
    Nat fin;
    listeg lsucc, lpred;
} *Noeud;

Noeud nouvNoeud(CarUTF8 c) {
    Noeud n = MALLOC(struct snoeud);
    if (n == NULL) return NULL;
    n->car = c; n->count = 0; n->fin = 0;
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

listeg existeMot(GDM* g, CarUTF8* s);

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

Nat nbNoeuds(GDM* g) {
    Nat count = 0;
    listeg all_nodes = nouvlg();
    for (Nat i = 0; i < N_ASCII; i++) {
        listeg lists[] = {g->racines[i], g->feuilles[i], g->isole[i], g->interne[i]};
        for (int j = 0; j < 4; j++) {
            listeg cur = lists[j];
            while (cur) {
                Noeud n = (Noeud)cur->data;
                if (!elemlg(all_nodes, n)) {
                    all_nodes = adjtetelg(all_nodes, n);
                    count++;
                }
                cur = cur->succ;
            }
        }
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
    if (is_root && is_leaf) {
        if (!elemlg(g->isole[h], n)) g->isole[h] = adjtetelg(g->isole[h], n);
    }
    else if (is_root) {
        if (!elemlg(g->racines[h], n)) g->racines[h] = adjtetelg(g->racines[h], n);
    }
    else if (is_leaf) {
        if (!elemlg(g->feuilles[h], n)) g->feuilles[h] = adjtetelg(g->feuilles[h], n);
    }
    else {
        if (!elemlg(g->interne[h], n)) g->interne[h] = adjtetelg(g->interne[h], n);
    }
}

Noeud _queueNoeud(listeg l) {
    if (l == NULL) return NULL;
    listeg cur = l;
    while (cur->succ != NULL) cur = cur->succ;
    return (Noeud)cur->data;
}

listeg _inverseListe(listeg l) {
    listeg prev = NULL;
    while (l != NULL) {
        listeg n = l->succ;
        l->succ = prev;
        prev = l;
        l = n;
    }
    return prev;
}

None _incrementeCompteurs(listeg chemin) {
    listeg cur = chemin;
    while (cur != NULL) {
        Noeud n = (Noeud)cur->data;
        n->count++;
        cur = cur->succ;
    }
}

Bool _existeCheminNoeud(Noeud src, Noeud dst) {
    if (src == NULL || dst == NULL) return false;

    listeg a_traiter = adjtetelg(NULL, src);
    listeg visites = NULL;

    while (a_traiter != NULL) {
        Noeud cur = (Noeud)tetelg(a_traiter);
        a_traiter = supkiemelg(a_traiter, 0);

        if (cur == dst) {
            detruirelg(a_traiter);
            detruirelg(visites);
            return true;
        }

        if (elemlg(visites, cur) == NULL) {
            visites = adjtetelg(visites, cur);
            listeg succ = cur->lsucc;
            while (succ != NULL) {
                Noeud nx = (Noeud)succ->data;
                if (elemlg(visites, nx) == NULL) a_traiter = adjtetelg(a_traiter, nx);
                succ = succ->succ;
            }
        }
    }

    detruirelg(a_traiter);
    detruirelg(visites);
    return false;
}

Bool _ajouteArcEtMaj(GDM* g, Noeud nx, Noeud ny) {
    if (g == NULL || nx == NULL || ny == NULL) return false;
    if (elemlg(nx->lsucc, ny) != NULL) return false;
    if (_existeCheminNoeud(ny, nx)) return false;
    nx->lsucc = adjtetelg(nx->lsucc, ny);
    ny->lpred = adjtetelg(ny->lpred, nx);
    _updateNodeCategory(g, nx);
    _updateNodeCategory(g, ny);
    return true;
}

listeg _trouveCheminSuccRec(Noeud current, CarUTF8* s, Nat pos, Nat len) {
    if (current == NULL || s == NULL || len == 0) return NULL;
    if (pos + 1 == len) return adjtetelg(NULL, current);

    CarUTF8 c = s[pos + 1];
    listeg cur = current->lsucc;
    while (cur != NULL) {
        Noeud next = (Noeud)cur->data;
        if (next->car == c) {
            listeg rec = _trouveCheminSuccRec(next, s, pos + 1, len);
            if (rec != NULL) return adjtetelg(rec, current);
        }
        cur = cur->succ;
    }
    return NULL;
}

listeg _trouvePrefixeLongueur(GDM* g, CarUTF8* s, Nat len_prefixe, Nat len_mot) {
    if (g == NULL || s == NULL || len_prefixe == 0) return NULL;

    Nat h = s[0] % N_ASCII;
    listeg starts[] = {g->racines[h], g->isole[h]};
    for (int i = 0; i < 2; i++) {
        listeg cur = starts[i];
        while (cur != NULL) {
            Noeud n = (Noeud)cur->data;
            if (n->car == s[0]) {
                listeg chemin = _trouveCheminSuccRec(n, s, 0, len_prefixe);
                if (chemin != NULL) {
                    (void)len_mot;
                    return chemin;
                }
            }
            cur = cur->succ;
        }
    }
    return NULL;
}

listeg _plusLongPrefixe(GDM* g, CarUTF8* s, Nat len_mot, Nat* len_prefixe) {
    if (len_prefixe == NULL) return NULL;
    *len_prefixe = 0;
    if (g == NULL || s == NULL || len_mot == 0) return NULL;

    Nat l = len_mot;
    while (l > 0) {
        listeg p = _trouvePrefixeLongueur(g, s, l, len_mot);
        if (p != NULL) {
            *len_prefixe = l;
            return p;
        }
        l--;
    }
    return NULL;
}

listeg _trouveCheminPredRec(Noeud current, CarUTF8* s, Nat pos, Nat debut) {
    if (current == NULL || s == NULL) return NULL;
    if (pos == debut) return adjtetelg(NULL, current);

    CarUTF8 c = s[pos - 1];
    listeg cur = current->lpred;
    while (cur != NULL) {
        Noeud pred = (Noeud)cur->data;
        if (pred->car == c) {
            listeg rec = _trouveCheminPredRec(pred, s, pos - 1, debut);
            if (rec != NULL) return adjtetelg(rec, current);
        }
        cur = cur->succ;
    }
    return NULL;
}

listeg _trouveSuffixeLongueur(GDM* g, CarUTF8* s, Nat len_mot, Nat len_suffixe) {
    if (g == NULL || s == NULL || len_mot == 0 || len_suffixe == 0) return NULL;

    Nat debut = len_mot - len_suffixe;
    Nat h = s[len_mot - 1] % N_ASCII;
    listeg ends[] = {g->feuilles[h], g->isole[h]};

    for (int i = 0; i < 2; i++) {
        listeg cur = ends[i];
        while (cur != NULL) {
            Noeud leaf = (Noeud)cur->data;
            if (leaf->car == s[len_mot - 1]) {
                listeg rev = _trouveCheminPredRec(leaf, s, len_mot - 1, debut);
                if (rev != NULL) {
                    listeg fwd = _inverseListe(rev);
                    Noeud start = (Noeud)tetelg(fwd);
                    if (len_suffixe < len_mot && estRacine(start)) {
                        detruirelg(fwd);
                    }
                    else if (len_suffixe == len_mot && !estRacine(start)) {
                        detruirelg(fwd);
                    }
                    else {
                        return fwd;
                    }
                }
            }
            cur = cur->succ;
        }
    }
    return NULL;
}

listeg _plusLongSuffixe(GDM* g, CarUTF8* s, Nat len_mot, Nat max_suffixe, Nat* len_suffixe) {
    if (len_suffixe == NULL) return NULL;
    *len_suffixe = 0;
    if (g == NULL || s == NULL || len_mot == 0 || max_suffixe == 0) return NULL;

    Nat l = max_suffixe;
    while (l > 0) {
        listeg p = _trouveSuffixeLongueur(g, s, len_mot, l);
        if (p != NULL) {
            *len_suffixe = l;
            return p;
        }
        l--;
    }
    return NULL;
}

// ============================================================================
// adjMot - Ajoute un mot
// ============================================================================

None adjMot(GDM* g, CarUTF8* s) {
    if (g == NULL || s == NULL) return;
    Nat mot_len = chaineUTF8Longueur(s);
    if (mot_len == 0) return;

    if (mot_len == 1) {
        Noeud iso = existeNoeud(g, s[0], true, true);
        if (iso == NULL) {
            iso = nouvNoeud(s[0]);
            if (iso == NULL) return;
            g->isole[hashNoeud(iso)] = adjtetelg(g->isole[hashNoeud(iso)], iso);
        }
        iso->count++;
        iso->fin++;
        return;
    }

    listeg deja = existeMot(g, s);
    if (!videlg(deja)) {
        _incrementeCompteurs(deja);
        Noeud fin = _queueNoeud(deja);
        if (fin != NULL) fin->fin++;
        detruirelg(deja);
        return;
    }

    Nat lp = 0;
    listeg prefixe = _plusLongPrefixe(g, s, mot_len, &lp);

    Nat ls = 0;
    Nat max_suffixe = mot_len - lp;
    listeg suffixe = _plusLongSuffixe(g, s, mot_len, max_suffixe, &ls);

    // Si prefixe+suffixe couvrent tout le mot, un raccord direct peut introduire un cycle.
    if (lp > 0 && ls > 0 && lp + ls == mot_len) {
        Noeud fin_prefixe = _queueNoeud(prefixe);
        Noeud debut_suffixe = (Noeud)tetelg(suffixe);
        Bool raccord_risque = false;
        if (fin_prefixe == debut_suffixe) raccord_risque = true;
        if (!raccord_risque && _existeCheminNoeud(debut_suffixe, fin_prefixe)) raccord_risque = true;

        if (raccord_risque) {
            Nat new_lp = 0;
            if (lp > 0) new_lp = lp - 1;

            detruirelg(prefixe);
            prefixe = NULL;
            lp = 0;

            if (new_lp > 0) {
                prefixe = _trouvePrefixeLongueur(g, s, new_lp, mot_len);
                if (prefixe != NULL) lp = new_lp;
            }

            max_suffixe = mot_len - lp;
            detruirelg(suffixe);
            suffixe = _plusLongSuffixe(g, s, mot_len, max_suffixe, &ls);
        }
    }

    Noeud prev = _queueNoeud(prefixe);

    Nat i = lp;
    while (i < mot_len - ls) {
        Noeud n = nouvNoeud(s[i]);
        if (n == NULL) {
            detruirelg(prefixe);
            detruirelg(suffixe);
            return;
        }
        g->isole[hashNoeud(n)] = adjtetelg(g->isole[hashNoeud(n)], n);

        if (prev != NULL) _ajouteArcEtMaj(g, prev, n);
        prev = n;
        i++;
    }

    if (suffixe != NULL && prev != NULL) {
        Nat ls_local = ls;
        Bool raccord_ok = false;

        while (!raccord_ok && suffixe != NULL && prev != NULL) {
            Noeud debut_suffixe = (Noeud)tetelg(suffixe);

            if (_ajouteArcEtMaj(g, prev, debut_suffixe)) {
                raccord_ok = true;
            } else {
                Nat idx = mot_len - ls_local;
                if (idx < mot_len) {
                    Noeud n = nouvNoeud(s[idx]);
                    if (n == NULL) {
                        detruirelg(prefixe);
                        detruirelg(suffixe);
                        return;
                    }
                    g->isole[hashNoeud(n)] = adjtetelg(g->isole[hashNoeud(n)], n);
                    _ajouteArcEtMaj(g, prev, n);
                    prev = n;
                }

                if (ls_local > 0) ls_local--;
                detruirelg(suffixe);
                suffixe = NULL;
                if (ls_local > 0) suffixe = _trouveSuffixeLongueur(g, s, mot_len, ls_local);
            }
        }

        ls = ls_local;
    }

    listeg ajoute = _trouvePrefixeLongueur(g, s, mot_len, mot_len);
    if (!videlg(ajoute)) {
        _incrementeCompteurs(ajoute);
        Noeud fin = _queueNoeud(ajoute);
        if (fin != NULL) fin->fin++;
        detruirelg(ajoute);
    }

    detruirelg(prefixe);
    detruirelg(suffixe);
}

// ============================================================================
// existeMot - Recherche un mot 
// ============================================================================

listeg _existeMot_rec(Noeud current, CarUTF8* s, Nat pos, Nat len) {
    // Fin du mot: retourner sentinelle non-NULL pour indiquer succès
    if (pos >= len) {
        if (current != NULL && current->fin > 0) return adjtetelg(NULL, (Joker)0x1);
        return NULL;
    }
    
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
    
    // Le premier caractere doit partir d'une racine (ou noeud isole)
    listeg lists[] = {g->racines[h], g->isole[h], NULL, NULL};
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
                    return result;
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
    Nat mot_len = chaineUTF8Longueur(s);
    if (mot_len == 0) return 0;

    listeg prefix = _trouvePrefixeLongueur(g, s, mot_len, mot_len);
    if (prefix == NULL) return 0;

    Noeud current_node = _queueNoeud(prefix);
    if (current_node == NULL) {
        detruirelg(prefix);
        return 0;
    }

    Nat nc = 0;
    _dfs_completion(current_node, prefix, mot_len, mots, &nc, max);
    detruirelg(prefix);
    return nc;
}

None _dfs_completion(Noeud n, listeg prefix, Nat depth, listeg mots[], Nat* nc, Nat max) {
    if (n == NULL || *nc >= max) return;
    (void)depth;

    if (estFeuille(n)) {
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

    printf("TEST BASIQUE\n----------------\n");
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
        printf("DBG step %u: N=%u R=%u F=%u A=%u\n", i, nbNoeuds(&g), nbRacines(&g), nbFeuilles(&g), nbArcs(&g));
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
        txt[k] = c;
        k++;
    }
    txt[k] = 0;
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
