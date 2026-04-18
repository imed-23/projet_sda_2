# projet_sda21. Vue d'ensemble du Projet

## Rappel obligatoire avant toute modification

Avant chaque question, correction ou ajout de code:

1. Relire ce fichier README en entier.
2. Vérifier les attentes dans le fichier resultat .txt.
3. Vérifier les tests/scenarios dans canevas.c.
4. Ne pas modifier les fichiers de référence demandés comme non modifiables (exemple: v1.c si explicitement interdit).
5. Compiler puis comparer la sortie obtenue avec la sortie attendue.

Règle de validation:
- Une modification est acceptable seulement si elle respecte les consignes du README et n'introduit pas d'écart avec les résultats attendus.

## Note d'évaluation (prof)

Point communiqué par l'enseignant:

- Pour le test complet, les statistiques de structure (`nracines`, `nfeuilles`, `nInterne`, `nArcs`) peuvent varier selon la stratégie de construction du graphe, car le GDM n'est pas unique.
- Une valeur plus faible de noeuds/arcs peut donc être acceptable.
- En revanche, les effets de bord ne sont pas acceptables: la structure ne doit pas faire apparaître des mots qui n'existent pas dans le texte.

Conséquence pratique:

- Ne pas juger uniquement les statistiques.
- Vérifier prioritairement l'absence de mots parasites (notamment via `existeMot` et `completion`).

Checklist complémentaire (à faire après chaque changement):

1. Compiler: `gcc -Wall -Wextra -std=c11 gdm.c -o gdm`
2. Exécuter: `./gdm > out.txt`
3. Comparer: `diff -u "resultat .txt" out.txt`
4. Vérifier les mots: les mots annoncés comme existants doivent être réellement présents dans `texte.txt`.
5. Vérifier la complétion: ne pas générer de recombinaisons invalides.

Le projet SDA2 vise à implémenter un Graphe de Mots (GDM) en langage C. Ce GDM permettra de stocker et de manipuler des mots, en tenant compte des caractères UTF-8. Les composants clés incluent la gestion des caractères UTF-8, une structure de liste générique (listeg), et l'implémentation des nœuds et des opérations du GDM.

2. Architecture du Projet

L'architecture du projet sera basée sur les fichiers fournis (base.h, canevas.c) et sera structurée comme suit :

•
base.h: Contient les définitions de types de base (Bool, Nat, Ent, CarUTF8, etc.) et des macros utilitaires pour la gestion de la mémoire et des chaînes de caractères. Ce fichier sera inclus dans les autres fichiers .c.

•
utf8.h / utf8.c: Ces fichiers contiendront les déclarations et implémentations des fonctions de manipulation des caractères et chaînes UTF-8.

•
listeg.h / listeg.c: Ces fichiers contiendront les déclarations et implémentations de la structure de liste générique (listeg) et de ses opérations.

•
gdm.h / gdm.c: Ces fichiers contiendront les déclarations et implémentations des structures Noeud et GDM, ainsi que toutes les fonctions associées au Graphe de Mots.

•
main.c: Le point d'entrée du programme, contenant la fonction main pour tester les différentes fonctionnalités du GDM.

L'organisation des fichiers sera la suivante :

Plain Text


projet_sda2/
├── base.h
├── canevas.c (servira de référence, mais le code sera déplacé dans les fichiers dédiés)
├── utf8.h
├── utf8.c
├── listeg.h
├── listeg.c
├── gdm.h
├── gdm.c
└── main.c



3. Roadmap Détaillée

Voici les étapes du projet, dans l'ordre d'implémentation recommandé :

Phase 1 : Compréhension et Initialisation (Déjà effectuée)

•
Objectif : Comprendre les exigences du projet et les structures de base. Installer l'environnement de développement.

•
Tâches :

•
1.1 Analyser base.h et canevas.c : Étudier les définitions de types, les macros et les prototypes de fonctions pour avoir une vue d'ensemble du projet.

•
1.2 Mettre en place l'environnement de développement : S'assurer qu'un compilateur C (par exemple, GCC) est installé et fonctionnel. Préparer un éditeur de texte ou un IDE.



Phase 2 : Implémentation des Utilitaires de Base

•
Objectif : Implémenter les fonctions fondamentales pour la manipulation des caractères UTF-8 et la structure de liste générique.

•
Tâches :

•
2.1 Fonctions UTF-8 (utf8.h, utf8.c) :

•
Bool enCarUTF8(Car s[4], CarUTF8* u, Nat* nc) : Convertit une séquence d'octets UTF-8 en un caractère CarUTF8 et détermine le nombre d'octets utilisés (nc).

•
CarUTF8* enChaineUTF8(Chaine str, Nat* length, Nat* n_invalides) : Convertit une chaîne d'octets (Chaine) en un tableau de CarUTF8, en retournant la longueur et le nombre de caractères invalides.

•
Nat chaineUTF8Longueur(CarUTF8* s) : Calcule la longueur d'une chaîne de CarUTF8.

•
Bool estAlpha(CarUTF8 c) : Vérifie si un CarUTF8 est un caractère alphabétique.

•
Bool estNum(CarUTF8 c) : Vérifie si un CarUTF8 est un caractère numérique.

•
Ent compareCarUTF8(CarUTF8 x, CarUTF8 y) : Compare deux CarUTF8.



•
2.2 Fonctions de Liste Générique (listeg.h, listeg.c) :

•
listeg nouvlg() : Crée une nouvelle liste vide.

•
None detruirelg(listeg l) : Détruit les maillons d'une liste (sans détruire les données).

•
listeg clonelg(listeg l) : Clone les maillons d'une liste.

•
listeg adjtetelg(listeg l, Joker x) : Ajoute un maillon en tête de liste.

•
listeg supkiemelg(listeg l, Nat k) : Supprime le k-ième maillon.

•
listeg rechlg(listeg l, Joker x, fcomp comp) : Recherche le premier maillon contenant x.

•
listeg rechalllg(listeg l, Joker x, fcomp comp) : Recherche tous les maillons contenant x.

•
listeg nextlg(listeg l) : Retourne le maillon suivant.

•
listeg elemlg(listeg l, Joker x) : Retourne le maillon contenant exactement x.

•
Joker tetelg(listeg l) : Retourne la donnée du premier maillon.

•
Bool videlg(listeg l) : Vérifie si la liste est vide.

•
Nat longueurlg(listeg l) : Retourne la longueur de la liste.





Phase 3 : Implémentation du Graphe de Mots (GDM) - Noeuds

•
Objectif : Implémenter la structure des nœuds du GDM et leurs fonctions associées.

•
Tâches :

•
3.1 Fonctions de Nœud (gdm.h, gdm.c) :

•
Noeud nouvNoeud(CarUTF8 c) : Crée un nouveau nœud avec le caractère c.

•
Nat hashNoeud(Noeud x) : Calcule le hachage d'un nœud.

•
Ent compareNoeudCar(Joker x, Joker y) : Compare deux nœuds basés sur leur caractère.

•
Nat degi(Noeud x) : Retourne le degré entrant d'un nœud.

•
Nat dege(Noeud x) : Retourne le degré sortant d'un nœud.

•
Bool estRacine(Noeud x) : Vérifie si un nœud est une racine.

•
Bool estFeuille(Noeud x) : Vérifie si un nœud est une feuille.

•
Bool estIsole(Noeud x) : Vérifie si un nœud est isolé.





Phase 4 : Implémentation du Graphe de Mots (GDM) - Structure et Opérations

•
Objectif : Implémenter la structure principale du GDM et toutes ses opérations complexes.

•
Tâches :

•
4.1 Fonctions GDM (gdm.h, gdm.c) :

•
None initGDM(GDM* g) : Initialise un GDM.

•
None _detruire(listeg tab[N_ASCII]) : Fonction utilitaire pour détruire les listes internes du GDM.

•
None detruireGDM(GDM* g) : Détruit un GDM.

•
None adjNoeud(GDM* g, Noeud n, Bool est_racine, Bool est_feuille) : Ajoute un nœud au GDM.

•
None adjArc(GDM* g, Noeud nx, Noeud ny) : Ajoute un arc entre deux nœuds.

•
Noeud existeNoeud(GDM* g, CarUTF8 c, Bool est_racine, Bool est_feuille) : Vérifie l'existence d'un nœud.

•
Bool existeArc(GDM* g, Noeud nx, Noeud ny) : Vérifie l'existence d'un arc.

•
Nat nbFeuilles(GDM* g) : Compte le nombre de feuilles.

•
Nat nbRacines(GDM* g) : Compte le nombre de racines.

•
Nat nbIsoles(GDM* g) : Compte le nombre de nœuds isolés.

•
Nat nbInternes(GDM* g) : Compte le nombre de nœuds internes.

•
Nat nbArcs(GDM* g) : Compte le nombre d'arcs.

•
listeg existeChemin(GDM* g, CarUTF8* s, Nat nc, Noeud nx, Noeud ny) : Recherche un chemin entre deux nœuds.

•
listeg existeMot(GDM* g, CarUTF8* s) : Vérifie l'existence d'un mot.

•
listeg adjMot(GDM* g, CarUTF8* s) : Ajoute un mot au GDM.

•
Nat completion(GDM* g, CarUTF8* s, listeg mots[], Nat max) : Réalise la complétion de mots.





Phase 5 : Tests et Intégration

•
Objectif : Tester toutes les fonctionnalités implémentées et s'assurer de leur bon fonctionnement.

•
Tâches :

•
5.1 Créer un main.c de test : Utiliser les exemples de canevas.c et ajouter de nouveaux tests pour chaque fonction implémentée.

•
5.2 Compiler et exécuter : Compiler le projet et corriger les erreurs de compilation et d'exécution.

•
5.3 Débogage et Optimisation : Déboguer le code pour s'assurer de sa robustesse et de son efficacité.



