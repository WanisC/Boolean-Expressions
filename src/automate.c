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
	printf("    État poubelle : %d\n", A.N-1);
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
	// On va parcourir les transitions de A
	struct transition *t = A.T;
	while (t) {
		// Si on trouve la transition (p,a,q) alors on renvoie vrai
		if (t->p == p && t->a == a && t->q == q) return 1;
		// Sinon on passe à la transition suivante
		t = t->suiv;
	}
	// Si on arrive ici alors la transition n'est pas présente dans A
	return 0;
}

AUTOMATE supprime_epsilon_transitions (AUTOMATE A) {
	// Pour chaque état q1
	for (unsigned int q1 = 0; q1 < A.N; q1++) {
		//printf("Etat Q1: %d\n", q1);

		// On va parcourir les transitions de A
		struct transition *simple_trans = A.T;
		while (simple_trans) {

			// Cas: c'est une transition epsilon avec q1 comme état de départ
			if (simple_trans->p == q1 && simple_trans->a == 0) {
				//printf("	1. Transition epsilon (%d,%c,%d)\n", simple_trans->p, affcar(simple_trans->a), simple_trans->q);

				// On initialise l'état q2 que l'on va actualiser dans le cas d'une transition epsilon
				unsigned int q2 = simple_trans->q;
				//printf("		1.1. Etat Q2: %d\n", q2);

				// On va parcourir les transitions de A depuis q2
				struct transition *q2_trans = A.T;

				//printf("		1.2. Recherche des transitions depuis q2\n");
				// On va parcourir les transitions de A depuis q2
				while (q2_trans) {

					// Cas: on trouve une transition non epsilon
					if (q2_trans->p == q2 && q2_trans->a != 0) {
						//printf("			Transition non epsilon trouvée: (%d,%c,%d)\n", q2, affcar(q2_trans->a), q2_trans->q);
						//printf("			- Vérifions si la transition (%d,%c,%d) existe déjà\n", q1, affcar(q2_trans->a), q2_trans->q);

						// Cas: la transition n'existe pas
						if (!transition_presente(A, q1, q2_trans->a, q2_trans->q)) {
							//printf("				|-Transition (%d,%c,%d) NON présente\n", q1, affcar(q2_trans->a), q2_trans->q);

							// On ajoute la transition (q1,a,q3)
							A = ajoute_une_transition(A, q1, q2_trans->a, q2_trans->q);
							//printf("				|-Transition ajoutée\n");

							// On regarde si q2 est un état final pour modifier q1 si besoin
							if (A.F[q2]) {
								etat_final_ON(A, q1);
							}
						}
						q2_trans = q2_trans->suiv;

					// Cas: on trouve une transition epsilon
					} else if (q2_trans->p == q2 && q2_trans->a == 0) {
						//printf("			Transition epsilon (%d,%c,%d)\n", q2_trans->p, affcar(q2_trans->a), q2_trans->q);

						q2 = q2_trans->q; // On démarre la recherche depuis q2
						//printf("			Recherche des transitions depuis: %d\n", q2);

						// On va parcourir les transitions de A depuis q2
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
								q2_trans = q2_trans->suiv;
								q2 = q2_trans->p;
								break;
							} else q4_trans = q4_trans->suiv;
						}
					} else q2_trans = q2_trans->suiv;
				}
				free(q2_trans);
			}
			simple_trans = simple_trans->suiv;
		}
		free(simple_trans);
	}

	// On va parcourir les transitions de A pour trouver les transitions epsilon et les supprimer
	struct transition *supp_e = A.T;
	struct transition *prev = NULL;
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
	// Vérification si l'automate est déjà déterministe
	//###

// Renvoie vrai si la lettre est déjà présente dans le tableau
int dejaPresent(char lettre, char *tab, int taille) {
	for (int i = 0; i < taille; i++) {
		if (tab[i] == lettre) {
			return 1;
		}
	}
	return 0;
}

// Filtre le tableau pour garder que les lettres uniques
char* filtrage(char *tab, int taille, int *nb_element) {
	char *filtreTab = malloc(taille * sizeof(char));

	int index = 0;

	for (int i = 0; i < taille; i++) {
		if (!dejaPresent(tab[i], filtreTab, index)) {
			filtreTab[index] = tab[i];
			index++;
		}
	}

	filtreTab = realloc(filtreTab, index * sizeof(char));

	// On met à jour le nombre d'éléments dans le tableau
    *nb_element = index;

	return filtreTab;
}

// Renvoie vrai si l'automate A est déterministe
int check_deterministe(AUTOMATE A) {

	// On récupère l'alphabet de l'automate
	int newA_size = A.N + 1;
	AUTOMATE newA = creer_automate("newA", newA_size);
	int index = 0;
	char *tab = malloc(A.nb_trans * sizeof(char));

	newA = A;
	// On va parcourir les transitions de newA
	struct transition *auto_newA = newA.T;
	while (auto_newA) {
		// On ajoute la lettre de la transition dans le tableau
		tab[index] = auto_newA->a;
		index++;
		// On passe à la transition suivante
		auto_newA = auto_newA->suiv;
	}

	int tab_filtre_size;;
	// On va filtrer le tableau pour garder que les lettres uniques
	char *tab_filtre = filtrage(tab, index, &tab_filtre_size);

	// On va parcourir tous les états de A
	for (unsigned int q1 = 0; q1 < A.N; q1++) {
		// On va voir si pour chaque lettre, l'état courant à au plus une transition avec cette lettre et voir qu'il n'y a pas de transition epsilon
		for (int i = 0; i < tab_filtre_size; i++) {
			char curr_letter = tab_filtre[i]; // Lettre courante
			int cpt = 0; // Compteur pour le nombre de transitions avec la lettre courante
			struct transition *trans_lettre = A.T;
			while (trans_lettre) {
				// Si on trouve une transition avec l'état courant et la lettre courante, on incrémente le compteur
				if (trans_lettre->p == q1 && trans_lettre->a == curr_letter) {
					cpt++;
				}
				trans_lettre = trans_lettre->suiv;
			}
			// Si on a plus d'une transition avec la lettre courante, l'automate n'est pas déterministe
			if (cpt > 1) {
				return 0;
			}
		}
	}
	return 1;
}

// Fonction qui sera appelée si l'automate est déjà déterministe pour le rendre complet
AUTOMATE AFD_complet(AUTOMATE A) {

	int newA_size = A.N + 1;
	AUTOMATE newA = creer_automate("newA", newA_size);
	int index = 0;
	char *tab = malloc(A.nb_trans * sizeof(char));

	newA = A;
	// On va parcourir les transitions de newA
	struct transition *auto_newA = newA.T;
	while (auto_newA) {
		// On ajoute la lettre de la transition dans le tableau
		tab[index] = auto_newA->a;
		index++;
		// On passe à la transition suivante
		auto_newA = auto_newA->suiv;
	}

	int tab_filtre_size;;
	// On va filtrer le tableau pour garder que les lettres uniques
	char *tab_filtre = filtrage(tab, index, &tab_filtre_size);
	
	// On va ajouter un état poubelle à newA
	newA.N++;
	//printf("Nombre états: %d\n", newA.N);
	//printf("Nombre transitions: %d\n", newA.nb_trans);
	unsigned int poubelle = newA.N - 1;
	//printf("Ajout de l'état poubelle: %d\n", poubelle);
	// Pour chaque état de A
	for (unsigned int q1 = 0; q1 < newA.N; q1++) {
		// Pour chaque lettre de l'alphabet newtab
		for (int i = 0; i < tab_filtre_size; i++) {
			// On va regarder si l'état q1 possède une transition avec la lettre courante
			char curr_letter = tab_filtre[i]; // Lettre courante
			int possede_lettre = 0; // Booléen pour savoir si l'état possède la lettre courante
			struct transition *trans = A.T;
			while (trans) {
				// Si on trouve une transition avec l'état courant et la lettre courante
				if (trans->p == q1 && trans->a == curr_letter) {
					possede_lettre = 1; // On actualise le booléen
					break;
				}
				// On passe à la transition suivante
				trans = trans->suiv;
			}
			// Si l'état ne possède pas la lettre courante
			if (!possede_lettre) {
				// On ajoute une transition depuis l'état courant vers l'état poubelle avec la lettre courante
				newA = ajoute_une_transition(newA, q1, curr_letter, poubelle);
			}
		}
	}
	return newA;
}

// Structure pour une liste chaînée d'états
struct listeChainee {
	int clé;
	int size;
    struct listeChainee *suiv;
	unsigned int etat[];
};

//! A SUPPRIMER A LA FIN
// Fonction pour afficher les éléments de la liste 
void printList(struct listeChainee *head) {
    while (head != NULL) {
		printf("Clé: %d, ", head->clé);
		printf("Size : %d, ", head->size);
		printf("Etat: ");
        for (int i = 0; i < head->size; i++) {
			printf("%d, ", head->etat[i]);
        }
        printf("\n");
        head = head->suiv;
    }
}

// Libère la mémoire d'une liste chaînée d'ensemble d'états
void liberer_chaine(struct listeChainee *chaine) {
	struct listeChainee *curr = chaine;
    struct listeChainee *next;

    while (curr != NULL) {
        next = curr->suiv;
        free(curr);
        curr = next;
    }
}

//###
// 7. Déterminisation
//###
// ? https://www.geeksforgeeks.org/data-structures/linked-list/singly-linked-list/
// ? https://www.learn-c.org/en/Linked_lists
AUTOMATE determinise (AUTOMATE A) {

	// On vérifie si l'automate n'est pas déjà déterministe
	if (check_deterministe(A)) {
		A = AFD_complet(A);
		return A;
	}

	// On va récupérer l'alphabet de l'automate
	int index = 0;
	char *tab = malloc(A.nb_trans * sizeof(char));
	struct transition *auto_A = A.T;
	while (auto_A) {
		// On ajoute la lettre de la transition dans le tableau
		tab[index] = auto_A->a;
		index++;
		// On passe à la transition suivante
		auto_A = auto_A->suiv;
	}
	int tab_filtre_size;;
	// On va filtrer le tableau pour garder que les lettres uniques
	char *tab_filtre = filtrage(tab, index, &tab_filtre_size);

	// Affichage de l'alphabet de l'automate //! A SUPPRIMER DANS LA VERSION QUE L'ON REND
	printf("Alphabet: ");
	for (int i = 0; i < tab_filtre_size; i++) {
		printf("%c ", tab_filtre[i]);
	}
	printf("\n");

	int index_etat = 0; // Index pour le tableau d'états
	int index_cle = 0; // Index pour la clé
	struct listeChainee *liste = malloc(sizeof(struct listeChainee));
	// On va mettre l'état initial (0) dans liste->etat
	liste->etat[index_etat] = 0;
	liste->clé = index_cle;
	liste->suiv = NULL;
	liste->size = 1;
	index_etat++; // On incrémente l'index pour le tableau d'états
	index_cle++; // On incrémente l'index pour la clé

	// On va construire un nouvel automate qui sera déterministe
	AUTOMATE A_determinise = creer_automate("A_determinise", 0);

	// On va regarder pour chaque état dans liste->etat
	for (int i = 0; i < liste->size; i++) {
		// Affichage de l'ensemble d'états courant //! A SUPPRIMER DANS LA VERSION QUE L'ON REND
		printf("Ensemble courant: ");
		for (int k = 0; k < liste->size; k++) {
			printf("%d ", liste->etat[k]);
		}
		printf("\n");
		unsigned int q1 = liste->etat[i]; // On récupère l'état courant
		printf("Etat Q1: %d\n", q1);
		// On va regarder pour chaque lettre de l'alphabet
		for (int j = 0; j < tab_filtre_size; j++) {
			char curr_letter = tab_filtre[j]; // Lettre courante
			printf("	Lettre courante: %c\n", curr_letter);
			// On va mettre les états successeurs de q1 avec la lettre courante dans un tableau qui sera ensuite donné à liste->etat
			unsigned int *successeurs = malloc(A.N * sizeof(unsigned int));
			int index_succ = 0;
			struct transition *trans_lettre_courante = A.T;
			// Tant qu'il reste des transitions
			while (trans_lettre_courante) {
				// Si on trouve une transition avec etat_courant et lettre_courante
				if (trans_lettre_courante->p == q1 && trans_lettre_courante->a == curr_letter) {
					printf("		Transition (%d,%c,%d) matchée\n", trans_lettre_courante->p, affcar(trans_lettre_courante->a), trans_lettre_courante->q);
					successeurs[index_succ] = trans_lettre_courante->q;
					index_succ++;
				}
				trans_lettre_courante = trans_lettre_courante->suiv;
			}
			// On va regarder si notre tableau d'états successeurs n'est pas vide
			if (index_succ != 0) {
				// Affichage de l'ensemble d'états successeurs //! A SUPPRIMER DANS LA VERSION QUE L'ON REND
				printf("		Etat successeur: ");
				for (int k = 0; k < index_succ; k++) {
					printf("%d ", successeurs[k]);
				}
				printf("\n");
				// On va regarder si l'ensemble d'états successeurs est déjà présent dans liste->etat
				int est_present = 0; // Booléen pour savoir si l'ensemble d'états successeurs est déjà présent dans liste->etat
				struct listeChainee *curr = liste;
				while (curr != NULL) {
					// On va regarder si l'ensemble d'états successeurs est déjà présent dans liste->etat
					int cpt = 0; // Compteur pour savoir si l'ensemble d'états successeurs est déjà présent dans liste->etat
					for (int k = 0; k < index_succ; k++) {
						for (int l = 0; l < curr->etat[k]; l++) {
							if (successeurs[k] == curr->etat[l]) {
								cpt++;
							}
						}
					}
					// Si l'ensemble d'états successeurs est déjà présent dans liste->etat
					if (cpt == index_succ) {
						est_present = 1;
						break;
					}
					curr = curr->suiv;
				}
				// Si l'ensemble d'états successeurs n'est pas présent dans liste->etat
				if (!est_present) { //! problème surement ici
					printf("		Tableau d'états successeurs NON présent dans liste->etat\n");
					// On ajoute l'ensemble d'états successeurs à liste->etat
					struct listeChainee *newlisteChainee = malloc(index_succ * sizeof(struct listeChainee));
					memcpy(newlisteChainee->etat, successeurs, index_succ * sizeof(unsigned int));
					newlisteChainee->clé = index_cle;
					newlisteChainee->suiv = NULL;
					liste->suiv = newlisteChainee;
					index_etat++; // On incrémente l'index pour le tableau d'états
					index_cle++; // On incrémente l'index pour la clé
					printf("		Tableau d'états successeurs ajouté à liste->etat\n");
				}
				// On ajoute la transition (liste->clé, curr_letter, liste->suiv->clé) à A_determinise
				printf("		Ajout de la transition (%d,%c,%d) à A_determinise\n", liste->clé, curr_letter, liste->suiv->clé);
				printf("			Avec liste->clé: %d\n", liste->clé);
				printf("			Avec curr_letter: %c\n", curr_letter);
				printf("			Avec liste->suiv->clé: %d\n", liste->suiv->clé);
				//Affichage du tableau d'états successeurs pour la clé liste->suiv->clé //! A SUPPRIMER DANS LA VERSION QUE L'ON REND
				printf("				Etat successeur: ");
				for (int k = 0; k < index_succ; k++) {
					printf("%d ", successeurs[k]);
				}
				printf("\n");
				A_determinise.N = index_cle;
				A_determinise = ajoute_une_transition(A_determinise, liste->clé, curr_letter, liste->suiv->clé);
				// Il faut regarder si l'ensemble d'états successeurs contient un état final
				int est_final = 0; // Booléen pour savoir si l'ensemble d'états successeurs contient un état final
				for (int k = 0; k < index_succ; k++) {
					if (A.F[successeurs[k]]) {
						est_final = 1;
						break;
					}
				}
				// Si l'ensemble d'états successeurs contient un état final
				if (est_final) {
					// On ajoute liste->suiv->clé à A_determinise.F
					etat_final_ON(A_determinise, liste->suiv->clé);
					printf("		Etat final %d ajouté à A_determinise\n", liste->suiv->clé);
				}
				printf("		Transition ajoutée à A_determinise\n");

			}
			// On libère la mémoire du tableau d'états successeurs
			free(successeurs);
		}
		// On passe à l'ensemble d'états suivant
		liste = liste->suiv;

		// Affichage de la liste //! A SUPPRIMER DANS LA VERSION QUE L'ON REND
		printf("Liste:\n");
		printList(liste);
		printf("\n");
	}
	// On libère la mémoire de la liste chaînée / du tableau d'états / du tableau d'états filtré
	liberer_chaine(liste); free(tab); free(tab_filtre);

	// Nous devons rendre l'automate déterministe complet
	A_determinise = AFD_complet(A_determinise);
	char *temp = A.nom; // On sauvegarde le nom de l'automate
	A = A_determinise;
	A.nom = temp;
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
	char *recherche_mot = mot;
	// Tant qu'il reste des lettres dans le mot
	//printf("MOT A RECONNAITRE: %s\n", mot);
	while (recherche_mot[0]) { 
		struct transition *t = A.T;
		// Recherche de la transition (curr_etat, mot[0], q)
		while (t) {
			// Si on trouve la transition (curr_etat, mot[0], q)
			if (t->p == curr_etat && t->a == recherche_mot[0]) {
				//printf("	TRANSITION TROUVEE: (%d,%c,%d)\n", t->p, affcar(t->a), t->q);
				curr_etat = t->q;
				//printf("	ETAT COURANT: %d\n", curr_etat);
				break;
			}
			t = t->suiv;
		}
		// Si on ne trouve pas la transition (curr_etat, mot[0], q)
		if (!t) {
			printf("%s N'est PAS reconnu par %s\n", mot, A.nom);
			return retour;
		}
		recherche_mot++;
		//printf("	MOT RESTANT: %s\n", recherche_mot);
	}
	//printf("ETAT ARRIVEE APRES PARCOURS: %d\n", curr_etat);
	retour = A.F[curr_etat]; // Si l'état dans lequel est final alors le mot est reconnu, sinon non
	if (retour) printf("%s EST RECONNU PAR %s\n", mot, A.nom);
	else    	printf("%s N'est PAS reconnu par %s\n", mot, A.nom);
	return retour;
}