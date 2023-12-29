%{

#include "regexp.tab.h"

%}

%%
[a-z]       { printf("%s", yytext);yylval.op = 1; yylval.str = strdup(yytext); return LETTRE; }
[a-z]+      { printf("%s", yytext); yylval.str = strdup(yytext); return MOT; }
"+"         { printf("+");  return UNION;   }
"."         { printf(".");  return CONCAT;  }
"*"         { printf("*");  return ETOILE;  }
"("         { printf("(");  return PAR_O;   }
")"         { printf(")");  return PAR_F;   }
"E"         { printf("E"); yylval.op = 1 ;  return EPSILON; }
"O"         { printf("0"); yylval.op = 1 ;  return VIDE;    }
\n          { printf("\n"); return NEWLINE; } /* on aura besoin du saut de lignes pour notre fichier test1 */
[\.\t ]     { printf("Caractère inconnu\n"); exit(1); }
%% 



/* pour lancer l'analyse syntaxique exécuter dans le terminal les commandes suivantes:  
make regexp.yy.c
make test
make exetest
*/
