
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
"("    {printf("%d ",PAR_O);}
")"    {printf("%d ",PAR_F);}
[a-z]  {printf("%s ",yytext);}
"+"    {printf("%d ",UNION);}
"."   {printf("%d ",CONCAT);}
"*"    {printf("%d ",ETOILE);}
"E"    {printf("%d ",EPSILON);}
"0"    {printf("%d ",VIDE);}

\n      {printf("%s \n", yytext);} /* on aura besoin du saut de lignes pour notre fichier test1 */
[ \.\t]   {}

%% 

int main() {
};

/* pour lancer l'analyse syntaxique faire dans le teminal les commande suivantes:  
make regexp.yy.c
make test
make exetest
*/