%{ 
#include <stdio.h>
#include <stdlib.h>
#include "automate.c"

#if YYBISON
int yylex();
int yyerror();
#endif 
%}

%union{
char* str;
struct automate Auto;
}

%token <str> LETTRE EPSILON
%token VIDE
%token PAR_O PAR_F
%token ETOILE
%left CONCAT UNION 
%type <Auto> expression 
%%
sortie : 
        expression  {afficher($1);}
    ;

expression : 
        expression CONCAT expression {$$ = concat_automate($1,$3);}

    |   expression UNION expression {$$ = union_automate($1,$3);}

    |   expression ETOILE  {$$ = etoile_automate($1);}

    |   PAR_O expression PAR_F {$$ =$2;}

    |   LETTRE {printf("lettre = %s \n",$1);$$ = creer_automate_une_lettre($1);}

    |   EPSILON {$$ = creer_automate_une_lettre($1);}
    |   VIDE {$$ = creer_automate("vide", 1);}
    ;
%%
