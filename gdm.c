// ============================================================================
// gdm.c - Graphe de Mots Projet SDA2
// ============================================================================

#include "base.h"

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
    Nat i = 0;
    Nat j = 0;
    while (i < str_len) {
        CarUTF8 u_char;
        Nat nc = 0;
        if (enCarUTF8(&str[i], &u_char, &nc) && u_char != 0x20AC) {
            utf8_chain[j++] = u_char;
            i += nc;
        } else {
            (*n_invalides)++;
            utf8_chain[j++] = 0x20;
            if (nc > 0) { i += nc; } else { i++; }
        }
    }
    utf8_chain[j] = 0;
    *length = j;
    return utf8_chain;
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
// LISTEG (Liste generique)
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
    new_list->data = l->data;
    new_list->succ = NULL;
    listeg current_old = l->succ;
    listeg current_new = new_list;
    while (current_old != NULL) {
        current_new->succ = MALLOC(struct s_maillong);
        if (current_new->succ == NULL) { detruirelg(new_list); return NULL; }
        current_new = current_new->succ;
        current_new->data = current_old->data;
        current_new->succ = NULL;
        current_old = current_old->succ;
    }
    return new_list;
}

listeg adjtetelg(listeg l, Joker x) {
    listeg m = MALLOC(struct s_maillong);
    m->data = x;
    m->succ = l;
    return m;
}

listeg supkiemelg(listeg l, Nat k) {
    if (l == NULL) return NULL;
    if (k == 0) { listeg t = l->succ; FREE(l); return t; }
    listeg cur = l;
    for (Nat i = 0; cur && i < k - 1; i++) cur = cur->succ;
    if (!cur || !cur->succ) return l;
    listeg t = cur->succ;
    cur->succ = t->succ;
    FREE(t);
    return l;
}

listeg rechlg(listeg l, Joker x, fcomp comp) {
    while (l) { if (comp(x, l->data) == 0) return l; l = l->succ; }
    return NULL;
}

listeg rechalllg(listeg l, Joker x, fcomp comp) {
    listeg result = NULL;
    while (l) {
        if (comp(x, l->data) == 0)
            result = adjtetelg(result, l->data);
        l = l->succ;
    }
    return result;
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
    n->car = c;
    n->count = 0;
    n->lsucc = nouvlg();
    n->lpred = nouvlg();
    return n;
}

Nat hashNoeud(Noeud nx) { return nx->car % N_ASCII; }

Ent compareNoeudCar(Joker x, Joker y) {
    Noeud nx = (Noeud)x;
    Noeud ny = (Noeud)y;
    return compareCarUTF8(nx->car, ny->car);
}

Ent compareNoeud(Joker x, Joker y) {
    Noeud nx = (Noeud)x;
    Noeud ny = (Noeud)y;
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
    listeg racines[N_ASCII];
    listeg feuilles[N_ASCII];
    listeg isole[N_ASCII];
    listeg interne[N_ASCII];
} GDM;

listeg supelemg(listeg l, Joker x, fcomp comp) {
    if (l == NULL) return NULL;
    if (comp(x, l->data) == 0) { listeg t = l->succ; FREE(l); return t; }
    listeg cur = l;
    while (cur->succ && comp(x, cur->succ->data) != 0) cur = cur->succ;
    if (cur->succ) { listeg t = cur->succ; cur->succ = t->succ; FREE(t); }
    return l;
}

None initGDM(GDM* g) {
    for (Nat i = 0; i < N_ASCII; i++) {
        g->racines[i] = nouvlg();
        g->feuilles[i] = nouvlg();
        g->isole[i] = nouvlg();
        g->interne[i] = nouvlg();
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
        listeg lists[4];
        lists[0] = g->racines[i];
        lists[1] = g->feuilles[i];
        lists[2] = g->isole[i];
        lists[3] = g->interne[i];
        for (Nat j = 0; j < 4; j++) {
            listeg cur = lists[j];
            while (cur) {
                Noeud n = (Noeud)cur->data;
                if (!elemlg(all_nodes, n)) all_nodes = adjtetelg(all_nodes, n);
                cur = cur->succ;
            }
        }
    }
    _detruire(g->racines);
    _detruire(g->feuilles);
    _detruire(g->isole);
    _detruire(g->interne);
    listeg cur = all_nodes;
    while (cur) {
        Noeud n = (Noeud)cur->data;
        detruirelg(n->lsucc);
        detruirelg(n->lpred);
        FREE(n);
        cur = cur->succ;
    }
    detruirelg(all_nodes);
}

None adjNoeud(GDM* g, Noeud n, Bool est_racine, Bool est_feuille) {
    Nat h = hashNoeud(n);
    if (est_racine && est_feuille) {
        if (!elemlg(g->isole[h], n))
            g->isole[h] = adjtetelg(g->isole[h], n);
    } else if (est_racine) {
        if (!elemlg(g->racines[h], n))
            g->racines[h] = adjtetelg(g->racines[h], n);
    } else if (est_feuille) {
        if (!elemlg(g->feuilles[h], n))
            g->feuilles[h] = adjtetelg(g->feuilles[h], n);
    } else {
        if (!elemlg(g->interne[h], n))
            g->interne[h] = adjtetelg(g->interne[h], n);
    }
}

None adjArc(GDM* g, Noeud nx, Noeud ny) {
    if (!elemlg(nx->lsucc, ny)) nx->lsucc = adjtetelg(nx->lsucc, ny);
    if (!elemlg(ny->lpred, nx)) ny->lpred = adjtetelg(ny->lpred, nx);
    (void)g;
}

None _updateNodeCategory(GDM* g, Noeud n) {
    if (n == NULL) return;
    Nat h = hashNoeud(n);
    g->racines[h] = supelemg(g->racines[h], n, compareNoeud);
    g->feuilles[h] = supelemg(g->feuilles[h], n, compareNoeud);
    g->isole[h] = supelemg(g->isole[h], n, compareNoeud);
    g->interne[h] = supelemg(g->interne[h], n, compareNoeud);
    Bool is_root = estRacine(n);
    Bool is_leaf = estFeuille(n);
    if (is_root && is_leaf) g->isole[h] = adjtetelg(g->isole[h], n);
    else if (is_root)       g->racines[h] = adjtetelg(g->racines[h], n);
    else if (is_leaf)       g->feuilles[h] = adjtetelg(g->feuilles[h], n);
    else                    g->interne[h] = adjtetelg(g->interne[h], n);
}

Noeud existeNoeud(GDM* g, CarUTF8 c, Bool est_racine, Bool est_feuille) {
    Nat h = c % N_ASCII;
    if (est_racine && est_feuille) {
        listeg cur = g->isole[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
        return NULL;
    }
    if (est_racine) {
        listeg cur = g->racines[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
        return NULL;
    }
    if (est_feuille) {
        listeg cur = g->feuilles[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
        return NULL;
    }
    listeg cur = g->racines[h];
    while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
    cur = g->feuilles[h];
    while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
    cur = g->isole[h];
    while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
    cur = g->interne[h];
    while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) return n; cur = cur->succ; }
    return NULL;
}

listeg tousNoeuds(GDM* g, CarUTF8 c, Bool est_racine, Bool est_feuille) {
    Nat h = c % N_ASCII;
    listeg result = NULL;
    if (est_racine && est_feuille) {
        listeg cur = g->isole[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) result = adjtetelg(result, n); cur = cur->succ; }
        return result;
    }
    if (est_racine) {
        listeg cur = g->racines[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) result = adjtetelg(result, n); cur = cur->succ; }
        return result;
    }
    if (est_feuille) {
        listeg cur = g->feuilles[h];
        while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) result = adjtetelg(result, n); cur = cur->succ; }
        return result;
    }
    listeg cur = g->interne[h];
    while (cur) { Noeud n = (Noeud)cur->data; if (n->car == c) result = adjtetelg(result, n); cur = cur->succ; }
    return result;
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
        listeg lists[4];
        lists[0] = g->racines[i];
        lists[1] = g->feuilles[i];
        lists[2] = g->isole[i];
        lists[3] = g->interne[i];
        for (Nat j = 0; j < 4; j++) {
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

// ============================================================================
// Suffix search: find longest suffix from a leaf/isole, each non-leaf
// node must have dege == 1
// Returns a listeg of suffix nodes (first to last), sets *suf_len
// ============================================================================

Nat _suffixDepth(Noeud current, CarUTF8* s, Nat pos) {
    if (pos >= 30000) return 0;
    if (pos == 0) return 0;
    CarUTF8 c = s[pos - 1];
    Nat best = 0;
    listeg pcur = current->lpred;
    while (pcur) {
        Noeud pred = (Noeud)pcur->data;
        if (pred->car == c && dege(pred) == 1) {
            Nat depth = 1 + _suffixDepth(pred, s, pos - 1);
            if (depth > best) best = depth;
        }
        pcur = pcur->succ;
    }
    return best;
}

None _buildSuffix(Noeud current, CarUTF8* s, Nat pos, Nat target_depth,
                  Noeud result[], Nat* idx) {
    if (target_depth == 0) return;
    CarUTF8 c = s[pos - 1];
    listeg pcur = current->lpred;
    while (pcur) {
        Noeud pred = (Noeud)pcur->data;
        if (pred->car == c && dege(pred) == 1) {
            Nat depth = 1 + _suffixDepth(pred, s, pos - 1);
            if (depth == target_depth) {
                result[(*idx)++] = pred;
                _buildSuffix(pred, s, pos - 1, target_depth - 1, result, idx);
                return;
            }
        }
        pcur = pcur->succ;
    }
}

listeg _findSuffix(GDM* g, CarUTF8* s, Nat len, Nat* suf_len) {
    *suf_len = 0;
    if (len == 0) return NULL;

    CarUTF8 last = s[len - 1];
    Nat h = last % N_ASCII;
    Noeud end_node = NULL;

    listeg cur = g->feuilles[h];
    while (cur && !end_node) {
        Noeud n = (Noeud)cur->data;
        if (n->car == last) end_node = n;
        cur = cur->succ;
    }
    if (!end_node) {
        cur = g->isole[h];
        while (cur && !end_node) {
            Noeud n = (Noeud)cur->data;
            if (n->car == last) end_node = n;
            cur = cur->succ;
        }
    }
    if (!end_node) return NULL;

    Nat depth = _suffixDepth(end_node, s, len - 1);
    *suf_len = depth + 1;

    Noeud nodes[26];
    Nat idx = 0;
    nodes[idx++] = end_node;
    _buildSuffix(end_node, s, len - 1, depth, nodes, &idx);

    listeg suffix = NULL;
    for (Nat i = 0; i < idx; i++)
        suffix = adjtetelg(suffix, nodes[i]);
    return suffix;
}

// ============================================================================
// Prefix search: DFS through existing arcs from root, find longest match
// Returns the length of the best prefix found, fills best[] array
// ============================================================================

Nat _prefixDFS(Noeud cur, CarUTF8* s, Nat pos, Nat max_pos,
               listeg suffix_nodes, Noeud path[], Noeud best[], Nat best_len) {
    if (pos > max_pos) return best_len;
    CarUTF8 c = s[pos];

    listeg sc = cur->lsucc;
    while (sc) {
        Noeud next = (Noeud)sc->data;
        if (next->car == c && !estFeuille(next) && !estIsole(next)
            && !elemlg(suffix_nodes, next)) {
            path[pos] = next;
            Nat candidate = pos + 1;
            if (candidate > best_len) {
                best_len = candidate;
                for (Nat k = 0; k < candidate; k++) best[k] = path[k];
            }
            best_len = _prefixDFS(next, s, pos + 1, max_pos, suffix_nodes,
                                  path, best, best_len);
        }
        sc = sc->succ;
    }
    return best_len;
}

// ============================================================================
// adjMot - Ajoute un mot (suffix-first, then prefix, then infixe)
// ============================================================================

listeg existeMot(GDM* g, CarUTF8* s);

listeg adjMot(GDM* g, CarUTF8* s) {
    if (g == NULL || s == NULL) return NULL;
    Nat mot_len = chaineUTF8Longueur(s);
    if (mot_len == 0) return NULL;

    // --- Single character ---
    if (mot_len == 1) {
        Nat h = s[0] % N_ASCII;
        Noeud iso = NULL;
        listeg cur = g->isole[h];
        while (cur) {
            Noeud n = (Noeud)cur->data;
            if (n->car == s[0]) { iso = n; cur = NULL; }
            else cur = cur->succ;
        }
        if (!iso) {
            cur = g->racines[h];
            while (cur) {
                Noeud n = (Noeud)cur->data;
                if (n->car == s[0]) { iso = n; cur = NULL; }
                else cur = cur->succ;
            }
        }
        if (!iso) {
            iso = nouvNoeud(s[0]);
            g->isole[hashNoeud(iso)] = adjtetelg(g->isole[hashNoeud(iso)], iso);
        }
        iso->count++;
        return NULL;
    }

    // --- Word already exists: just increment counts ---
    listeg existing = existeMot(g, s);
    if (!videlg(existing)) {
        listeg tmp = existing;
        while (tmp) {
            ((Noeud)tmp->data)->count++;
            tmp = tmp->succ;
        }
        detruirelg(existing);
        return NULL;
    }

    // --- Find longest suffix (leave position 0 for root) ---
    Nat suf_len = 0;
    listeg suffix_list = _findSuffix(g, s, mot_len, &suf_len);

    // --- Find longest prefix ---
    Nat max_prefix_pos = mot_len - suf_len - 1;
    Noeud chemin[26];
    for (Nat i = 0; i < 26; i++) chemin[i] = NULL;

    Nat pref_len = 0;
    Noeud root_node = NULL;
    Nat h0 = s[0] % N_ASCII;

    listeg rcur = g->racines[h0];
    while (rcur && !root_node) {
        Noeud n = (Noeud)rcur->data;
        if (n->car == s[0]) root_node = n;
        rcur = rcur->succ;
    }
    if (!root_node) {
        rcur = g->isole[h0];
        while (rcur && !root_node) {
            Noeud n = (Noeud)rcur->data;
            if (n->car == s[0]) root_node = n;
            rcur = rcur->succ;
        }
    }

    if (root_node && !elemlg(suffix_list, root_node)) {
        Noeud best[26];
        Noeud path[26];
        path[0] = root_node;
        best[0] = root_node;
        pref_len = 1;
        if (mot_len - suf_len > 1) {
            pref_len = _prefixDFS(root_node, s, 1, max_prefix_pos,
                                  suffix_list, path, best, 1);
        }
        for (Nat i = 0; i < pref_len; i++) chemin[i] = best[i];

        // Trim prefix if last node has degi > 1 (prevent forbidden words)
        while (pref_len > 1 && degi(chemin[pref_len - 1]) > 1) {
            pref_len--;
        }
        if (pref_len == 1 && root_node != NULL) {
            chemin[0] = root_node;
        }
    }

    // --- If no root found, create one ---
    if (pref_len == 0) {
        if (!root_node) {
            root_node = nouvNoeud(s[0]);
            g->racines[hashNoeud(root_node)] =
                adjtetelg(g->racines[hashNoeud(root_node)], root_node);
        }
        chemin[0] = root_node;
        pref_len = 1;
    }

    // --- Fill suffix nodes into chemin ---
    if (suf_len > 0 && suffix_list != NULL) {
        listeg scur = suffix_list;
        Nat idx = mot_len - suf_len;
        while (scur && idx < mot_len) {
            chemin[idx] = (Noeud)scur->data;
            idx++;
            scur = scur->succ;
        }
    }

    // --- Create infixe nodes ---
    Nat gap_start = pref_len;
    Nat gap_end = mot_len - suf_len;
    for (Nat i = gap_start; i < gap_end; i++) {
        Noeud n = nouvNoeud(s[i]);
        chemin[i] = n;
        g->isole[hashNoeud(n)] = adjtetelg(g->isole[hashNoeud(n)], n);
    }

    // --- Connect all consecutive nodes ---
    for (Nat i = 0; i < mot_len - 1; i++) {
        Noeud nx = chemin[i];
        Noeud ny = chemin[i + 1];
        if (nx == NULL || ny == NULL) continue;
        if (!elemlg(nx->lsucc, ny)) {
            nx->lsucc = adjtetelg(nx->lsucc, ny);
            ny->lpred = adjtetelg(ny->lpred, nx);
            _updateNodeCategory(g, nx);
            _updateNodeCategory(g, ny);
        }
    }

    // --- Increment counts ---
    for (Nat i = 0; i < mot_len; i++) {
        if (chemin[i] != NULL) chemin[i]->count++;
    }

    detruirelg(suffix_list);
    return NULL;
}

// ============================================================================
// existeChemin
// ============================================================================

listeg existeChemin(GDM* g, CarUTF8* m, Nat nc, Noeud nx, Noeud ny) {
    (void)g;
    if (nc == 0) {
        if (elemlg(nx->lsucc, ny))
            return adjtetelg(adjtetelg(NULL, ny), nx);
        return NULL;
    }
    CarUTF8 c = m[0];
    listeg cur = nx->lsucc;
    while (cur) {
        Noeud next = (Noeud)cur->data;
        if (next->car == c) {
            listeg sub = existeChemin(g, m + 1, nc - 1, next, ny);
            if (sub != NULL)
                return adjtetelg(sub, nx);
        }
        cur = cur->succ;
    }
    return NULL;
}

// ============================================================================
// existeMot
// ============================================================================

listeg existeMot(GDM* g, CarUTF8* s) {
    if (g == NULL || s == NULL) return NULL;
    Nat mot_len = chaineUTF8Longueur(s);
    if (mot_len == 0) return NULL;

    if (mot_len == 1) {
        Noeud iso = existeNoeud(g, s[0], true, true);
        if (iso && iso->count > 0) return adjtetelg(NULL, iso);
        Noeud root = existeNoeud(g, s[0], true, false);
        if (root && root->count > 0) return adjtetelg(NULL, root);
        return NULL;
    }

    Nat h0 = s[0] % N_ASCII;
    Nat hlast = s[mot_len - 1] % N_ASCII;

    // Find roots/isole matching first char
    Noeud starts[10];
    Nat nstarts = 0;
    listeg cur = g->racines[h0];
    while (cur && nstarts < 10) {
        Noeud n = (Noeud)cur->data;
        if (n->car == s[0]) starts[nstarts++] = n;
        cur = cur->succ;
    }
    cur = g->isole[h0];
    while (cur && nstarts < 10) {
        Noeud n = (Noeud)cur->data;
        if (n->car == s[0]) starts[nstarts++] = n;
        cur = cur->succ;
    }

    // Find leaves/isole matching last char
    Noeud ends[10];
    Nat nends = 0;
    cur = g->feuilles[hlast];
    while (cur && nends < 10) {
        Noeud n = (Noeud)cur->data;
        if (n->car == s[mot_len - 1]) ends[nends++] = n;
        cur = cur->succ;
    }
    cur = g->isole[hlast];
    while (cur && nends < 10) {
        Noeud n = (Noeud)cur->data;
        if (n->car == s[mot_len - 1]) ends[nends++] = n;
        cur = cur->succ;
    }

    for (Nat si = 0; si < nstarts; si++) {
        for (Nat ei = 0; ei < nends; ei++) {
            if (mot_len == 2) {
                if (elemlg(starts[si]->lsucc, ends[ei]))
                    return adjtetelg(adjtetelg(NULL, ends[ei]), starts[si]);
            } else {
                listeg path = existeChemin(g, s + 1, mot_len - 2,
                                           starts[si], ends[ei]);
                if (path != NULL) return path;
            }
        }
    }
    return NULL;
}

// ============================================================================
// completion
// ============================================================================

None _dfs_completion(Noeud n, listeg prefix, listeg mots[], Nat* nc, Nat max) {
    if (n == NULL || *nc >= max) return;

    if (estFeuille(n) || estIsole(n)) {
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

        _dfs_completion(child, extended, mots, nc, max);
        detruirelg(extended);
        succ = succ->succ;
    }
}

Nat completion(GDM* g, CarUTF8* s, listeg mots[], Nat max) {
    if (g == NULL || s == NULL || mots == NULL || max == 0) return 0;
    Nat mot_len = chaineUTF8Longueur(s);
    if (mot_len == 0) return 0;

    Noeud current = NULL;
    listeg prefix = NULL;

    for (Nat i = 0; i < mot_len; i++) {
        CarUTF8 c = s[i];
        Noeud next_node = NULL;
        if (i == 0) {
            Nat h = c % N_ASCII;
            listeg cur = g->racines[h];
            while (cur && !next_node) {
                Noeud t = (Noeud)cur->data;
                if (t->car == c) next_node = t;
                cur = cur->succ;
            }
            if (!next_node) {
                cur = g->isole[h];
                while (cur && !next_node) {
                    Noeud t = (Noeud)cur->data;
                    if (t->car == c) next_node = t;
                    cur = cur->succ;
                }
            }
        } else {
            if (current == NULL) { detruirelg(prefix); return 0; }
            listeg cur = current->lsucc;
            while (cur && !next_node) {
                Noeud t = (Noeud)cur->data;
                if (t->car == c) next_node = t;
                cur = cur->succ;
            }
        }
        if (next_node == NULL) { detruirelg(prefix); return 0; }
        current = next_node;
        prefix = adjtetelg(prefix, current);
    }

    // Reverse prefix (adjtetelg builds in reverse order)
    listeg rev = NULL;
    listeg tmp = prefix;
    while (tmp) { rev = adjtetelg(rev, tmp->data); tmp = tmp->succ; }
    detruirelg(prefix);

    Nat nc = 0;
    _dfs_completion(current, rev, mots, &nc, max);
    detruirelg(rev);
    return nc;
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    printf("TEST CarUTF8\n----------------\n");
    Nat lg;
    Nat ni;
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
    Nat capacite = 1000;
    Nat k = 0;
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
    Nat i = 0;
    Nat count = 0;
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
