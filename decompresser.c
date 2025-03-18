#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>



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
void afficher_pile(pile*);

//Fonctions relatifs à la structure arbre:
arbre* new_arbre();
arbre* creer_arbre(pile*);
arbre* init_arbre(FILE*, uint32_t);
void arbre_free(noeud*);

//Fonctions relatifs à l'algorithme d'Huffman:
void decompression(FILE*, FILE*, arbre*);



int main(int argc, char* argv[]){
  FILE* f = fopen("mieuxquewinrar.txt", "rb");
  
  uint32_t t = 0;
  char type[5];
  if (!feof(f)){
    fscanf(f, "%s %d\n", type, &t);
  }
  char c[20] = "fautdecompresser.";
  strcat(c, type);
  
  FILE* sortie = fopen(c, "wb");
  arbre* a = init_arbre(f, t);
  decompression(f, sortie, a);

  //Libération mémoire
  arbre_free(a->racine);
  free(a);
  fclose(sortie);
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
  noeud* n = new_noeud();
  noeud* n1 = new_noeud();
  noeud* n2;
  uint8_t l, i;
  pile* p2 = new_pile();
  arbre* a = new_arbre();
  a->racine = n;
  while (!pile_estVide(p)){
    n1 = a->racine;
    n = depiler(p);
    l = strlen(n->code);
    for(i=0; i<l; i++){
      if (n->code[i]=='0'){
	if (n1->gauche==NULL){
	  n2 = new_noeud();
	  n1->gauche = n2;
	}
	n1 = n1->gauche;
      }
      else{
	if (n1->droit==NULL){
	  n2 = new_noeud();
	  n1->droit = n2;
	}
	n1 = n1->droit;
      }
    }
    n1->code = strdup(n->code);
    n1->symbole = n->symbole;
    empiler(p2, n);
  }
  while (!pile_estVide(p2)){
    empiler(p, depiler(p2));
  }
  free(p2);
  return a;
}


arbre* init_arbre(FILE* f, uint32_t t){
  noeud* n;
  pile* p = new_pile();
  char code[100];
  uint32_t i;
  for(i=0; i<t ;i++){
    n = new_noeud();
    fscanf(f, "/%c %s /", &(n->symbole), code);
    n->code = strdup(code);
    empiler(p, n);
  }
  arbre* a = creer_arbre(p);
  free(p);
  return a;
}


void arbre_free(noeud* n){
  if (!est_feuille(n)){
    arbre_free(n->gauche);
    arbre_free(n->droit);
  }
  free(n);
}


//Fonctions relatifs à l'algorithme d'Huffman:

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
