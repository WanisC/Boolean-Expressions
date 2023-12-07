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

// Renvoie vrai si la transition (p,a,q) est dans l'automate A
int transition_presente (AUTOMATE A, unsigned int p, char a, unsigned int q) {
	struct transition *t = A.T;
	while (t) {
		if (t->p == p && t->a == a && t->q == q) return 1;
		t = t->suiv;
	}
	return 0;
}

AUTOMATE supprime_epsilon_transitions (AUTOMATE A) {
	// Pour chaque état q1
	for (unsigned int q1 = 0; q1 < A.N; q1++) {
		//printf("Etat Q1: %d\n", q1);

		// On va parcourir les transitions de A
		struct transition *simple_trans = A.T;
		
		// Tant qu'il y a des transitions
		while (simple_trans) {

			// Cas: c'est une transition epsilon avec q1 comme état de départ
			if (simple_trans->p == q1 && simple_trans->a == 0) {
				//printf("	1. Transition epsilon (%d,%c,%d)\n", simple_trans->p, affcar(simple_trans->a), simple_trans->q);

				// On initialise l'état q2 que l'on va actualiser dans le cas d'une transition epsilon
				unsigned int q2 = simple_trans->q;
				//printf("		1.1. Etat Q2: %d\n", q2);

				// On va parcourir les transitions de A depuis q2
				struct transition *q3_trans = A.T;

				//printf("		1.2. Recherche des transitions depuis q2\n");
				while (q3_trans) {

					// Cas: on trouve une transition non epsilon
					if (q3_trans->p == q2 && q3_trans->a != 0) {
						//printf("			Transition non epsilon trouvée: (%d,%c,%d)\n", q2, affcar(q3_trans->a), q3_trans->q);
						//printf("			- Vérifions si la transition (%d,%c,%d) existe déjà\n", q1, affcar(q3_trans->a), q3_trans->q);

						// Cas: la transition n'existe pas
						if (!transition_presente(A, q1, q3_trans->a, q3_trans->q)) {
							//printf("				|-Transition (%d,%c,%d) NON présente\n", q1, affcar(q3_trans->a), q3_trans->q);

							// On ajoute la transition (q1,a,q3)
							A = ajoute_une_transition(A, q1, q3_trans->a, q3_trans->q);
							//printf("				|-Transition ajoutée\n");

							// On regarde si q2 est un état final pour modifier q1 si besoin
							if (A.F[q2]) {
								etat_final_ON(A, q1);
							}
						}
						q3_trans = q3_trans->suiv;

					// Cas: on trouve une transition epsilon
					} else if (q3_trans->p == q2 && q3_trans->a == 0) {
						//printf("			Transition epsilon (%d,%c,%d)\n", q3_trans->p, affcar(q3_trans->a), q3_trans->q);

						q2 = q3_trans->q; // On démarre la recherche depuis q2
						//printf("			Recherche des transitions depuis: %d\n", q2);

						struct transition *q4_trans = A.T;
						while (q4_trans) {
							// Cas: on trouve une transition non epsilon
							if (q4_trans->p == q2 && q4_trans->a != 0) {
								//printf("			Transition non epsilon trouvée: (%d,%c,%d)\n", q4_trans->p, affcar(q4_trans->a), q4_trans->q);
								//printf("			- Vérifions si la transition (%d,%c,%d) existe déjà\n", q1, affcar(q4_trans->a), q4_trans->q);

								// Cas: la transition n'existe pas
								if (!transition_presente(A, q1, q4_trans->a, q4_trans->q)) {
									//printf("				|-Transition (%d,%c,%d) NON présente\n", q1, affcar(q4_trans->a), q4_trans->q);

									// On ajoute la transition (q1,a,q3)
									A = ajoute_une_transition(A, q1, q4_trans->a, q4_trans->q);
									//printf("				|-Transition ajoutée\n");

									// On regarde si q2 est un état final pour modifier q1 si besoin
									if (A.F[simple_trans->p]) {
										etat_final_ON(A, q1);
									}
								}
								// On passe à la transition suivante et on actualise q2
								q3_trans = q3_trans->suiv;
								q2 = q3_trans->p;
								break;
							} else q4_trans = q4_trans->suiv;
						}
					} else q3_trans = q3_trans->suiv;
				}
				free(q3_trans);
			}
			simple_trans = simple_trans->suiv;
		}
		free(simple_trans);
	}
	
	// Suppression des transitions epsilon
	struct transition *supp_e = A.T;
	struct transition *prev = NULL;
	// Tant qu'il y a des transitions
	while (supp_e) {
		// Cas: c'est une transition epsilon
		if (supp_e->a == 0) {
			// On prend la transition suivante
			struct transition *next = supp_e->suiv;
			// On libère la mémoire de la transition epsilon
			free(supp_e);
			// On met à jour les pointeurs
			// Cas: c'est la première transition
			if (prev == NULL) {
				A.T = next; 
			// Cas: ce n'est pas la première transition
			} else {
				prev->suiv = next;
			}
			// On passe à la transition suivante
			supp_e = next;
			// On décrémente le nombre de transitions
			A.nb_trans--;
		// Cas: ce n'est pas une transition epsilon
		} else { // On passe simplement à la transition suivante
			prev = supp_e;
			supp_e = supp_e->suiv;
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
	// Tant qu'il reste des lettres dans le mot
	while (mot[0] != 0) { 
		struct transition *t = A.T;
		// Recherche de la transition (curr_etat, mot[0], q)
		while (t && t->p != curr_etat && t->a != mot[0]) t = t->suiv;
		// Cas: on trouve une transition correspondante à (curr_etat, mot[0], q)
		if (t) {
			curr_etat = t->q; // On passe à l'état suivant
			t = t->suiv; // On passe à la transition suivante
			mot++; // On passe à la lettre suivante
		}
		// Cas: on ne trouve pas de transition correspondante à (curr_etat, mot[0], q)
		else continue;
	}
	retour = A.F[curr_etat]; // Si l'état dans lequel est final alors le mot est reconnu, sinon non
	if (retour) printf("%s EST RECONNU PAR %s\n", mot, A.nom);
	else    printf("%s N'est PAS reconnu par %s\n", mot, A.nom);
	return retour;
}