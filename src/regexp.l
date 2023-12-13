
%{
#include "regexp.tab.h"
#include "automate.c"


%}

%%
[a-z]   { printf("%s", yytext); yylval.str = strdup(yytext); return LETTRE;  }
"+"     { printf("+");              return UNION;   }
"."     { printf(".");              return CONCAT;  }
"*"     { printf("*");              return ETOILE;  }
"("     { printf("(");              return PAR_O;   }
")"     { printf(")");              return PAR_F;   }
"E"     { printf("E");              return EPSILON; }
"0"     { printf("0");              return VIDE;    }
\n      { printf("\n"); return 0;  } /* on aura besoin du saut de lignes pour notre fichier test1 */
[ \.\t] {}

%% 



/* pour lancer l'analyse syntaxique exécuter dans le terminal les commandes suivantes:  
make regexp.yy.c
make test
make exetest
*/