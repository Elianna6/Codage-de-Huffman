#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>



//Definiton des structures:

typedef struct noeud{
  char symbole;
  char* code;
  uint64_t freq;
  struct noeud* droit;
  struct noeud* gauche;
}noeud;


typedef struct arbre{
  noeud* racine;
}arbre;

  
typedef struct maillon{
  noeud* donnee;
  struct maillon* suiv;
}maillon;


typedef struct pile{
  uint16_t taille;
  maillon* tete;
}pile;



//Fonctions relatifs à la structure noeud:
noeud* new_noeud();
uint8_t noeud_estVide(noeud*);
uint8_t est_feuille(noeud*);

//Fonctions relatifs aux structures maillon et pile:
maillon* new_maillon(noeud*);
pile* new_pile();
uint8_t pile_estVide(pile*);
void empiler(pile*, noeud*);
noeud* depiler(pile*);
void ranger_pile(pile*, noeud*);
void afficher_pile(pile*);

//Fonctions relatifs à la structure arbre:
arbre* new_arbre();
arbre* creer_arbre(pile*);
noeud* fusion(noeud*, noeud*);
void definir_code(arbre*);
void parcourir_arbre(noeud*, char*);
void arbre_free(noeud*);

//Fonctions relatifs à l'algorithme d'Huffman:
void chercherdans_pile(pile*, char);
pile* lire_fichier(FILE*);
void compression(FILE*, pile*, char*);
noeud* codage_noeud(pile*, char);



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


//Fonctions relatifs à la structure noeud:

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


uint8_t noeud_estVide(noeud* n){
  uint8_t re = 1;
  if (n->symbole != '\0'){
    re = 0;
  }
  return re;
}


uint8_t est_feuille(noeud* n){
  uint8_t re = 0;
  if (n->droit == NULL && n->gauche == NULL){
    re = 1;
  }
  return re;
}


//Fonctions relatifs aux structures maillon et pile:

maillon* new_maillon(noeud* n){
  maillon* m = malloc(sizeof(maillon));
  if (m==NULL){
    assert(0);
  }
  m->donnee = n;
  m->suiv = NULL;
  return m;
}


pile* new_pile(){
  pile* p = malloc(sizeof(pile));
  if (p==NULL){
    assert(0);
  }
  p->taille = 0;
  p->tete = NULL;
  return p;
}


uint8_t pile_estVide(pile* p){
  uint8_t re = 1;
  if (p->taille != 0){
    re = 0;
  }
  return re;
}


void empiler(pile* p, noeud* n){
  maillon* m = new_maillon(n);
  m->suiv = p->tete;
  p->tete = m; 
  p->taille += 1;
}


noeud* depiler(pile* p){
  maillon* m = p->tete;
  p->tete = m->suiv;
  p->taille -= 1;
  noeud* n = m->donnee;
  free(m);
  return n;
}


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


void afficher_pile(pile* p){
  pile* r = new_pile();
  noeud* n;
  while(!pile_estVide(p)){
    n = depiler(p);
    printf("%c / %ld / %s\n", n->symbole, n->freq, n->code);
    empiler(r, n);
  }
  while(!pile_estVide(p)){
    n = depiler(r);
    empiler(p, n);
  }
  free(r);
}


//Fonctions relatifs à la structure arbre:

arbre* new_arbre(){
  arbre* a = malloc(sizeof(arbre));
  if (a==NULL){
    assert(0);
  }
  a->racine = NULL;
  return a;
}


arbre* creer_arbre(pile* p){
  noeud* n;
  noeud* n1;
  noeud* n2;
  pile* p2 = new_pile();
  arbre* a = new_arbre();
  while (!pile_estVide(p)){
    n1 = depiler(p);
    if (!pile_estVide(p)){
      n2 = depiler(p);
      n = fusion(n1, n2);
      ranger_pile(p, n);
      if (est_feuille(n1)){
	  ranger_pile(p2, n1);
      }
      if (est_feuille(n2)){
	  ranger_pile(p2, n2);
      }
    }
    else{
      empiler(p, n1);
      break;
    }
  }
  n = depiler(p);
  a->racine = n;
  while (!pile_estVide(p2)){
    empiler(p, depiler(p2));
  }
  free(p2);
  return a;
}


noeud* fusion(noeud* n1, noeud* n2){
  noeud* n = new_noeud();
  n->freq = n1->freq + n2->freq;
  n->droit = n1;
  n->gauche = n2;
  return n;
}


void definir_code(arbre* a){
  parcourir_arbre(a->racine, "");
}


void parcourir_arbre(noeud* n, char* c){
  if (est_feuille(n)){
    n->code = strdup(c);
  }
  else{
    char c1[20], c2[20];
    strcpy(c1, c);
    strcpy(c2, c);     
    strcat(c1, "0");
    strcat(c2, "1");
    parcourir_arbre(n->gauche, c1);
    parcourir_arbre(n->droit, c2);
  }
}


void arbre_free(noeud* n){
  if (!est_feuille(n)){
    arbre_free(n->gauche);
    arbre_free(n->droit);
  }
  free(n);
}


//Fonctions relatifs à l'algorithme d'Huffman:

void chercherdans_pile(pile* p, char a){
  noeud* n;
  if (pile_estVide(p)){
    n = new_noeud();
    n->symbole = a;
    n->freq = 1;
    empiler(p, n);
  }
  else{
    n = depiler(p);
    if (n->symbole == a){
      n->freq++;
    }
    else{
      chercherdans_pile(p, a);
    }
    ranger_pile(p, n);
  }
}


pile* lire_fichier(FILE* f){
  char a;
  pile* p = new_pile();
  while (!feof(f)){
    fscanf(f, "%c", &a);
    chercherdans_pile(p, a);
  }
  return p;
}


void compression(FILE* f, pile* p, char* type){
  FILE* sortie = fopen("mieuxquewinrar.txt", "wb");
  fseek(f, 0, SEEK_SET);
  pile* q = new_pile();
  char a;
  uint8_t i, b, j=0;
  char* code;
  noeud* n;
  fprintf(sortie, "%s %d\n", type, p->taille);
  while(!pile_estVide(p)){
    n = depiler(p);
    fprintf(sortie, "/%c %s /", n->symbole, n->code);
    empiler(q, n);
  }
  while (!pile_estVide(q)){
    n = depiler(q);
    empiler(p, n);
  }
  if (!feof(f)){
    fscanf(f, "%c", &a);
    n = codage_noeud(p, a);
    code = strdup(n->code);
  }
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


noeud* codage_noeud(pile* p, char a){
  noeud* n = depiler(p);
  noeud* n1 = n;
  if (n->symbole != a){
    n1 = codage_noeud(p, a);
  }
  empiler(p, n);
  return n1;
}
