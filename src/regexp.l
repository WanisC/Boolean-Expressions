
%{

#define EPSILON 0
#define LETTRE 1
#define UNION 100
#define CONCAT 101
#define ETOILE 102
#define PAR_O 200
#define PAR_F 201
#define VIDE 301

%}

%%
[a-z]   { printf("%s", yytext);     return LETTRE;  }
"+"     { printf("+");              return UNION;   }
"."     { printf(".");              return CONCAT;  }
"*"     { printf("*");              return ETOILE;  }
"("     { printf("(");              return PAR_O;   }
")"     { printf(")");              return PAR_F;   }
"E"     { printf("E");              return EPSILON; }
"0"     { printf("0");              return VIDE;    }
\n      { printf("\n"); } /* on aura besoin du saut de lignes pour notre fichier test1 */
[ \.\t] {}

%% 



/* pour lancer l'analyse syntaxique exécuter dans le terminal les commandes suivantes:  
make regexp.yy.c
make test
make exetest
*/