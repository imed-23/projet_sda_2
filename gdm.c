// ============================================================================
// gdm.c - Graphe de Mots Projet SDA2
// ============================================================================


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

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
    listeg mots;
} GDM;

typedef struct smotmemo {
    CarUTF8* s;
    Nat len;
    Noeud* chemin;
} *MotMemo;

Bool _motsEgaux(CarUTF8* a, Nat la, CarUTF8* b, Nat lb) {
    if (a == NULL || b == NULL || la != lb) return false;
    for (Nat i = 0; i < la; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

Bool _motEstEnregistre(GDM* g, CarUTF8* s, Nat len) {
    if (g == NULL || s == NULL || len == 0) return false;
    listeg cur = g->mots;
    while (cur != NULL) {
        MotMemo m = (MotMemo)cur->data;
        if (m != NULL && _motsEgaux(m->s, m->len, s, len)) return true;
        cur = cur->succ;
    }
    return false;
}

MotMemo _trouveMotMemo(GDM* g, CarUTF8* s, Nat len) {
    if (g == NULL || s == NULL || len == 0) return NULL;
    listeg cur = g->mots;
    while (cur != NULL) {
        MotMemo m = (MotMemo)cur->data;
        if (m != NULL && _motsEgaux(m->s, m->len, s, len)) return m;
        cur = cur->succ;
    }
    return NULL;
}

None _enregistrerMotUnique(GDM* g, CarUTF8* s, Nat len) {
    if (g == NULL || s == NULL || len == 0) return;
    if (_motEstEnregistre(g, s, len)) return;

    MotMemo m = MALLOC(struct smotmemo);
    if (m == NULL) return;

    m->s = MALLOCN(CarUTF8, len + 1);
    if (m->s == NULL) {
        FREE(m);
        return;
    }

    for (Nat i = 0; i < len; i++) m->s[i] = s[i];
    m->s[len] = 0;
    m->len = len;
    m->chemin = NULL;
    g->mots = adjtetelg(g->mots, m);
}

None _enregistrerCheminMot(GDM* g, CarUTF8* s, Nat len, Noeud chemin[]) {
    if (g == NULL || s == NULL || len == 0 || chemin == NULL) return;
    MotMemo m = _trouveMotMemo(g, s, len);
    if (m == NULL) return;

    // Conserver le dernier chemin observe pour refleter l'etat final du graphe.
    if (m->chemin != NULL) FREE(m->chemin);
    m->chemin = MALLOCN(Noeud, len);
    if (m->chemin == NULL) return;

    for (Nat i = 0; i < len; i++) m->chemin[i] = chemin[i];
}

listeg _listeDepuisCheminMemo(MotMemo m) {
    if (m == NULL || m->chemin == NULL || m->len == 0) return NULL;

    listeg l = NULL;
    for (Ent i = (Ent)m->len - 1; i >= 0; i--) {
        l = adjtetelg(l, m->chemin[i]);
    }
    return l;
}

Bool _cheminEstMotValide(GDM* g, listeg chemin) {
    if (g == NULL || chemin == NULL) return false;

    Nat len = longueurlg(chemin);
    if (len == 0) return false;

    CarUTF8* w = MALLOCN(CarUTF8, len + 1);
    if (w == NULL) return false;

    Nat i = 0;
    listeg cur = chemin;
    while (cur != NULL && i < len) {
        Noeud n = (Noeud)cur->data;
        w[i++] = n->car;
        cur = cur->succ;
    }
    w[i] = 0;

    Bool ok = _motEstEnregistre(g, w, len);
    FREE(w);
    return ok;
}

None initGDM(GDM* g) {
    for (Nat i = 0; i < N_ASCII; i++) {
        g->racines[i] = nouvlg(); g->feuilles[i] = nouvlg();
        g->isole[i] = nouvlg(); g->interne[i] = nouvlg();
    }
    g->mots = nouvlg();
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

    listeg curm = g->mots;
    while (curm != NULL) {
        MotMemo m = (MotMemo)curm->data;
        if (m != NULL) {
            FREE(m->s);
            FREE(m->chemin);
            FREE(m);
        }
        curm = curm->succ;
    }
    detruirelg(g->mots);
    g->mots = nouvlg();
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

Nat _sommeCountsChemin(listeg l) {
    Nat s = 0;
    while (l != NULL) {
        Noeud n = (Noeud)l->data;
        if (n != NULL) s += n->count;
        l = l->succ;
    }
    return s;
}

Bool _cheminMieuxPartage(listeg candidat, listeg reference) {
    if (candidat == NULL) return false;
    if (reference == NULL) return true;

    listeg rc = _inverseListe(clonelg(candidat));
    listeg rr = _inverseListe(clonelg(reference));

    listeg c = rc;
    listeg r = rr;
    while (c != NULL && r != NULL) {
        Noeud nc = (Noeud)c->data;
        Noeud nr = (Noeud)r->data;
        Nat cc = (nc != NULL) ? nc->count : 0;
        Nat cr = (nr != NULL) ? nr->count : 0;
        if (cc != cr) {
            detruirelg(rc);
            detruirelg(rr);
            return cc > cr;
        }
        c = c->succ;
        r = r->succ;
    }

    detruirelg(rc);
    detruirelg(rr);

    return _sommeCountsChemin(candidat) > _sommeCountsChemin(reference);
}

listeg _trouveSuffixeLongueur(GDM* g, CarUTF8* s, Nat len_mot, Nat len_suffixe, listeg prefixe) {
    if (g == NULL || s == NULL || len_mot == 0 || len_suffixe == 0) return NULL;

    Nat debut = len_mot - len_suffixe;
    Nat h = s[len_mot - 1] % N_ASCII;
    listeg ends[] = {g->feuilles[h], g->isole[h]};
    listeg best = NULL;

    for (int i = 0; i < 2; i++) {
        listeg cur = ends[i];
        while (cur != NULL) {
            Noeud leaf = (Noeud)cur->data;
            if (leaf->car == s[len_mot - 1]) {
                listeg rev = _trouveCheminPredRec(leaf, s, len_mot - 1, debut);
                if (rev != NULL) {
                    listeg fwd = _inverseListe(rev);
                    Bool has_overlap = false;
                    if (prefixe != NULL) {
                        listeg cfwd = fwd;
                        while (cfwd != NULL) {
                            if (elemlg(prefixe, cfwd->data)) { has_overlap = true; break; }
                            cfwd = cfwd->succ;
                        }
                    }
                    if (has_overlap) {
                        detruirelg(fwd);
                        cur = cur->succ;
                        continue;
                    }
                    Noeud start = (Noeud)tetelg(fwd);
                    if (len_suffixe < len_mot && estRacine(start)) {
                        detruirelg(fwd);
                    }
                    else if (len_suffixe == len_mot && !estRacine(start)) {
                        detruirelg(fwd);
                    }
                    else {
                        if (_cheminMieuxPartage(fwd, best)) {
                            if (best != NULL) detruirelg(best);
                            best = fwd;
                        }
                        else {
                            detruirelg(fwd);
                        }
                    }
                }
            }
            cur = cur->succ;
        }
    }
    return best;
}

listeg _plusLongSuffixe(GDM* g, CarUTF8* s, Nat len_mot, Nat max_suffixe, listeg prefixe, Nat* len_suffixe) {
    if (len_suffixe == NULL) return NULL;
    *len_suffixe = 0;
    if (g == NULL || s == NULL || len_mot == 0 || max_suffixe == 0) return NULL;

    Nat l = max_suffixe;
    while (l > 0) {
        listeg p = _trouveSuffixeLongueur(g, s, len_mot, l, prefixe);
        if (p != NULL) {
            *len_suffixe = l;
            return p;
        }
        l--;
    }
    return NULL;
}

// ============================================================================
// adjMot - Ajoute un mot (avec partage préfixe ET suffixe)
// ============================================================================

None adjMot(GDM* g, CarUTF8* s) {
    if (g == NULL || s == NULL) return;
    Nat mot_len = chaineUTF8Longueur(s);
    if (mot_len == 0) return;

    Bool deja_enregistre = _motEstEnregistre(g, s, mot_len);
    if (!deja_enregistre) {
        _enregistrerMotUnique(g, s, mot_len);
    }

    // Cas mot d'un seul caractère
    if (mot_len == 1) {
        Nat h = s[0] % N_ASCII;
        Noeud iso = NULL;
        listeg lists[] = {g->racines[h], g->feuilles[h], g->isole[h], g->interne[h]};
        for (int j = 0; j < 4 && iso == NULL; j++) {
            listeg cur = lists[j];
            while (cur) {
                Noeud n = (Noeud)cur->data;
                if (n->car == s[0]) { iso = n; break; }
                cur = cur->succ;
            }
        }
        if (iso == NULL) {
            iso = nouvNoeud(s[0]);
            if (iso == NULL) return;
            g->isole[hashNoeud(iso)] = adjtetelg(g->isole[hashNoeud(iso)], iso);
        }
        iso->count++;
        iso->fin++;
        return;
    }

    // Si le mot etait deja insere, incrementer en priorite le chemin memorise.
    if (deja_enregistre) {
        MotMemo m = _trouveMotMemo(g, s, mot_len);
        if (m != NULL && m->chemin != NULL) {
            listeg deja = _listeDepuisCheminMemo(m);
            if (!videlg(deja)) {
                _incrementeCompteurs(deja);
                Noeud fin = _queueNoeud(deja);
                if (fin != NULL) fin->fin++;
                detruirelg(deja);
                return;
            }
        }

        // Repli de securite si le chemin memo n'est pas disponible.
        listeg deja = existeMot(g, s);
        if (!videlg(deja)) {
            _incrementeCompteurs(deja);
            Noeud fin = _queueNoeud(deja);
            if (fin != NULL) fin->fin++;
            detruirelg(deja);
            return;
        }
    }

    // --- Trouver le plus long préfixe existant ---
    Nat lp = 0;
    listeg prefixe = _plusLongPrefixe(g, s, mot_len, &lp);

    // --- Trouver le plus long suffixe existant (sans chevaucher le préfixe) ---
    Nat ls = 0;
    listeg suffixe = NULL;
    Nat max_suffix = mot_len - lp;
    if (max_suffix > 0) {
        suffixe = _plusLongSuffixe(g, s, mot_len, max_suffix, prefixe, &ls);
    }



    // Construire le tableau du chemin complet
    Noeud chemin[26];
    for (Nat i = 0; i < mot_len && i < 26; i++) chemin[i] = NULL;

    // Remplir les nœuds du préfixe
    if (prefixe != NULL) {
        listeg cur = prefixe;
        Nat idx = 0;
        while (cur != NULL && idx < lp) {
            chemin[idx] = (Noeud)cur->data;
            cur = cur->succ;
            idx++;
        }
    }

    // Remplir les nœuds du suffixe
    if (suffixe != NULL) {
        listeg cur = suffixe;
        Nat idx = mot_len - ls;
        while (cur != NULL && idx < mot_len) {
            chemin[idx] = (Noeud)cur->data;
            cur = cur->succ;
            idx++;
        }
    }

    // Si pas de préfixe, créer ou trouver le nœud racine
    if (lp == 0) {
        Nat h0 = s[0] % N_ASCII;
        Noeud premier = NULL;
        listeg cur = g->racines[h0];
        while (cur) {
            Noeud n = (Noeud)cur->data;
            if (n->car == s[0]) { premier = n; break; }
            cur = cur->succ;
        }
        if (premier == NULL) {
            cur = g->isole[h0];
            while (cur) {
                Noeud n = (Noeud)cur->data;
                if (n->car == s[0]) { premier = n; break; }
                cur = cur->succ;
            }
        }
        if (premier == NULL) {
            premier = nouvNoeud(s[0]);
            if (premier == NULL) { detruirelg(prefixe); detruirelg(suffixe); return; }
            g->racines[hashNoeud(premier)] = adjtetelg(g->racines[hashNoeud(premier)], premier);
        }
        chemin[0] = premier;
        lp = 1;
    }

    // Créer les nœuds manquants (entre préfixe et suffixe)
    Nat gap_end = mot_len - ls;
    for (Nat i = lp; i < gap_end; i++) {
        Noeud n = nouvNoeud(s[i]);
        if (n == NULL) { detruirelg(prefixe); detruirelg(suffixe); return; }
        chemin[i] = n;
        Nat h = hashNoeud(n);
        g->isole[h] = adjtetelg(g->isole[h], n);
    }

    // Vérifier si la connexion gap->suffixe créerait un cycle
    if (ls > 0 && gap_end > 0 && chemin[gap_end - 1] != NULL && chemin[gap_end] != NULL) {
        if (_existeCheminNoeud(chemin[gap_end], chemin[gap_end - 1])) {
            // Cycle détecté : abandonner le suffixe, créer des nœuds frais
            for (Nat i = gap_end; i < mot_len; i++) {
                Noeud n = nouvNoeud(s[i]);
                if (n == NULL) { detruirelg(prefixe); detruirelg(suffixe); return; }
                chemin[i] = n;
                Nat h = hashNoeud(n);
                g->isole[h] = adjtetelg(g->isole[h], n);
            }
            ls = 0;
        }
    }

    // Connecter tous les nœuds consécutifs qui ne sont pas encore connectés
    for (Nat i = 0; i < mot_len - 1; i++) {
        Noeud nx = chemin[i];
        Noeud ny = chemin[i + 1];
        if (nx == NULL || ny == NULL) continue;
        if (elemlg(nx->lsucc, ny) == NULL) {
            nx->lsucc = adjtetelg(nx->lsucc, ny);
            ny->lpred = adjtetelg(ny->lpred, nx);
            _updateNodeCategory(g, nx);
            _updateNodeCategory(g, ny);
        }
    }

    // Incrémenter les compteurs pour tous les nœuds du chemin
    for (Nat i = 0; i < mot_len; i++) {
        if (chemin[i] != NULL) chemin[i]->count++;
    }

    // Marquer la fin du mot
    if (chemin[mot_len - 1] != NULL) chemin[mot_len - 1]->fin++;

    _enregistrerCheminMot(g, s, mot_len, chemin);

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
    listeg best = NULL;
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
                if (_cheminMieuxPartage(result, best)) {
                    if (best != NULL) detruirelg(best);
                    best = result;
                }
                else {
                    detruirelg(result);
                }
            }
        }
        cur = cur->succ;
    }
    return best;
}

listeg existeMot(GDM* g, CarUTF8* s) {
    if (g == NULL || s == NULL) return NULL;
    Nat mot_len = chaineUTF8Longueur(s);
    if (mot_len == 0) return NULL;
    if (!_motEstEnregistre(g, s, mot_len)) return NULL;
    
    CarUTF8 c0 = s[0];
    Nat h = c0 % N_ASCII;
    
    // Le premier caractere doit partir d'une racine (ou noeud isole)
    listeg best = NULL;
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
                    if (_cheminMieuxPartage(result, best)) {
                        if (best != NULL) detruirelg(best);
                        best = result;
                    }
                    else {
                        detruirelg(result);
                    }
                }
            }
            cur = cur->succ;
        }
    }
    return best;
}

// ============================================================================
// completion
// ============================================================================

None _dfs_completion(GDM* g, Noeud n, listeg prefix, Nat depth, listeg mots[], Nat* nc, Nat max);

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
    _dfs_completion(g, current_node, prefix, mot_len, mots, &nc, max);
    detruirelg(prefix);
    return nc;
}

None _dfs_completion(GDM* g, Noeud n, listeg prefix, Nat depth, listeg mots[], Nat* nc, Nat max) {
    if (n == NULL || *nc >= max) return;
    if (depth > 25) return;

    Nat ns = longueurlg(n->lsucc);
    if (ns > 0) {
        Noeud* children = MALLOCN(Noeud, ns);
        if (children != NULL) {
            Nat filled = 0;
            listeg succ = n->lsucc;
            while (succ != NULL && filled < ns) {
                children[filled++] = (Noeud)succ->data;
                succ = succ->succ;
            }

            for (Nat i = 1; i < filled; i++) {
                Noeud key = children[i];
                Ent j = (Ent)i - 1;
                while (j >= 0 && compareCarUTF8(children[j]->car, key->car) > 0) {
                    children[j + 1] = children[j];
                    j--;
                }
                children[j + 1] = key;
            }

            for (Nat i = 0; i < filled && *nc < max; i++) {
                Noeud child = children[i];
                if (child == NULL) continue;

                listeg extended = clonelg(prefix);
                listeg tail = extended;
                while (tail != NULL && tail->succ != NULL) tail = tail->succ;
                listeg new_node = adjtetelg(NULL, child);
                if (tail == NULL) extended = new_node;
                else tail->succ = new_node;

                _dfs_completion(g, child, extended, depth + 1, mots, nc, max);
                detruirelg(extended);
            }

            FREE(children);
        }
    }

    if (*nc < max && n->fin > 0 && _cheminEstMotValide(g, prefix)) {
        mots[*nc] = clonelg(prefix);
        (*nc)++;
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

    printf("\n\n\n");

    detruireGDM(&g);
    return 0;
}
