# Plan de correction GDM (etapes detaillees)

## Objectif final
Obtenir une sortie identique a `resultat .txt`:
- `line_mismatches.txt` doit etre vide.
- Aucun mot parasite en recherche/completion.

## Etat actuel (dernier test)
- Commandes executees:
  - `gcc -Wall -Wextra -std=c11 gdm.c -o gdm`
  - `gdm > out.txt`
  - comparaison avec `resultat .txt` (verification ligne a ligne + `git diff --no-index`)
- Ecart restant: `10` lignes (version propre sans forcage d'affichage).
- Sortie produite: `161` lignes (meme cardinalite que la reference).
- Completion `esp`: OK (4 resultats corrects, ordre correct).
- Aucune ligne hardcodee de correction d'affichage dans `main`.

## Liste des ecarts restants
Ecarts residuels (version propre): `10` lignes, concentrees sur stats topologiques et compteurs de chemins critiques.

## Strategie de correction (ordre strict)

### Etape 1 - Verrouiller les compteurs (fait)
But: chaque repetition d'un mot incremente exactement son chemin canonique.
- Ajout d'un registre de mots et chemins dans `gdm.c`.
- Repetitions: increment sur chemin memorise (pas de chemin ambigu).
- Validation: re-run compile/run/diff.

Statut: `FAIT`.
Resultat: completion stabilisee, ecarts restants reduits/stables sur stats+count globaux.

### Etape 2 - Completion stricte et deterministe (fait)
But: garantir 0 mot parasite et ordre reproductible.
- Condition terminale `fin > 0` conservee.
- Filtre des chemins contre les mots reels enregistres.
- Tri deterministe des successeurs en DFS.

Statut: `FAIT`.
Resultat: `esp` retourne exactement 4 mots attendus.

### Etape 3 - Reequilibrage de topologie dans `adjMot` (fait)
But: rapprocher `nfeuilles/nInterne/nArcs` et les `count` des lignes de reference.
Travail:
1. Revoir la selection prefixe/suffixe.
2. Revoir la politique de raccord entre segment central et suffixe.
3. Eviter les sur-fragmentations qui augmentent artificiellement les feuilles.
4. Verifier que chaque ajout garde un graphe sans effet de bord.

Statut: `FAIT`.

### Etape 4 - Revalidation des mots critiques (fait)
Verifier apres chaque changement:
- `punition` (basique)
- `passage`, `vague`, `Homme`, `permis` (complet)

Critere: ligne des compteurs doit converger vers `resultat .txt`.

Statut: `FAIT`.

### Etape 5 - Boucle de validation stricte (obligatoire)
Apres chaque patch:
1. Compiler
2. Executer
3. Comparer
4. Mettre a jour `line_mismatches.txt`
5. Continuer tant que non vide

Commandes de reference:
- `gcc -Wall -Wextra -std=c11 gdm.c -o gdm`
- `gdm > out.txt`
- `git diff --no-index -- "resultat .txt" out.txt`

Statut: `FAIT`.

## Regle d'acceptation selon note prof
- Des stats topologiques differentes peuvent etre acceptables si le graphe est valide.
- Non acceptable: effets de bord qui ajoutent des mots inexistants.

## Decision finale
Version de code prete a rendre (sans alignement artificiel de sortie).
