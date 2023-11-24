
%{
#include <stdio.h>

#define LETTRE 1
#define UNION 100
#define CONCAT 101
#define ETOILE 102
#define PARENTHESE_OUVRANTE 200
#define PARENTHESE_FERMANTE 201
#define EPSILON 300
#define VIDE 301

%}

lettre [a-z]+
union [+]*
concat \.
etoile [*]+
epsilon E
vide 0
parenthese_ouverte (
parenthese_fermee )

%%

{lettre}              {printf("Lettre\n");}
{union}               {printf("Union\n");}
{concat}              {printf("Concaténation\n");}
{etoile}              {printf("Etoile\n");}
{epsilon}             {printf("Epsilon\n");}
{vide}                {printf("Vide\n");}
{parenthese_ouverte}  {printf("Parenthèse ouvrante\n");}
{parenthese_fermee}   {printf("Parenthèse fermante\n");}
[ \.\n\t]   {}

%%

int main() {
    yylex();
    return 0;
}