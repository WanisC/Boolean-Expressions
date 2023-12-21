#include "automate.h"

int main () {
    AUTOMATE A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, A17, A18, A19, A20, A21, A22, A23, A24, A25, A_sans_epsilon, A_determinise;
    AUTOMATE A_final;

    // Automate pour tester la méthode de suppression des epsilon transitions
    // A0 = creer_automate_une_lettre('a');
    // A1 = creer_automate_une_lettre('b');
    // A2 = union_automate(A0,A1);
    // A3 = etoile_automate(A2);
    // A4 = creer_automate_une_lettre('c');
    // A5 = concat_automate(A3, A4);

    // A_final = supprime_epsilon_transitions (A5);
    // A_final = determinise(A_final);


    // Automate pour tester la méthode de déterminisation
    // A0 = creer_automate_une_lettre('a');
    // A1 = creer_automate_une_lettre('a');
    // A2 = union_automate(A0, A1);
    // A3 = creer_automate_une_lettre('a');
    // A4 = union_automate(A2, A3);

    // A_final = supprime_epsilon_transitions (A4);
    // A_final = determinise(A_final);
    
    A0 = creer_automate_une_lettre('a');
    A1 = creer_automate_une_lettre('b');
    A2 = union_automate(A0, A1);
    A3 = creer_automate_une_lettre('c');
    A4 = union_automate(A2, A3);
    A5 = creer_automate_une_lettre('a');
    A6 = creer_automate_une_lettre('b');
    A7 = concat_automate(A5, A6);
    A8 = creer_automate_une_lettre('c');
    A9 = creer_automate_une_lettre('a');
    A10 = union_automate(A8, A9);
    A11 = etoile_automate(A10);
    A12 = concat_automate(A7, A11);
    A13 = concat_automate(A4, A12);
    A14 = creer_automate_une_lettre('a');
    A15 = creer_automate_une_lettre('b');
    A16 = union_automate(A14, A15);
    A17 = etoile_automate(A16);
    A18 = creer_automate_une_lettre('c');
    A19 = creer_automate_une_lettre('z');
    A20 = creer_automate_une_lettre('a');
    A21 = concat_automate(A19, A20);
    A22 = union_automate(A18, A21);
    A23 = etoile_automate(A22);
    A24 = concat_automate(A17, A23);
    A25 = union_automate(A13, A24);

    A_sans_epsilon = supprime_epsilon_transitions(A25);
    A_determinise = determinise(A_sans_epsilon);

    afficher(A_determinise);

    reconnait(A_determinise,"ab");
    reconnait(A_determinise,"a");
    reconnait(A_determinise,"aaaaac");
    reconnait(A_determinise,"c");
    reconnait(A_determinise,"ca");
    reconnait(A_determinise,"");


    return 0;
}