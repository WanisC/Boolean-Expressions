%{ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if YYBISON
int yylex();
int yyerror();
#endif 
char* concatenerChaines(const char** chaines, int nombreDeChaines);
%}

%union{
    char* str;
}

%token  <str> LETTRE EPSILON VIDE
%left   <str> UNION 
%left   <str> CONCAT 
%token  <str> ETOILE
%token  <str> PAR_O PAR_F
%type   <str> expression 
%%

sortie : 
        expression  { printf("%s ;\n",$1); }
    ;

expression : 
        expression CONCAT expression    { const char* listeChaines[] = {"concat_automate(%c, %c)", $1, $3};   $$ = concatenerChaines(listeChaines, 5); }
    |   expression UNION expression     { const char* listeChaines[] = {"union_automate(%c, %c)", $1, $3};    $$ = concatenerChaines(listeChaines, 5); }
    |   expression ETOILE               { const char* listeChaines[] = {"etoile_automate(%c)", $1};           $$ = concatenerChaines(listeChaines, 3); }
    |   PAR_O expression PAR_F          { const char* listeChaines[] = {"(%c)", $2};                          $$ = concatenerChaines(listeChaines, 3); }
    |   LETTRE                          { const char* listeChaines[] = {"creer_automate_une_lettre(%c)", $1}; $$ = concatenerChaines(listeChaines, 3); }
    |   EPSILON                         { const char* listeChaines[] = {"Ɛ"};                                 $$ = concatenerChaines(listeChaines, 3); }
    |   VIDE                            { const char* listeChaines[] = {"Mot vide"};                          $$ = concatenerChaines(listeChaines, 3); }
    ;
%%

char* concatenerChaines(const char** chaines, int nombreDeChaines) {
    // Vérifier les cas spéciaux
    if (chaines == NULL || nombreDeChaines <= 0) {
        return NULL;
    }

    // Calculer la longueur totale de la chaîne résultante
    size_t longueurTotale = 0;
    for (int i = 0; i < nombreDeChaines; i++) {
        if (chaines[i] != NULL) {
            longueurTotale += strlen(chaines[i]);
        }
    }

    // Allouer de la mémoire pour la chaîne résultante
    char* chaineResultante = (char*)malloc(longueurTotale + 1); // +1 pour le caractère nul '\0'

    // Vérifier si l'allocation de mémoire a réussi
    if (chaineResultante == NULL) {
        return NULL; // Échec de l'allocation de mémoire
    }

    // Copier chaque chaîne dans la chaîne résultante
    size_t positionActuelle = 0;
    for (int i = 0; i < nombreDeChaines; i++) {
        if (chaines[i] != NULL) {
            size_t longueurChaine = strlen(chaines[i]);
            strncpy(chaineResultante + positionActuelle, chaines[i], longueurChaine);
            positionActuelle += longueurChaine;
        }
    }

    // Ajouter le caractère nul à la fin de la chaîne résultante
    chaineResultante[positionActuelle] = '\0';

    return chaineResultante;
}