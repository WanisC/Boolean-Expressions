%{ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if YYBISON
    int yylex(); // Fonction de l'analyseur lexical
    int yyerror(); // Fonction de gestion des erreurs
#endif 
char* concatenerChaines(const char** chaines, int nombreDeChaines);
char* itc(int entier);
char* genererChainesAutomates(int x);
int genere_main(int cpt, char* code, char* reconnaissance);
int cpt = 0;
int autodroit = 0;
char* code;
%}

%union{
    unsigned int op;
    char* str;  
}

%token  <str> LETTRE EPSILON VIDE MOT NEWLINE
%left   <op> UNION
%left   <op> CONCAT  
%token  <op> ETOILE
%token  <op> PAR_O PAR_F
%type   <str> expression 
%type   <str> mots
%type   <str> sortie

%%

sortie : 
        expression NEWLINE mots { genere_main(cpt, code, $3); }
    ;

expression : 
        expression CONCAT expression  { autodroit = cpt -1 - $<op>3; $$=  $<op>1 + $<op>3 + 1;
                                        const char* listeChaines[] = { code, "A", itc(cpt), " = concat_automate(A", itc(autodroit), ", A", itc(cpt-1), ");\n" };        
                                        code = concatenerChaines(listeChaines, 8); cpt ++; }

    |   expression UNION expression   { autodroit = cpt -1 - $<op>3; $$=  $<op>1 + $<op>3 + 1;
                                        const char* listeChaines[] = { code, "A", itc(cpt), " = union_automate(A", itc(autodroit), ", A", itc(cpt-1), ");\n" };         
                                        code = concatenerChaines(listeChaines, 8); cpt ++; }

    |   expression ETOILE             { $$=  $<op>1  + 1;                                     
                                        const char* listeChaines[] = { code,  "A", itc(cpt), " = etoile_automate(A", itc(cpt-1), ");\n" };                               
                                        code = concatenerChaines(listeChaines,6 ); cpt ++; }

    |   PAR_O expression PAR_F        { $$ = $2; }

    |   LETTRE                        { $<op>$ = 1 ;                                          
                                        const char* listeChaines[] = { code, "A", itc(cpt), " = creer_automate_une_lettre(\'", $1, "\');\n" };                                 
                                        code = concatenerChaines(listeChaines, 6); cpt ++;  }

    |   EPSILON                       { $<op>0 = 1 ;                                          
                                        const char* listeChaines[] = { code, "A", itc(cpt), " = creer_automate(\"E\", 1);\n", "etat_final_ON(A", itc(cpt), ", 0);\n" }; 
                                        code = concatenerChaines(listeChaines, 7); cpt ++;  }

    |   VIDE                          { $<op>0 = 1 ;                                         
                                        const char* listeChaines[] = { code, "A", itc(cpt), " = creer_automate(\"V\", 1);\n" };                                            
                                        code = concatenerChaines(listeChaines, 4); cpt ++;  }            
    ;

mots :
        mots NEWLINE mots             { const char* listeChaines[] = { $1, $3 };                                            
                                        $$ = concatenerChaines(listeChaines, 2); }

    |   MOT                           { const char* listeChaines[] = { "reconnait(A_final, \"", $1, "\");\n" };                                            
                                        $$ = concatenerChaines(listeChaines, 3); }

    |   LETTRE                        { const char* listeChaines[] = { "reconnait(A_final, \"", $1, "\");\n" };                                            
                                        $$ = concatenerChaines(listeChaines, 3); }

    |   EPSILON                       { $$ = "reconnait(A_final, \"E\");\n" ; }

    |   NEWLINE mots                  { $$ = $2; }
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

// Convertir un entier en chaîne de caractères
char* itc(int entier) {
    char *chaine = (char *)malloc(100 * sizeof(char));

    sprintf(chaine, "%d", entier);

    return chaine;
}

// Générer la chaîne de caractères "AUTOMATE A0, A1, A2, ..., An;"
char* genererChainesAutomates(int x) {
    if (x < 0) {
        return NULL; // Gestion du cas où x est négatif
    }

    // Allouer de la mémoire pour la chaîne résultante
    char* chaineResultante = (char*)malloc(100 * (x + 1));
    chaineResultante[0] = '\0'; 

    strcat(chaineResultante, "AUTOMATE ");
    // Concaténer les noms des automates à la chaîne résultante
    for (int i = 0; i <= x; i++) {
        char automate[20]; 
        sprintf(automate, "A%d", i);
        strcat(chaineResultante, automate);

        if (i < x) {
            strcat(chaineResultante, ", ");
        }
    }

    return chaineResultante;
}

// Générer le fichier main.c
int genere_main(int cpt, char* code, char* reconnaissance) {
    FILE *file;

    file = fopen("main.c", "w");

    if (file == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier main.c\n");
        return 1;
    }

    fprintf(file, "#include \"automate.h\"\n\n");
    fprintf(file, "int main() {\n");
    fprintf(file, genererChainesAutomates(cpt-1));
    fprintf(file, ";\n");
    fprintf(file, "AUTOMATE A_final, A_sans_epsilon, A_determinise;\n\n");
    fprintf(file, code);
    fprintf(file, "\n");
    fprintf(file, "A_sans_epsilon = supprime_epsilon_transitions(A%d);", cpt-1);
    fprintf(file, "\n");
    fprintf(file, "A_determinise = determinise(A_sans_epsilon);");
    fprintf(file, "\n");
    fprintf(file, "A_final = minimise(A_determinise);");
    fprintf(file, "\n\n");
    fprintf(file, "afficher(A_final); \n\n");
    fprintf(file, "%s\n", reconnaissance);
    fprintf(file, "return 0;\n");
    fprintf(file, "}\n");

    fclose(file);

    return 0;
}

int main() {
    yyparse();
    return 0;
}