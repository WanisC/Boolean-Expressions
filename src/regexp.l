
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

[a-z]  {return LETTRE;}
"+"     {return UNION;}
"\."     {return CONCAT;}
"*"     {return ETOILE;}
"E"     {return EPSILON;}
"0"     {return VIDE;}
"("     {return PAR_O;}
")"     {return PAR_F;}
\n      {} /* on aura besoin du saut de lignes pour notre fichier test1 */
[ \.\t]   {}

%% 