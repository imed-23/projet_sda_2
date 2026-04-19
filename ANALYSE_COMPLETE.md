# ANALYSE COMPLÈTE DU CODE GDM.C

## Date: 2026-04-01

---

## 1. RÉSUMÉ EXÉCUTIF

Ce document présente une analyse complète du programme `gdm.c` (Graphe de Mots), incluant:
- Comparaison des résultats attendus vs obtenus
- Analyse du code source
- Problèmes identifiés
- Recommandations d'amélioration

---

## 2. COMPARAISON DES RÉSULTATS

### 2.1 Différences Principales

#### A. TEST BASIQUE (mots de test)

**Résultats attendus (`resultat .txt`):**
- Racines: 7
- Feuilles: 3
- Internes: 24
- Arcs: 39

**Résultats obtenus (`resultat_nouveau.txt`):**
- Racines: 5
- Feuilles: 11
- Internes: 60
- Arcs: 71

**Écart:** Le nouveau code produit PLUS de nœuds et arcs. Cela indique un changement dans l'algorithme de construction du graphe.

#### B. Recherche de mots (TEST BASIQUE)

**Mot "punition":**
- Attendu: `'p'0x70(4) 'u'0x75(2) 'n'0x6e(2) 'i'0x69(1) 't'0x74(9) 'i'0x69(10) 'o'0x6f(10) 'n'0x6e(10)`
- Obtenu: `'n'0x6e(1) 'o'0x6f(1) 'i'0x69(1) 't'0x74(1) 'i'0x69(3) 'n'0x6e(4) 'u'0x75(3) 'p'0x70(4)`

**PROBLÈME CRITIQUE:** L'ordre des caractères est INVERSÉ dans le résultat nouveau!
Cela suggère que la fonction `existeMot()` retourne le chemin dans le mauvais ordre.

#### C. TEST COMPLET (fichier texte)

**Différences de lecture:**
- Attendu: 30769 octets, 29292 caractères, 1241 invalides
- Obtenu: 30768 octets, 29291 caractères, 364 invalides

**Écart:** Lecture légèrement différente du fichier (1 octet, 1 caractère de moins, beaucoup moins de caractères invalides).

**Statistiques du graphe:**
- Attendu: Racines: 56, Feuilles: 46, Internes: 2361, Arcs: 3860
- Obtenu: Racines: 31, Feuilles: 3340, Internes: 10880, Arcs: 14220

**PROBLÈME:** Structure du graphe complètement différente! Beaucoup plus de feuilles et d'arcs.

#### D. Recherche de mots (TEST COMPLET)

**"passage":**
- Attendu: `'p'0x70(363) 'a'0x61(62) 's'0x73(3) 's'0x73(2) 'a'0x61(6) 'g'0x67(12) 'e'0x65(1225)`
- Obtenu: `'e'0x65(1) 'g'0x67(1) 'a'0x61(1) 's'0x73(1) 's'0x73(1) 'a'0x61(2) 'p'0x70(2)`

**PROBLÈME:** Ordre inversé ET compteurs complètement différents!

**"Homme":**
- Attendu: `'H'0x48(23) 'o'0x6f(28) 'm'0x6d(29) 'm'0x6d(77) 'e'0x65(1225)`
- Obtenu: `'e'0x65(19) 'm'0x6d(19) 'm'0x6d(19) 'o'0x6f(19) 'H'0x48(20)`

**PROBLÈME:** Même problème - ordre inversé!

#### E. TEST COMPLETION

**Attendu:** 4 mots de complétion pour "esp"
**Obtenu:** AUCUN mot de complétion (vide)

**PROBLÈME CRITIQUE:** La fonction `completion()` ne fonctionne pas du tout!

---

## 3. ANALYSE DU CODE SOURCE

### 3.1 Structure Générale

Le programme implémente un graphe de mots orienté avec les composants suivants:
- **CarUTF8**: Support UTF-8 (caractères sur 2 octets)
- **listeg**: Liste générique chaînée
- **Noeud**: Nœud du graphe avec caractère, compteur, successeurs et prédécesseurs
- **GDM**: Graphe de mots avec 4 catégories de nœuds (racines, feuilles, isolés, internes)

### 3.2 Fonctions Clés

#### A. `adjMot()` - Ajout de mot (lignes 382-441)

**Fonctionnement:**
1. Parcourt chaque caractère du mot
2. Pour le premier caractère, cherche dans TOUS les nœuds
3. Pour les suivants, cherche uniquement dans les successeurs du nœud précédent
4. Crée un nouveau nœud si non trouvé
5. Ajoute les arcs entre nœuds consécutifs
6. Incrémente le compteur du nœud

**Problèmes potentiels:**
- Complexité élevée pour la recherche du premier caractère (O(n))
- Mise à jour de catégories après chaque arc ajouté (coûteux)

#### B. `existeMot()` - Recherche de mot (lignes 447-513)

**PROBLÈME IDENTIFIÉ:**
```c
// Ligne 496-502: Inversion de la liste
listeg reversed = NULL;
listeg tmp = result;
while (tmp) {
    reversed = adjtetelg(reversed, tmp->data);
    tmp = tmp->succ;
}
detruirelg(result);
```

Cette inversion est faite, MAIS le chemin est déjà dans le bon ordre quand il est construit!
La fonction `_existeMot_rec` construit le chemin de gauche à droite en ajoutant les nœuds.

**BUG:** L'inversion est incorrecte - elle inverse le chemin qui était déjà correct!

#### C. `completion()` - Auto-complétion (lignes 521-591)

**Analyse:**
La fonction traverse le graphe jusqu'au dernier caractère du préfixe, puis explore tous les chemins possibles via `_dfs_completion`.

**Problème possible:** La fonction ne retourne rien dans le nouveau résultat. Cela peut être dû à:
1. Le préfixe n'est pas trouvé
2. Aucun nœud avec count > 0 n'est trouvé
3. Problème dans la construction du graphe

### 3.3 Gestion UTF-8

**Code (lignes 56-120):**
- Conversion correcte des caractères UTF-8 sur 2 octets
- Filtrage des caractères invalides (notamment le symbole €)
- Support des caractères accentués français (é, è, à, etc.)

**Observation:** La différence dans le nombre de caractères invalides (1241 vs 364) suggère que le code a été modifié pour mieux gérer UTF-8.

---

## 4. PROBLÈMES IDENTIFIÉS

### 4.1 Critiques (Bloquants)

1. **BUG MAJEUR - Ordre inversé dans `existeMot()`**
   - Localisation: Lignes 496-502
   - Impact: Tous les résultats de recherche sont inversés
   - Cause: Inversion inutile du chemin

2. **BUG - Completion ne fonctionne pas**
   - Localisation: Fonction `completion()` et `_dfs_completion()`
   - Impact: Aucune complétion n'est retournée
   - Cause probable: Problème de construction du graphe ou de parcours DFS

3. **INCOHÉRENCE - Structure du graphe différente**
   - Impact: Nombre de racines/feuilles/arcs complètement différent
   - Cause probable: Changement dans l'algorithme de construction (`adjMot()` ou `_updateNodeCategory()`)

### 4.2 Importants (Fonctionnels)

4. **Compteurs de nœuds incorrects**
   - Les compteurs (count) sont beaucoup plus bas dans le nouveau résultat
   - Exemple: 'p' devrait avoir count=363, a count=2
   - Cause possible: Les nœuds ne sont pas réutilisés correctement

5. **Catégorisation des nœuds**
   - Le nouveau code a beaucoup plus de feuilles (3340 vs 46)
   - Cause: Probablement `_updateNodeCategory()` ne fonctionne pas correctement

### 4.3 Mineurs (Qualité)

6. **Performance de recherche**
   - Recherche linéaire dans tous les nœuds pour le premier caractère
   - Amélioration possible: Table de hachage améliorée

7. **Gestion mémoire**
   - Pas de vérification systématique des allocations
   - Risque de fuite si allocation échoue

---

## 5. CORRECTIONS RECOMMANDÉES

### 5.1 Correction Urgente - Bug d'inversion

**Dans `existeMot()` (lignes 496-502):**

```c
// AVANT (incorrect):
listeg reversed = NULL;
listeg tmp = result;
while (tmp) {
    reversed = adjtetelg(reversed, tmp->data);
    tmp = tmp->succ;
}
detruirelg(result);
// Utilise reversed

// APRÈS (correct):
// NE PAS inverser - result est déjà dans le bon ordre!
// OU vérifier la construction dans _existeMot_rec
```

**Solution:**
- Option 1: Supprimer l'inversion
- Option 2: Inverser la construction dans `_existeMot_rec` si l'inversion est nécessaire

### 5.2 Correction - Fonction adjMot

**Problème:** Les nœuds ne sont pas correctement réutilisés entre différents mots.

**Analyse requise:**
- Vérifier que `existeNoeud()` cherche dans TOUTES les catégories
- S'assurer que les arcs ne créent pas de doublons de nœuds

### 5.3 Correction - Fonction completion

**Actions:**
1. Ajouter des printf de debug pour tracer:
   - Si le préfixe est trouvé
   - Combien de nœuds successeurs sont explorés
   - Combien de mots avec count > 0 sont trouvés

2. Vérifier que les nœuds terminaux ont bien count > 0

### 5.4 Amélioration - Structure de données

**Recommandation:** Utiliser une vraie table de hachage au lieu de N_ASCII buckets

```c
// Au lieu de:
listeg racines[N_ASCII];  // 128 buckets

// Utiliser:
listeg racines[256];      // Plus de buckets pour moins de collisions
// OU implémenter une vraie table de hachage avec fonction de hachage sur 2 octets UTF-8
```

### 5.5 Amélioration - Gestion des erreurs

```c
// Ajouter des vérifications:
if (malloc_result == NULL) {
    fprintf(stderr, "Erreur allocation mémoire\n");
    return NULL;  // ou exit(1)
}
```

---

## 6. TESTS RECOMMANDÉS

### 6.1 Tests unitaires à ajouter

1. **Test UTF-8:**
   - Vérifier conversion des caractères accentués
   - Tester les caractères invalides
   - Vérifier les caractères multi-octets

2. **Test adjMot:**
   - Ajouter un mot simple: "test"
   - Vérifier le nombre de nœuds créés (doit être 4)
   - Vérifier le nombre d'arcs (doit être 3)
   - Ajouter un mot partageant un préfixe: "tesla"
   - Vérifier la réutilisation des nœuds communs

3. **Test existeMot:**
   - Ajouter "test"
   - Chercher "test" - doit retourner le chemin dans l'ordre
   - Vérifier chaque caractère: 't', 'e', 's', 't'

4. **Test completion:**
   - Ajouter: "test", "tesla", "terrain"
   - Compléter "te" - doit retourner 3 mots
   - Compléter "tes" - doit retourner 2 mots

### 6.2 Tests de performance

1. **Test grande échelle:**
   - Charger 10000 mots
   - Mesurer le temps d'ajout
   - Mesurer le temps de recherche
   - Mesurer l'utilisation mémoire

---

## 7. ARCHITECTURE ET DESIGN

### 7.1 Points forts

1. **Séparation des préoccupations:** UTF-8, listes génériques, nœuds, graphe
2. **Support UTF-8:** Bien implémenté pour le français
3. **Structure de données:** Graphe orienté avec catégorisation des nœuds

### 7.2 Points faibles

1. **Complexité:** Gestion de 4 catégories de nœuds complique le code
2. **Performance:** Recherche linéaire dans les listes
3. **Maintenabilité:** Fonctions longues (main fait 200 lignes)

### 7.3 Recommandations architecturales

1. **Séparer les tests dans un fichier séparé**
   - Créer `test_gdm.c`
   - Garder `gdm.c` pour l'implémentation
   - Créer `gdm.h` pour les déclarations

2. **Simplifier la catégorisation**
   - Calculer dynamiquement si un nœud est racine/feuille
   - Éviter de maintenir 4 listes séparées

3. **Améliorer les structures de données**
   - Utiliser un trie (arbre préfixe) au lieu d'un graphe général
   - Cela simplifierait beaucoup le code

---

## 8. QUALITÉ DU CODE

### 8.1 Style et lisibilité

**Points positifs:**
- Nommage clair des fonctions
- Commentaires de séparation
- Utilisation de typedefs

**Points à améliorer:**
- Fonctions trop longues (main, adjMot)
- Manque de commentaires explicatifs
- Variables avec noms courts (nx, ny, n, m)

### 8.2 Bonnes pratiques

**Respectées:**
- Libération de la mémoire
- Vérification des pointeurs NULL dans certains cas

**Non respectées:**
- Pas de vérification systématique des malloc
- Pas de const pour les paramètres en lecture seule
- Pas de documentation des fonctions

---

## 9. CONCLUSION

### 9.1 Résumé des problèmes

Le code `gdm.c` présente **3 bugs critiques:**

1. **Ordre inversé dans existeMot()** - URGENT
2. **Completion ne fonctionne pas** - URGENT
3. **Structure du graphe incorrecte** - IMPORTANT

Et **plusieurs points d'amélioration** sur la performance, la qualité et la maintenabilité.

### 9.2 Priorités de correction

**Priorité 1 (Critique - à faire immédiatement):**
1. Corriger l'inversion dans `existeMot()`
2. Débugger `completion()`
3. Corriger la construction du graphe dans `adjMot()`

**Priorité 2 (Important - à faire rapidement):**
4. Corriger les compteurs de nœuds
5. Améliorer la catégorisation des nœuds
6. Ajouter des tests unitaires

**Priorité 3 (Amélioration - moyen terme):**
7. Optimiser les performances
8. Améliorer la gestion des erreurs
9. Refactoriser le code pour la maintenabilité

### 9.3 Estimation du travail

- **Corrections critiques:** 2-4 heures
- **Tests et validation:** 2-3 heures
- **Améliorations:** 5-10 heures
- **Refactoring complet:** 15-20 heures

---

## 10. ANNEXES

### 10.1 Commandes utilisées

```bash
# Compilation
gcc -o gdm gdm.c -lm -Wall

# Exécution
./gdm > resultat_nouveau.txt

# Comparaison
diff -u "resultat .txt" resultat_nouveau.txt
```

### 10.2 Environnement de test

- OS: Linux
- Compilateur: GCC
- Fichier de test: texte_pour_test.txt (30768 octets)
- Encodage: UTF-8

### 10.3 Fichiers générés

- `resultat_nouveau.txt`: Résultat de l'exécution actuelle
- `ANALYSE_COMPLETE.md`: Ce document

---

**Fin du rapport d'analyse**
