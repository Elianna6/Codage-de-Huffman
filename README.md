typedef struct noeud{
  char symbole;
  uint64_t freq;
};


uint64_t freq: nous avons décidé de choisir un int en 64 bits parce que le fichier texte donné est long donc le poids des caractères va être assez élevé.

Pour construire l'arbre sans parcourir notre structure à chaque fois pour repérer les deux caractères ayant les poids minimum, nous avons conclu qu’utiliser une pile est la solution la plus appropriée. De cette manière, nous avons les éléments de poids le plus faible au-dessus de la pile et il nous suffit de réarranger dans la pile le nouveau nœud donné par la somme des deux nœuds. 
Si nous avons besoin de pile, nous avons besoin de créer une structure maillon pour lier les nœuds de la pile les uns aux autres. 
Ainsi, nous ajoutons deux structures : maillon et pile 

typedef struct maillon{
  noeud* donnee;
  struct maillon* suiv;
}maillon;


typedef struct pile{
  uint16_t taille;
  maillon* tete;
}pile;


Il faut maintenant construire l’arbre. Pour cela, il nous faut créer une structure arbre. Pour reconstituer l’arbre, il nous suffit d’avoir son nœud racine. Ainsi, on définit la structure :

typedef struct arbre{
  noeud* racine;
}arbre;


Pour chaque nœud de l’arbre, nous avons besoin de connaître son fils droit ainsi que son fils gauche. Une fois l’arbre construit, nous devons stocker le code correspondant à nos caractères dans une chaîne de caractères. Notre structure noeud devient alors :

typedef struct noeud{
  char symbole;
  uint64_t freq;
  char* code;
  struct noeud* droit;
  struct noeud* gauche;
}noeud;
Fonctions d’initialisation de nos structures de données

Pour pouvoir utiliser nos structures de données, nous devons déjà commencer par les initialiser.

Pour pouvoir initialiser nos nœuds, nous nous sommes inspirés des fonctions d’initialisation des maillons vues en cours. En y ajoutant les données qui nous sont nécessaires.

noeud* new_noeud(){
  noeud* n = malloc(sizeof(noeud));
  if (n==NULL){
    assert(0);
  }
  n->symbole = '\0';
  n->code = NULL;
  n->freq = 0;
  n->droit = NULL;
  n->gauche = NULL;
  return n;
}


noeud* n = malloc(sizeof(noeud)): nous permet d’allouer un espace mémoire dédié au stockage d’un nœud. On vérifie après cette allocation si l’adresse renvoyée est NULL, afin de prévenir les possibles saturations de mémoire.

De même, pour l’initialisation des maillons, nous avons repris la fonction permettant d’initialiser les maillons d’une pile vue en cours. Sauf qu'ici on suppose qu’on ne peut pas créer de maillon qui ne contient aucun nœud.

maillon* new_maillon(noeud* n){
  maillon* m = malloc(sizeof(maillon));
  if (m==NULL){
    assert(0);
  }
  m->donnee = n;
  m->suiv = NULL;
  return m;
}




Pour initialiser la pile, nous avons simplement directement repris la fonction d’initialisation d’une pile vue en cours.

pile* new_pile(){
  pile* p = malloc(sizeof(pile));
  if (p==NULL){
    assert(0);
  }
  p->taille = 0;
  p->tete = NULL;
  return p;
}


Et finalement, pour la structure arbre, nous nous sommes inspirés des fonctions d’initialisation déjà faites.

arbre* new_arbre(){
  arbre* a = malloc(sizeof(arbre));
  if (a==NULL){
    assert(0);
  }
  a->racine = NULL;
  return a;
}





Fonctions relatives à la structure noeud

Dans cette partie, nous allons brièvement expliquer les fonctions relatives à la structure de données nœud.

uint8_t noeud_estVide(noeud*);
uint8_t est_feuille(noeud*);


uint8_t noeud_estVide(noeud*): Cette fonction nous permet de savoir si un nœud contient un caractère. Elle renvoie 1 si elle en contient, sinon elle renvoie 0.

uint8_t est_feuille(noeud*): Cette fonction nous permet de savoir si un nœud possède des nœuds fils. Elle renvoie 0 si elle en possède, sinon elle renvoie 1.




Fonctions relatives à la structure pile

Dans cette partie, nous allons brièvement expliquer certaines fonctions relatives à la structure de données pile.

uint8_t pile_estVide(pile*);
void empiler(pile*, noeud*);
noeud* depiler(pile*);
void ranger_pile(pile*, noeud*);


uint8_t pile_estVide(pile*): Cette fonction permet de vérifier si la pile est vide. Elle renvoie 1 si elle est vide, sinon elle renvoie 0.

void empiler(pile*, noeud*): Cette fonction permet d’ajouter un maillon à la tête d’une pile. Elle prend en argument un nœud qui représente les données d’un maillon.

noeud* depiler(pile*): Cette fonction permet de retirer le maillon en tête d’une pile. Elle renvoie un nœud qui représente les données du maillon dépilé.

Afin d’avoir une pile rangée dans l’ordre croissant des poids (le plus petit poids en tête, le plus grand en queue) nous avons écrit la fonction ranger_pile, qui rangera un nœud dans la pile. 

Pour cela, nous allons supposer que la pile est rangée dans l’ordre croissant et que nous souhaitons ajouter un nœud n de poids quelconque. On compare le poids du nœud n avec le poids du nœud à la tête de la pile:

	


Si le poids de n est plus petit que le poids du nœud en tête (ou si la pile est vide), alors on met en tête le nœud n.





Sinon, on dépile la tête et on recommence la comparaison. On remplit progressivement les éléments dépilés quand on aura empilé le nœud n.






Pour pouvoir retranscrire cela, nous avons écrit la fonction ranger_pile de façon récursive afin de ne pas perdre les données qui ont été dépilées de la pile.

void ranger_pile(pile* p, noeud* n){
  if (pile_estVide(p) || p->tete->donnee->freq >= n->freq){
    empiler(p, n);
  }
  else{
    noeud* e = depiler(p);
    ranger_pile(p, n);
    empiler(p, e);
  }
}

Fonctions relatives à la structure arbre

L’arbre est à présent initialisé, les caractères sont aussi triés selon leur poids dans la pile. Il faut maintenant construire l’arbre propre à cette dernière. 
-> Pour ce faire, nous utilisons la fonction :
arbre* creer_arbre(pile*);                                           


Cette fonction somme les deux nœuds de poids les plus faibles à chaque fois puis réarrange la somme des deux dans la pile. A la fin de la fonction, la pile contiendra donc un nœud qui correspondra à la racine de notre arbre.

-> Pour fusionner deux noeuds, nous utilisons la fonction : 
noeud* fusion(noeud*, noeud*);

Cette fonction crée un nouveau nœud qui a pour fils gauche et droite les deux nœuds qu’on lui donne en paramètre. Son poids correspond à la somme des poids de ses fils.

Une fois que deux nœuds sont fusionnés, il faut réarranger leur parent dans la pile pour que la pile soit toujours triée afin de faciliter la fusion suivante. 

-> Pour réarranger la pile, nous utilisons la fonction :
void ranger_pile(pile*, noeud*);

Remarque: 
Dans la fonction arbre* creer_arbre(pile*);  , à chaque fois que nous rencontrons une feuille, on la stocke dans une pile p2. Cette nouvelle pile contiendra donc toutes les feuilles, c'est-à-dire les nœuds correspondant à nos caractères. Le contenu de p2 est ensuite déversé dans la pile initiale. Utiliser p2 permet d’optimiser le code parce que si nous avons nos feuilles dans la pile, nous avons accès à leur code directement sans parcourir l’arbre à chaque fois que nous avons besoin de connaître le code d’un caractère. 



if (est_feuille(n1)){
      ranger_pile(p2, n1);
      }
      if (est_feuille(n2)){
      ranger_pile(p2, n2);
 }
       .
       .
       .
while (!pile_estVide(p2)){
    empiler(p, depiler(p2));
  }


La pile p2 allouée avec malloc est donc utilisée comme une pile temporaire pour stocker nos feuilles. Une fois nos feuilles remises dans la pile initiale, on peut free(p2);  pour éviter de gaspiller de la place dans la mémoire.
 
Il faut maintenant définir le code de chaque caractère de l’arbre. Pour ce faire, on utilise la fonction :

void definir_code(arbre*);


Cette fonction appelle une autre fonction void parcourir_arbre(noeud*, char*); qui assigne un code à un caractère, en marquant “0” quand on va vers la gauche et “1” quand on va vers la gauche.

void parcourir_arbre(noeud* n, char* c){

Cette fonction initialise le tableau de char nommé code dans un nœud lorsque ce  dernier est une feuille. De cette manière, les codes des feuilles que l’on a remis dans la pile comme on l’a expliqué ci-dessus seront directement accessibles via la pile sans parcourir tout l’arbre. 

Pour mémoriser le chemin parcouru, on initialise deux chaînes de caractères c1 et c2. c1 pour le sous-arbre gauche et c2 pour le sous-arbre droit. 

    strcpy(c1, c);
    strcpy(c2, c);     
    strcat(c1, "0");
    strcat(c2, "1");
    parcourir_arbre(n->gauche, c1);
    parcourir_arbre(n->droit, c2);


Une fois nos caractères encodés, nous pouvons free l’arbre pour libérer la mémoire avec la fonction : 

void arbre_free(noeud*);


Cette fonction prend en paramètre la racine de l’arbre et libère de la mémoire tous les nœuds contenus dans cet arbre via un parcours en profondeur. Après l'exécution de cette fonction, il est nécessaire de free la structure arbre.




Fonctions relatives à l’algorithme de Huffman

void chercherdans_pile(pile* , char );


Cette fonction cherche dans la pile si le caractère donné en paramètre est déjà présent dans la pile ou pas encore. Si c’est le cas, le poids du nœud correspondant est incrémenté sinon on crée un nouveau nœud que l’on insère dans la pile. On trie la pile à chaque nœud ajouté.

La fonction ci-dessous lit chaque caractère du fichier à compresser et le place dans la pile.
pile* lire_fichier(FILE* f){
  char a;
  pile* p = new_pile();
  while (!feof(f)){
    fscanf(f, "%c", &a);
    chercherdans_pile(p, a);
  }
  return p;
}



La fonction noeud* codage_noeud(pile*, char); cherche dans la pile le nœud correspondant au caractère passé en paramètre.  
noeud* codage_noeud(pile*, char);

 
Pour finir nous utilisons la fonction void compression(FILE*, pile*, char*);, qui prend en paramètre le fichier d'entrée, la pile qui contient nos caractères encodés et le format du fichier, afin de compresser le fichier donné en entrée. Cette fonction créera un fichier compressé qu’on nommera mieuxquewinrar.txt.

void compression(FILE* f, pile* p, char* type){
  FILE* sortie = fopen("mieuxquewinrar.txt", "wb");
       .
       .
       .
  fprintf(sortie, "%s %d\n", type, p->taille);
  while(!pile_estVide(p)){
    n = depiler(p);
    fprintf(sortie, "/%c %s /", n->symbole, n->code);
    empiler(q, n);
  }
       .
       .
       .
  while (!feof(f)){
    b = 0;
    for(i=0; i<8; i++){
      if (j>=strlen(code) && !feof(f)){
            fscanf(f, "%c", &a);
            n = codage_noeud(p, a);
            j = 0;
            strcpy(code, n->code);
      }
      else if (j>=strlen(code) && feof(f)){
            code[j] = '0';
      }
      b += (uint8_t)(code[j]-'0') * pow(2, 7-i);
      j++;
    }
    fprintf(sortie, "%c", b);
  }
}


Dans un premier temps, nous ouvrons (ou créons) le fichier mieuxquewinrar.txt via la fonction fopen("mieuxquewinrar.txt", "wb");. Puis nous écrivons en entête le format du fichier d'entrée et le nombre de caractères différent que contient le fichier. Puis nous écrivons les caractères et leurs codes entre deux ‘/’ afin de bien détecter tous les éléments lors de la décompression.
Pour pouvoir compresser un fichier, nous devons réduire le nombre de caractères. On sait qu’en ASCII chaque caractère est codé sur un octet (8 bits), nous allons donc récupérer le code de chaque caractère qu’on lit dans le fichier, et les écrire sur un octet.
Si nous pouvons écrire les codes d’un ou plusieurs caractères sur un octet, alors on écrit cet octet dans le fichier de sortie.

Si les codes dépasse la taille d’un octet, alors nous sauvegardons là où nous nous sommes arrêtés dans la lecture du code et nous écrivons l’octet obtenu avant le dépassement.
Et si les codes sont moins d’un octet, nous lisons le prochain caractère et nous recommençons les vérifications. Si on arrive à la fin du fichier et que nos codes sont plus petits qu’un octet, alors nous remplissons le code avec des “0” jusqu'à obtenir un octet.

Pour pouvoir écrire un octet, nous récupérons cet octet sous forme de uin8_t via la ligne b += (uint8_t)(code[j]-'0') * pow(2, 7-i);. Et nous l'écrivons dans le fichier de sortie via la ligne fprintf(sortie, "%c", b);. Sachant que b est codé sur un octet, nous pouvons l'écrire sous forme de caractère dans le fichier de sortie.

Fonction main de compresser.c

Nous construisons la fonction main de manière à récupérer le fichier donné en entrée lors de l'exécution du programme. Nous récupérons aussi le format du fichier et nous commençons à construire la pile et l’arbre afin d'exécuter le codage de Huffman. Et nous finissons par libérer la mémoire.

int main(int argc, char* argv[]){
  FILE* f = fopen(argv[1], "rb");
  char* type =  argv[1] + strlen(argv[1])-3;
  if (f==NULL){
    assert(0);
  }

  pile* p = lire_fichier(f);
  arbre* a = creer_arbre(p);
  definir_code(a);
  compression(f, p, type);

  //Libération mémoire
  free(p);
  arbre_free(a->racine);
  free(a);
  fclose(f);
  return EXIT_SUCCESS;
}


Remarque:
Pour récupérer le format du fichier nous utilisons la ligne suivante: 
char* type =  argv[1] + strlen(argv[1])-3;. Cela nous permet de récupérer les trois derniers caractères du nom du fichier. Or il existe certains formats comme les jpeg, html… qui possède plus de trois caractères pour définir leurs formats. Nous avons décidé de récupérer seulement les trois derniers caractères car la plupart des formats sont définis de cette façon. Nous aurions pu écrire une fonction qui fait en sorte de récupérer tous les caractères après le point, ou bien, nous aurions pu directement récupérer le nom du fichier afin de renvoyer un fichier du même nom lors de la décompression.


Décompression

Pour la décompression, nous utilisons les mêmes structures. Pour décompresser, il suffit de refaire les étapes de compression dans le sens inverse.

Tout d’abord, nous devons lire les données fournies en tête du fichier compressé et créer une pile contenant ces nœuds.


Cette tâche est assurée par la fonction :
arbre* init_arbre(FILE*, uint32_t);


Nous avons rencontré des problèmes à cette étape lors de la lecture du fichier. On n’arrivait pas à lire les “\n” et “ “ parce que pour séparer nos éléments, on les a séparés par un espace. Ainsi, le programme n'arrivait pas à savoir si on faisait référence à l’élément suivant ou si “ “  correspond à un caractère. 
On a donc construit nos en-tête de fichier de sorte à ce qu’un nouveau caractère est marqué par un “/” au début et à la fin pour dire que l’on passe à l’élément suivant comme décrit un peu plus haut dans le rapport. De cette manière, on évite le conflit lors de la lecture du fichier. 

Init_arbre fait appel à arbre* creer_arbre(pile*); .

arbre* creer_arbre(pile*);

L’objectif de cette fonction est de reconstruire l’arbre sans recalculer les fréquences mais juste en utilisant le code de Huffman des caractères.
Cette fonction initialise l’arbre avec une racine vide, puis parcourt la pile. Pour insérer les nœuds, si on voit ”0”, on va à gauche (créer un nœud si nécessaire).
Si on voit ”1”, on va à droite (créer un nœud si nécessaire).
On stocke ensuite le symbole dans la bonne feuille lorsque tout le code est parcouru. 

void decompression(FILE* f, FILE* sortie, arbre* a){
  uint8_t c;
  int8_t i;
  char e;
  uint8_t tab[8]; 
  noeud* n = a->racine;
  while (!feof(f)){
    e = fgetc(f);
    c = (uint8_t) e;
    for (i=0; i<8; i++){
      tab[i] = c%2;
      c = c/2;
    }
    for (i=7; i>=0; i--){
      if (tab[i]==0){
    n = n->gauche;
      }
      else{
    n = n->droit;
      }
      if (est_feuille(n)){
    fprintf(sortie, "%c", n->symbole);
    n = a->racine;
      }
    }
  }
}



