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

// Renvoie vrai si l'état est déjà présente dans le tableau
int dejaPresent_etat(unsigned int etat, unsigned int *tab, int taille) {
	for (int i = 0; i < taille; i++) {
		if (tab[i] == etat) {
			return 1;
		}
	}
	return 0;
}

// Filtre le tableau pour garder que les lettres uniques
char* filtrage_alphabet(char *tab, int taille, int *nb_element) {
	char *filtreTab = malloc(taille * sizeof(char));

	int index = 0;

	for (int i = 0; i < taille; i++) {
		if (!dejaPresent(tab[i], filtreTab, index)) {
			filtreTab[index] = tab[i];
			index++;
		}
	}

	// On redimensionne le tableau
	filtreTab = realloc(filtreTab, index * sizeof(char));

	// On met à jour le nombre d'éléments dans le tableau
    *nb_element = index;

	return filtreTab;
}

// Filtre le tableau pour garder que les états uniques
unsigned int* filtrage_etats(unsigned int *tab, int taille, int *nb_element) {
	unsigned int *filtreTab = malloc(taille * sizeof(char));

	int index = 0;

	for (int i = 0; i < taille; i++) {
		if (!dejaPresent_etat(tab[i], filtreTab, index)) {
			filtreTab[index] = tab[i];
			index++;
		}
	}

	// On redimensionne le tableau
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

	// Tant qu'il nous reste des transitions
	while (auto_newA) {
		// On ajoute la lettre de la transition dans le tableau
		tab[index] = auto_newA->a;
		index++;
		// On passe à la transition suivante
		auto_newA = auto_newA->suiv;
	}

	// On va filtrer le tableau pour garder que les lettres uniques
	int tab_filtre_size;
	char *tab_filtre = filtrage_alphabet(tab, index, &tab_filtre_size);

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
	free(tab);
	free(tab_filtre);
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
	char *tab_filtre = filtrage_alphabet(tab, index, &tab_filtre_size);
	
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
	
	// On libère la mémoire
	free(tab);
	free(tab_filtre);

	return newA;
}

// Structure pour une liste chaînée d'états
struct listeChainee {
	int clé;
	int size;
    struct listeChainee *suiv;
	unsigned int *etat;
};

// Fonction pour trier un tableau d'entiers
void triBulles(unsigned int *tableau, int taille) {
    unsigned int temp;

    for (int i = 0; i < taille - 1; i++) {
        for (int j = 0; j < taille - i - 1; j++) {
            // Comparaison et échange si l'élément actuel est plus grand que le suivant
            if (tableau[j] > tableau[j + 1]) {
                temp = tableau[j];
                tableau[j] = tableau[j + 1];
                tableau[j + 1] = temp;
            }
        }
    }
}

// Fonction pour ajouter un ensemble d'état dans une liste chaînée d'ensemble d'états
void ajouter_etat(struct listeChainee *liste, unsigned int *ens, int taille) {
	
	struct listeChainee *curr = liste;
	
	// Tant qu'on a pas atteint la fin de la liste
	while (curr->suiv != NULL) {
		curr = curr->suiv;
	}

	// On créee un nouvel élément
	struct listeChainee *nouveau = malloc(sizeof(struct listeChainee));
	nouveau->clé = curr->clé + 1;
	nouveau->size = taille;
	nouveau->etat = malloc(taille * sizeof(unsigned int));

	// Pour chaque élément de ens
	for (int i = 0; i < taille; i++) {
		nouveau->etat[i] = ens[i];
	}

	// On met à jour le pointeur suivant à NULL
	nouveau->suiv = NULL;

	// On met à jour le pointeur suivant de curr
	curr->suiv = nouveau;
}

//! A SUPPRIMER A LA FIN
// Fonction pour afficher les éléments de la liste 
void printList(struct listeChainee *head) {
    while (head != NULL) {
		printf("clé: %d, ", head->clé);
		printf("size : %d, ", head->size);
		printf("etat: {");
        for (int i = 0; i < head->size; i++) {
			if (i == head->size - 1) printf("%d", head->etat[i]);
			else printf("%d, ", head->etat[i]);
        }
		printf("}");
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



// Fonction qui renvoie un tableau contenant les états successeurs de l'ensemble d'états avec la lettre courante
unsigned int *tableauArrivee(AUTOMATE A, unsigned int *tab_depart, int taille_tab_depart, char lettre, int *taille_tab_arrivee) {
	unsigned int *tab_arrivee = malloc(A.N * sizeof(unsigned int));
	int index = 0;

	// Pour chaque état de tab_depart
	for (unsigned int q = 0; q < taille_tab_depart; q++) {
		// On va parcourir les transitions de A
		struct transition *trans_A = A.T;

		// Tant qu'il nous reste des transitions
		while (trans_A) {
			// Si on trouve une transition avec l'état courant et la lettre courante
			if (trans_A->p == q && trans_A->a == lettre) {
				tab_arrivee[index] = trans_A->q;
				index++;
			}
			trans_A = trans_A->suiv;
		}
	}

	// On redimensionne le tableau
	tab_arrivee = realloc(tab_arrivee, index * sizeof(unsigned int));

	// On met à jour le nombre d'éléments dans le tableau
	*taille_tab_arrivee = index;

	// On retourne le tableau
	return tab_arrivee;
}

// Fonction pour fusionner plusieurs tableaux et éliminer les doublons
void fusionnerTableaux(unsigned int *resultat, int *tailleResultat, unsigned int *tableaux[], int *tailles, int nombreDeTableaux) {

	// On regarde si notre tableau est de taille 1
	if (nombreDeTableaux == 1) {
		// On retourne simplement le tableau présent dans tableaux
		resultat = tableaux[0];
		*tailleResultat = tailles[0];
		return;
	}

    // On initiliase la taille du tableau résultat
    *tailleResultat = 0;

    // On fusionne les tableaux dans le tableau résultat en éliminant les doublons
    for (int k = 0; k < nombreDeTableaux; k++) {
        unsigned int *tableauCourant = tableaux[k];
        int tailleCourante = tailles[k];

        for (int i = 0; i < tailleCourante; i++) {
			unsigned int elementCourant = tableauCourant[i];
            int doublon = 0;

            // Vérification si l'élément courant existe déjà dans le tableau résultat
            for (int j = 0; j < *tailleResultat; j++) {
                if (resultat[j] == elementCourant) {
                    doublon = 1;
                    break;
                }
            }

            // Ajout de l'élément au tableau résultat s'il n'est pas un doublon
            if (!doublon) {
                resultat[*tailleResultat] = elementCourant;
                (*tailleResultat)++;
            }
        }
    }
}

int recherche_cle(struct listeChainee *liste, unsigned int *ens, int taille_ens) {
	struct listeChainee *curr = liste;
	int sauv; // Une sauvegarde au cas où ens n'est pas présent dans la liste

	while (curr != NULL) {
		// On vérifie d'abord que les tailles sont égales
		if (curr->size == taille_ens) {
			// On va regarder si l'ensemble d'états est déjà présent dans liste->etat
			int cpt = 0; // Compteur pour savoir si l'ensemble d'états successeurs est déjà présent dans liste->etat
			for (int k = 0; k < curr->size; k++) {
				for (int l = 0; l < curr->size; l++) {
					if (curr->etat[k] == ens[l]) {
						cpt++;
					}
				}
			}
			// Si on a trouvé l'ensemble d'états dans la liste
			if (cpt == taille_ens) {
				return curr->clé;
			}	
		}
		sauv = curr->clé;
		curr = curr->suiv;
	}
	// Si on arrive ici, c'est que l'ensemble d'états n'est pas présent dans la liste, on renvoie donc la clé précédente + 1
	return sauv + 1;
}

//###
// 7. Déterminisation
//###
// ? https://www.geeksforgeeks.org/data-structures/linked-list/singly-linked-list/
// ? https://www.learn-c.org/en/Linked_lists
// ? https://waytolearnx.com/2019/08/fusionner-deux-tableaux-en-c.html (fusionner deux tableaux)

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

	// On va filtrer le tableau pour ne garder que les lettres uniques
	int alphabet_size;
	char *alphabet = filtrage_alphabet(tab, index, &alphabet_size);

	// Affichage de l'alphabet de l'automate //! A SUPPRIMER DANS LA VERSION FINALE
	printf("Alphabet: ");
	for (int i = 0; i < alphabet_size; i++) {
		printf("%c ", alphabet[i]);
	}
	printf("\n");

	// On initialise un index pour la clé
	int index_cle = 0;

	// On va mettre l'état initial (0) dans liste->etat
	struct listeChainee *ens_etat = malloc(sizeof(struct listeChainee));
	unsigned int ens_temp[] = {0}; // On va mettre l'état initial (0) dans liste->etat
	ens_etat->etat = ens_temp;
	ens_etat->clé = index_cle;
	ens_etat->suiv = NULL;
	ens_etat->size = 1;

	// La liste chaînée ARCHIVES va contenir tous les ensembles d'états afin de l'utiliser pour des tests avant d'ajouter un nouvel ensemble d'états
	struct listeChainee *ARCHIVES = malloc(sizeof(struct listeChainee));
	ARCHIVES->etat = ens_temp;
	ARCHIVES->clé = index_cle;
	ARCHIVES->suiv = NULL;
	ARCHIVES->size = 1;

	// On incrémente l'index pour la clé
	index_cle++;

	// On va construire un nouvel automate qui sera déterministe
	AUTOMATE A_determinise = creer_automate(A.nom, 0);

	// Tant qu'il nous reste des ensembles d'états à traiter
	while (ens_etat) {
		// Affichage de l'ensemble d'états courant //! A SUPPRIMER DANS LA VERSION FINALE
		printf("Ensemble courant: {");
		for (int k = 0; k < ens_etat->size; k++) {
			if (k == ens_etat->size - 1) printf("%d}", ens_etat->etat[k]);
			else printf("%d, ", ens_etat->etat[k]);
		}
		printf("\n");

		// On récupère les informations de l'ensemble d'états courant
		unsigned int *curr_ens = ens_etat->etat;
		int curr_ens_size = ens_etat->size;
		int curr_clé = ens_etat->clé;

		// Pour chaque lettre de l'alphabet
		for (int i = 0; i < alphabet_size; i++) {
			char curr_letter = alphabet[i]; // Lettre courante
			printf("	Lettre courante: %c\n", curr_letter);

			// On va créer le tableau des successeurs trié de l'ensemble d'états courant avec la lettre courante en sauvegardant sa taille
			int taille_successeurs;
			unsigned int *successeurs = tableauArrivee(A, curr_ens, curr_ens_size, curr_letter, &taille_successeurs);

			// Affichage des informations du tableau successeurs //! A SUPPRIMER DANS LA VERSION FINALE
			printf("		Taille ensemble successeurs: %d\n", taille_successeurs);
			printf("		Ensemble successeurs: {");
			for (int k = 0; k < taille_successeurs; k++) {
				if (k == taille_successeurs - 1) printf("%d}", successeurs[k]);
				else printf("%d, ", successeurs[k]);
			}
			printf("\n");

			// On va regarder si notre tableau d'états successeurs n'est pas vide
			if (taille_successeurs != 0) {

				// On va recherche la bonne clé pour l'ensemble d'états successeurs
				int clé_succ = recherche_cle(ARCHIVES, successeurs, taille_successeurs);
				printf("		Obtention de la clé de l'ensemble successeurs: %d\n", clé_succ);

				// Nous insérons l'ensemble d'états successeurs si ce n'est pas déjà fait
				if (clé_succ == index_cle) {

					// On ajoute l'ensemble d'états successeurs à liste->etat
					ajouter_etat(ens_etat, successeurs, taille_successeurs);

					// On ajoute l'ensemble d'états successeurs à ARCHIVES
					ajouter_etat(ARCHIVES, successeurs, taille_successeurs);

					// On incrémente l'index pour la clé
					index_cle++;
					printf("	Incrémentation -> index clé: %d\n", index_cle);
				}

				// On met à jour le nombre d'états de A_determinise
				if (A_determinise.N < clé_succ) A_determinise.N = clé_succ + 1;

				printf("		Nombre d'états: %d\n", A_determinise.N);
				printf("		Ajout de la transition (%d,%c,%d)\n", curr_clé, curr_letter, clé_succ);

				// On ajoute la transition (liste->clé, curr_letter, cle) à A_determinise
				A_determinise = ajoute_une_transition(A_determinise, curr_clé, curr_letter, clé_succ); 
				printf("	Transition ajoutée\n");

				// Vérification si l'ensemble d'états successeurs contient un état final
				for (int k = 0; k < taille_successeurs; k++) {
					if (A.F[successeurs[k]]) {
						etat_final_ON(A_determinise, clé_succ);
						break;
					}
				}

				// Affichage des listes //! A SUPPRIMER DANS LA VERSION FINALE
				printf("\n");
				printf("	-> ens_etat:\n");
				printList(ens_etat);
				printf("\n");
				printf("	-> ARCHIVES:\n");
				printList(ARCHIVES);
				printf("\n");
			}

			// On libère la mémoire
			free(successeurs);
		}

		// On passe à l'ensemble d'états suivant
		ens_etat = ens_etat->suiv;
	} 
	
	// On libère la mémoire de la liste chaînée / du tableau d'états / du tableau d'états filtré
	liberer_chaine(ens_etat); free(tab); free(alphabet);

	// Nous devons rendre l'automate déterministe complet
	printf("NOWAY C LA FIN\n");
	A_determinise = AFD_complet(A_determinise);
	printf("NOWAY C LA FIN\n");
	return A_determinise;
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