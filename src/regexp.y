%{ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if YYBISON
    int yylex();
    int yyerror();
#endif 
char* concatenerChaines(const char** chaines, int nombreDeChaines);
char* itc(int entier );
int cpt = 0;
%}

/* https://cs.wmich.edu/~yang/teach/cs485/yacc.pdf */ 

%union{
    char* str;
}

%token  <str> LETTRE EPSILON VIDE MOT NEWLINE
%left   <str> UNION
%left   <str> CONCAT  
%token  <str> ETOILE
%token  <str> PAR_O PAR_F
%type   <str> expression 
%type   <str> mots
%type   <str> sortie

%%

sortie : 
        expression NEWLINE mots {
                            printf("#include <stdio.h>\n#include <stdlib.h>\n#include \"automate.h\"\n\nint main() {\n%s\nreturn 0;\n}\n", $1); 
                            printf("%s\n", $3);
                            printf("cpt d'op = %d\n", cpt);
                                }
    ;

expression : 
        expression CONCAT expression  { const char* listeChaines[] = { $1, $3, "AUTOMATE A", itc(cpt), " = concat_automate(A", itc(cpt-2), ", A", itc(cpt-1), ");\n" };        
                                        $$ = concatenerChaines(listeChaines, 9); cpt ++; }

    |   expression UNION expression   { const char* listeChaines[] = { $1, $3, "AUTOMATE A", itc(cpt), " = union_automate(A", itc(cpt-2), ", A", itc(cpt-1), ");\n" };         
                                        $$ = concatenerChaines(listeChaines, 9); cpt ++; }

    |   expression ETOILE             { const char* listeChaines[] = { $1, "AUTOMATE A", itc(cpt), " = etoile_automate(A", itc(cpt-1), ");\n" };                               
                                        $$ = concatenerChaines(listeChaines, 6); cpt ++; }

    |   PAR_O expression PAR_F        { $$ = $2; }

    |   LETTRE                        { const char* listeChaines[] = { "AUTOMATE A", itc(cpt), " = creer_automate_une_lettre(" , $1, ");\n" };                                 
                                        $$ = concatenerChaines(listeChaines, 5); cpt ++; }

    |   EPSILON                       { const char* listeChaines[] = { "AUTOMATE A", itc(cpt), " = creer_automate('Epsilon', 1);\n", "ETAT_FINAL_ON(A", itc(cpt), ", 0);\n" }; 
                                        $$ = concatenerChaines(listeChaines, 6); cpt ++; }

    |   VIDE                          { const char* listeChaines[] = { "AUTOMATE A", itc(cpt), " = creer_automate('Vide', 1);\n" };                                            
                                        $$ = concatenerChaines(listeChaines, 3); cpt ++; }
    ;

mots :
        MOT NEWLINE                   { const char* listeChaines[] = { "reconnait(A_determinise, ", $1, ");\n" };                                            
                                        $$ = concatenerChaines(listeChaines, 3); }
    |   EPSILON NEWLINE               { const char* listeChaines[] = { "reconnait(A_determinise, ", $1, ");\n" };                                            
                                            $$ = concatenerChaines(listeChaines, 3); }
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


char* itc(int entier ) {
    char *chaine = (char *)malloc(20 * sizeof(char));

    sprintf(chaine, "%d", entier);

    return chaine;
}

int main() {
    yyparse();
    return 0;
}