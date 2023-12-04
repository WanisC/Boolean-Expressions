#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "automate.h"

//###
// 1. Gestion des états finaux
//###
AUTOMATE etat_final_ON    (AUTOMATE A, unsigned int p) {if (p>=A.N) exit (41); A.F[p] = 1; return A;}
AUTOMATE etat_final_OFF   (AUTOMATE A, unsigned int p) {if (p>=A.N) exit (42); A.F[p] = 0; return A;}
AUTOMATE etat_fina_TOGGLE (AUTOMATE A, unsigned int p) {if (p>=A.N) exit (43); A.F[p] = 1-A.F[p]; return A;}


//###
// 2. Gestion des transitions
//###

// Renvoie vrai si la transition deb est strictment supérieure à t
int deb_sup_t (struct transition *deb, struct transition *t) {
	if (deb->p > t->p) return 1;
	if ((deb->p == t->p) && (deb->a > t->a)) return 1;
	if ((deb->p == t->p) && (deb->a == t->a) && (deb->q > t->q)) return 1;
	return 0;
}
// Insère en ordre croissant la transition t dans la liste deb
struct transition *inserer_trie (struct transition *deb, struct transition *t) {
	if (!deb) return t;
	if (deb_sup_t (deb,t)) {t->suiv = deb; return t;}
	if ((deb->p == t->p) && (deb->a == t->a) && (deb->q == t->q)) {
		fprintf(stderr,"WARNING 001: Transition (%d,%c,%d) déjà présente\n",t->p,affcar(t->a),t->q); 
		return deb;
		}
	deb->suiv = inserer_trie(deb->suiv,t);
	return deb;
	}

// Ajoute (de façon triée) la transition (p,a,q) dans A.
AUTOMATE ajoute_une_transition (AUTOMATE A, unsigned int p, char a, unsigned int q) {
	if (p>=A.N) exit (31);
	if (((a<'a') || ('z'<a)) && (a!=epsilon)) exit(32);
	if (q>=A.N) exit (33);
	struct transition *t;
	t = malloc(sizeof(struct transition)); if (!t) exit(22);
	t->p = p;
	t->a = a;
	t->q = q;
//	printf("INSERTION Transition (%d,%c,%d) \n",t->p,affcar(t->a),t->q); 
	A.T = inserer_trie(A.T,t);
	(A.nb_trans)++;
	return A;
}

// Ajoute toutes les transitions de la liste t dans l'automate A en décalant les indice de decal
AUTOMATE ajoute_toutes_les_transition (AUTOMATE A, struct transition *t, int decal) {
	while (t) {
		A = ajoute_une_transition (A, (t->p)+decal , t->a , (t->q)+decal);
		t = t->suiv;
	}
	return A;
}


//###
// 3. Création d'automates
//###

// Crée un automate N états sans transtion ni état final
AUTOMATE creer_automate (char *nom, int N) {
	AUTOMATE A;
	A.nom = nom;
	A.N = N;
	A.F = malloc(N*sizeof(int)); if (!A.F) exit(21);
	int i;
	for (i=0 ; i<N ; i++) A.F[i] = 0;
	A.nb_trans = 0;
	A.T = NULL;
	return A;
}

// Crée un automate reconnaissant uniquement le mot d'une lettre c
AUTOMATE creer_automate_une_lettre(char c) {
	char *s = malloc(2); if (!s) exit(61);
	sprintf(s,"%c",c);
	AUTOMATE A = creer_automate(s,2);
	A = ajoute_une_transition(A,0,c,1);
	etat_final_ON(A,1);
	return A;
}


//###
// 4. Affichage d'un automate
//###

// Affiche un automate
void afficher(AUTOMATE A) {
	printf("### %s : %d états, %d transitions\n",A.nom,A.N,A.nb_trans);
	printf("    États finaux :");
	int i;
	for (i=0 ; i<A.N ; i++) if (A.F[i]) printf(" %d",i);
	printf("\n");
	printf("    Transitions  :");
	struct transition *t = A.T;
	i = 1;
	while (t) {
			printf(" (%d,%c,%d)",t->p,affcar(t->a),t->q);
			t = t->suiv;
			if ((i%5 == 0) && t) printf("\n                  ");
			i++;
		}
	printf("\n\n");
}


//###
// 5. Opérations de base sur les automate : Union Concat, Étoile
//###

// Renvoie un nouvel automate qui est l'union de A et B
AUTOMATE union_automate (AUTOMATE A, AUTOMATE B) {
	char *s = malloc(strlen(A.nom)+strlen(B.nom)+4); if (!s) exit(51);
	sprintf(s,"(%s+%s)",A.nom,B.nom);
	AUTOMATE C = creer_automate(s,A.N+B.N+1);
	// Il y a un nouvel état initial
	// Les numéros d'état de A sont décalés de 1
	// Les numéors d'état de B sont décalés de 1+A.N
	// Les transitions depuis l'état initial sont un epsilon vers les états initiaux de A et B
	// Les états finaux de C sont ceux de A et ceux de B 

	// 1. Traitement des états finaux
	// Les états finaux sont l'union de ceux de A et de ceux de B
	int i;
	for (i = 0; i < A.N ; i++) C.F[i+1]     = A.F[i];
	for (i = 0; i < B.N ; i++) C.F[i+A.N+1] = B.F[i];
	// 2. Traitement des transtions
	C = ajoute_toutes_les_transition(C,A.T,1);     // Ajout des transitions de A décalées de 1
	C = ajoute_toutes_les_transition(C,B.T,1+A.N); // Ajout des transitions de B décalées de 1+A.N
	C = ajoute_une_transition(C,0,epsilon,1);         // De l'état initial de C vers celui de A
	C = ajoute_une_transition(C,0,epsilon,A.N+1);     // De l'état initial de C vers celui de B
	return C;
}

// Renvoie un nouvel automate qui est la concaténation de A et B
AUTOMATE concat_automate (AUTOMATE A, AUTOMATE B) {
	char *s = malloc(strlen(A.nom)+strlen(B.nom)+4); if (!s) exit(51);
	sprintf(s,"(%s.%s)",A.nom,B.nom);
	AUTOMATE C = creer_automate(s,A.N+B.N);
	// L'état initial de A sera celui de C
	// Les numéros des états de A restent les mêmes
	// Les numéros des états de B sont décalés de A.N
	// On ajoute les transitions depuis les états finaux de A vers l'état inital de B avec epsilon
	// Les numéros des états de B sont décalés de +(A.N)

	// 1. Traitement des états finaux
	// Les états finaux de C sont ceux de B
	int i;
	for (i = 0; i < B.N ; i++) C.F[i+A.N] = B.F[i];
	// 2. Traitement des transtions
	C = ajoute_toutes_les_transition(C,A.T,0);   // Ajout des transitions de A
	C = ajoute_toutes_les_transition(C,B.T,A.N); // Ajout des transitions de B
	// Ajout des transitions des états finaux de A vers état initial de B
	for (i = 0; i < A.N ; i++)
		if (A.F[i]) C = ajoute_une_transition(C,i,epsilon,A.N); // Ajout vers l'ancien état initial de B
	return C;
}

// Renvoie un nouvel automate qui est l'étoile de Kleene de A
AUTOMATE etoile_automate (AUTOMATE A) {
	char *s = malloc(strlen(A.nom)+4); if (!s) exit(51);
	sprintf(s,"(%s*)",A.nom);
	AUTOMATE C = creer_automate(s,A.N);
	// 1. Traitement des états finaux
	C.F[0] = 1; // L'état initial devient final
	int i;
	for (i = 1; i < A.N ; i++) C.F[i] = A.F[i];
	// 1. Traitement des transitions
	C = ajoute_toutes_les_transition(C,A.T,0);   // Ajout des transitions de A
	for (i = 0; i < A.N ; i++)
		if (A.F[i]) C = ajoute_une_transition(C,i,epsilon,0); // Ajout des transitions des états finauxvers 0
	return C;
}

//###
// 6. Suppression des epsilon-transitions
//###

AUTOMATE supprime_epsilon_transitions (AUTOMATE A) {

	// Pour chaque état q1
	for (unsigned int q1 = 0; q1 < A.N; ++q1) {
		struct transition *curr = A.T;
		//printf("Etat %d\n", q1);
		
		// Tant qu'il y a des transitions
		while (curr) {
			// On cherche les transitions (q1,epsilon,q2)
			if (curr->p == q1 && curr->a == 0) {
				//printf("	1. Transition epsilon (%d,%c,%d)\n", curr->p, affcar(curr->a), curr->q);
				// On initialise l'état q3 que l'on va actualiser dans le cas d'une transition epsilon
				//printf("		1.1. Etat q3 %d\n", curr->q);
				struct transition *curr2 = A.T;
				//printf("		1.2. Recherche des transitions depuis q3\n");
				while (curr2) {
					// Cas: on trouve une transition non epsilon
					if (curr2->p == curr->q && curr2->a != 0) {
						struct transition *verif = A.T; // On vérifie que la transition n'existe pas déjà
						int is_in = 0;
						while (verif) {
							// Cas: la transition existe déjà
							if (verif->p == q1 && verif->a == curr2->a && verif->q == curr2->q) {
								//printf("		1.3. Transition déjà présente (%d,%c,%d)\n", curr2->p, affcar(curr2->a), curr2->q);
								is_in = 1;
								break;
							}
							verif = verif->suiv;
						}
						if (!is_in) {
							//printf("		2. Transition NON epsilon trouvée (%d,%c,%d)\n", curr2->p, affcar(curr2->a), curr2->q);
							// On ajoute la transition (q1,a,q3)
							A = ajoute_une_transition(A, q1, curr2->a, curr2->q);
							//printf("		2.1. Transition ajoutée (%d,%c,%d)\n", q1, affcar(curr2->a), curr2->q);
							curr2 = curr2->suiv;
						} else curr2 = curr2->suiv;
					} else {
						curr2 = curr2->suiv;
					}
				}
				curr = curr->suiv;
				free(curr2);
			} else {
				curr = curr->suiv;
			}
		}
		free(curr);
	}
	
	// Phase 2 : Suppression des transitions epsilon
	struct transition *supp_e = A.T;
	while (supp_e) {
		// Cas: la transition n'est pas epsilon
		if (supp_e->a != 0) supp_e = supp_e->suiv;
		else {
			struct transition *temp = supp_e->suiv;
			A.T = supp_e;
			A.T->suiv = temp;
			supp_e = supp_e->suiv;
			A.nb_trans--;
		}
	}
	return A;
}

//###
// 7. Déterminisation
//###

AUTOMATE determinise (AUTOMATE A) {
	return A;
}

//###
// 8. Minimisation
//###

AUTOMATE minimise (AUTOMATE A) {
	return A;
}


//###
// 9. Reconnaissance
//###

// Renvoie vrai si le mot mot est reconnu par l'automate A
int reconnait (AUTOMATE A, char *mot) {
	int retour = 0;
	unsigned int curr_etat = 0; // Pointeur pour sauvegarder l'état courant
	struct transition *t = A.T;
	while (mot[0] != 0) { // Tant qu'il reste des lettres dans le mot
		while (t && (t->p != curr_etat) && (t->a != mot[0])) t = t->suiv; // Recherche de la transition (p,mot[0],q)
		if (t) { // Si la transition existe
			t = t->suiv;
			curr_etat = t->q; // On passe à l'état suivant
			mot++;
		}
		else break; // Pas de transition
	}
	retour = A.F[curr_etat]; // Si l'état dans lequel est final alors le mot est reconnu, sinon non
	if (retour) printf("%s EST RECONNU PAR %s\n",mot, A.nom);
	else    printf("%s N'est PAS reconnu par %s\n",mot, A.nom);
	return retour;
}