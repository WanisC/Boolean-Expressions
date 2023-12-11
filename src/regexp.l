
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
"("     { printf("(",PAR_O);    return PAR_O;   }
")"     { printf(")",PAR_F);    return PAR_F;   }
[a-z]   { printf("%s",yytext);  return LETTRE;  }
"+"     { printf("+");          return UNION;   }
"."     { printf(".");          return CONCAT;  }
"*"     { printf("*");          return ETOILE;  }
"E"     { printf("E",EPSILON);  return EPSILON; }
"0"     { printf("0",VIDE);     return VIDE;    }

\n      { printf("\n"); } /* on aura besoin du saut de lignes pour notre fichier test1 */
[ \.\t] {}

%% 

int main() {
    int retour;
    while (retour !=0) {
        retour = yylex();
    }
};

/* pour lancer l'analyse syntaxique faire dans le teminal les commande suivantes:  
make regexp.yy.c
make test
make exetest
*/