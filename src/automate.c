#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "automate.h"

//###
// 1. Gestion des états finaux
//###
AUTOMATE etat_final_ON    (AUTOMATE A, unsigned int p) {if (p>=A.N) exit (41); A.F[p] = 1; return A;}
AUTOMATE etat_final_OFF   (AUTOMATE A, unsigned int p) {if (p>=A.N) exit (42); A.F[p] = 0; return A;}
AUTOMATE etat_final_TOGGLE (AUTOMATE A, unsigned int p) {if (p>=A.N) exit (43); A.F[p] = 1-A.F[p]; return A;}


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
//		fprintf(stderr,"WARNING 001: Transition (%d,%c,%d) déjà présente\n",t->p,affcar(t->a),t->q); 
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
	t->suiv = NULL;
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
	// Méthodes supplémentaires
	//###

// Renvoie vrai si la lettre est déjà présente dans le tableau
int dejaPresent(char lettre, char *tab, int taille) {
	// Parcours du tableau
	for (int i = 0; i < taille; i++) {

		// Si on trouve la lettre dans le tableau, on renvoie vrai
		if (tab[i] == lettre) {
			return 1;
		}
	}
	// Sinon on renvoie faux
	return 0;
}

// Structure pour une liste chaînée d'états
struct listeChainee {
	int cle;
	int size;
	unsigned int *etat;
    struct listeChainee *suiv;
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
	nouveau->cle = curr->cle + 1;
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

// Libère la mémoire d'une liste chaînée d'ensemble d'états
void liberer_chaine(struct listeChainee *chaine) {
	struct listeChainee *tmp;

    while (chaine != NULL) {
        tmp = chaine;
        chaine = chaine->suiv;
        free(tmp);
    }
	free(chaine);
}

// Fonction recherchant la clé d'un ensemble d'états dans une liste chaînée (si l'ensemble d'états n'est pas présent, on renvoie la clé précédente + 1)
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
				return curr->cle;
			}	
		}
		sauv = curr->cle;
		curr = curr->suiv;
	}
	free(curr);
	
	// Si on arrive ici, c'est que l'ensemble d'états n'est pas présent dans la liste, on renvoie donc la clé précédente + 1
	return sauv + 1;
}

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
	free(t);
	// Si on arrive ici alors la transition n'est pas présente dans A
	return 0;
}

// Recherche les transitions non epsilon pour avoir la transition (q1,a,q3) si elle n'existe pas déjà avec a != epsilon
void recherche_transition_non_epsilon (AUTOMATE A, unsigned int q1, unsigned int q2) { 

	// On va parcourir les transitions de A
	struct transition *parcours = A.T;
	while (parcours) {

		// Si on trouve la transition (q1,a,q2) avec a != epsilon
		if (parcours->p == q2 && parcours->a != 0) {

			// On vérifie que la transition n'est pas dèjà présente
			if (!transition_presente(A, q1, parcours->a, parcours->q)) {

				A = ajoute_une_transition(A, q1, parcours->a, parcours->q);
				//printf("			Ajout (%d,%c,%d)\n", q1, affcar(parcours->a), parcours->q);
			}

			// On regarde si q2 est un état final pour modifier q1 si besoin
			if (A.F[q2]) {
				etat_final_ON(A, q1);
			}
		} else if (parcours->p == q2 && parcours->a == 0) {
			recherche_transition_non_epsilon(A, q1, parcours->q);
			//printf("			Recherche (%d,%c,%d)\n", q1, affcar(parcours->a), parcours->q);
		}
		// Sinon on passe à la transition suivante
		parcours = parcours->suiv;
	}
	free(parcours);
}

//###
// 6. Suppression des epsilon-transitions
//###

AUTOMATE supprime_epsilon_transitions (AUTOMATE A) {

	// Pour chaque état de A
	for (unsigned int q1 = 0; q1 < A.N; q1++) {
		//printf("q1: %d\n", q1);

		// Parcours des transitions de A
		struct transition *A_t = A.T;
		//printf("	Parcours des transitions de A\n");
		while (A_t) {

			// Nous allons distinguer 3 cas:

				// Cas 1: la transition a pour état de départ q1 et la lettre est epsilon
				// Cas 2: la transition a pour état de départ q1 et la lettre n'est pas epsilon
				// Cas 3: la transition n'a pas pour état de départ q1 

				// Le cas 2 et 3 ne nous intéresse pas car nous passons simplement à la transition suivante

			// Cas 1
			if (A_t->p == q1 && A_t->a == 0) {

				//printf("	(%d,%c,%d)\n", A_t->p, affcar(A_t->a), A_t->q);

				// On doit rechercher maintenant les transitions non-epsilon que l'on va ajouter à A pour l'état q1
				
				// On initialise l'état q2 que l'on va actualiser dans le cas d'une transition epsilon
				unsigned int q2 = A_t->q; 
				//printf("		q2: %d\n", q2);

				recherche_transition_non_epsilon(A, q1, q2);

			}
			A_t = A_t->suiv;
		}
		free(A_t);
	}

	// On va parcourir les transitions de A pour trouver les transitions epsilon et les supprimer
	struct transition *supp_e = A.T;
	struct transition *prev = NULL;
	// On va compter le nombre de transitions non epsilon
	int cpt = 0;
	while (supp_e) {

		// Cas: c'est une transition epsilon
		if (supp_e->a == 0) {
			// On prend la transition suivante
			struct transition *next = supp_e->suiv;
			// On libère la mémoire de la transition epsilon
			free(supp_e);

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
			cpt++;
		}
	}
	free(supp_e);

	A.nb_trans = cpt; // On met à jour le nombre de transitions car dans la méthode recherche_transition_non_epsilon, les transitions ajoutées ne sont pas comptées

	return A;
}

//###
// 7. Déterminisation
//###

AUTOMATE determinise (AUTOMATE A) {

	// On va récupérer l'alphabet de l'automate
	int alphabet_size = 0;
	char *alphabet = calloc(26, sizeof(char));

	// On doit tester la valeur de retour de calloc
	if (!alphabet) {
		printf("Erreur lors de l'allocation mémoire: alphabet\n");
		exit(1);
	}

	struct transition *auto_A = A.T;

	while (auto_A) {
		
		// On vérifie que la lettre n'est pas déjà présente dans le tableau
		if (!dejaPresent(auto_A->a, alphabet, alphabet_size)) {
			// On ajoute la lettre de la transition dans le tableau
			alphabet[alphabet_size] = auto_A->a;
			alphabet_size++;
		}
		
		// On passe à la transition suivante
		auto_A = auto_A->suiv;
	}
	free(auto_A);

	// Si l'alphabet est vide nous sommes dans le cas d'un automate sans transition, dans ce cas on renvoie l'automate A
	if (alphabet_size == 0) {
		return A;
	}

	alphabet = realloc(alphabet, alphabet_size * sizeof(char));

	// On doit tester la valeur de retour de realloc
	if (!alphabet) {
		printf("Erreur lors de la réallocation mémoire: alphabet\n");
		exit(1);
	}

	// On initialise un index pour la clé
	int index_cle = 0;

	// On va mettre l'état initial (0) dans liste->etat
	struct listeChainee *a_traiter = malloc(sizeof(struct listeChainee));

	// On doit tester la valeur de retour de malloc
	if (!a_traiter) {
		printf("Erreur lors de l'allocation mémoire: a_traiter\n");
		exit(1);
	}
	
	a_traiter->etat = malloc(sizeof(unsigned int));

	// On doit tester la valeur de retour de malloc
	if (!a_traiter->etat) {
		printf("Erreur lors de l'allocation mémoire: a_traiter->etat\n");
		exit(1);
	}

	a_traiter->etat[0] = 0;
	a_traiter->cle = index_cle;
	a_traiter->suiv = NULL;
	a_traiter->size = 1;

	// La liste chaînée ARCHIVES va contenir tous les ensembles d'états afin de l'utiliser pour des tests avant d'ajouter un nouvel ensemble d'états
	struct listeChainee *ARCHIVES = malloc(sizeof(struct listeChainee));

	// On doit tester la valeur de retour de malloc
	if (!ARCHIVES) {
		printf("Erreur lors de l'allocation mémoire: ARCHIVES\n");
		exit(1);
	}

	ARCHIVES->etat = malloc(sizeof(unsigned int));

	// On doit tester la valeur de retour de malloc
	if (!ARCHIVES->etat) {
		printf("Erreur lors de l'allocation mémoire: ARCHIVES->etat\n");
		exit(1);
	}

	ARCHIVES->etat[0] = 0;
	ARCHIVES->cle = index_cle;
	ARCHIVES->suiv = NULL;
	ARCHIVES->size = 1;

	// On incrémente l'index pour la clé
	index_cle++;

	// On va construire un nouvel automate qui sera déterministe
	AUTOMATE A_determinise = creer_automate(A.nom, 1);

	// Tant qu'il nous reste des ensembles d'états à traiter
	while (a_traiter) {

		// On récupère les informations de l'ensemble d'états courant
		unsigned int *curr_ens = a_traiter->etat;
		int curr_ens_size = a_traiter->size;
		int curr_clé = a_traiter->cle;

		// Pour chaque lettre de l'alphabet
		for (int i = 0; i < alphabet_size; i++) {

			char curr_letter = alphabet[i]; // Lettre courante

			// On va créer le tableau des successeurs trié de l'ensemble d'états courant avec la lettre courante en sauvegardant sa taille
			unsigned int *successeurs = malloc(A.N * sizeof(unsigned int));

			// On doit tester la valeur de retour de malloc
			if (!successeurs) {
				printf("Erreur lors de l'allocation mémoire: successeurs\n");
				exit(1);
			}

			int index = 0;

			// On va parcourir la longueur du tableau d'états de départ
			for (unsigned int i = 0; i < curr_ens_size; i++) {
				
				// On prend l'état à l'indice i du tableau
				unsigned curr_etat = curr_ens[i];

				// On va parcourir les transitions de A
				struct transition *trans_A = A.T;

				// Tant qu'il nous reste des transitions
				while (trans_A) {

					// Si on trouve une transition avec l'état courant et la lettre courante
					if (trans_A->p == curr_etat && trans_A->a == curr_letter) {
						successeurs[index] = trans_A->q;
						index++;
					}
					trans_A = trans_A->suiv;
				}
			}

			// On redimensionne le tableau (plus précisement on réduit l'espace mémoire alloué)
			successeurs = realloc(successeurs, index * sizeof(unsigned int));
			
			// On sauvgarde le nombre d'éléments dans le tableau
			int taille_successeurs;
			taille_successeurs = index;

			// On va trier dans l'ordre croissant le tableau
			triBulles(successeurs, taille_successeurs);

			// On va regarder si notre tableau d'états successeurs n'est pas vide
			if (taille_successeurs != 0) {

				// On va recherche la bonne clé pour l'ensemble d'états successeurs
				int clé_succ = recherche_cle(ARCHIVES, successeurs, taille_successeurs);

				// Nous insérons l'ensemble d'états successeurs si ce n'est pas déjà fait
				if (clé_succ == index_cle) {

					// On ajoute l'ensemble d'états successeurs à a_traiter->etat
					ajouter_etat(a_traiter, successeurs, taille_successeurs);

					// On ajoute l'ensemble d'états successeurs à ARCHIVES
					ajouter_etat(ARCHIVES, successeurs, taille_successeurs);

					// On incrémente l'index pour la clé
					index_cle++;
				}

				// On met à jour le nombre d'états de A_determinise
				A_determinise.N = index_cle;

				//printf("		Nombre d'états: %d\n", A_determinise.N);
				// On ajoute la transition (curr_clé, curr_letter, clé_succ) à A_determinise
				A_determinise = ajoute_une_transition(A_determinise, curr_clé, curr_letter, clé_succ);
				//printf("		Transition ajoutée: (%d,%c,%d)\n", curr_clé, affcar(curr_letter), clé_succ);

				// Vérification si l'ensemble d'états successeurs contient un état final
				for (int k = 0; k < taille_successeurs; k++) {

					// Si on trouve un état final dans l'ensemble d'états successeurs
					if (A.F[successeurs[k]]) {
						etat_final_ON(A_determinise, clé_succ);
						break;
					} else etat_final_OFF(A_determinise, clé_succ);
				}
			}

			// On libère la mémoire
			free(successeurs);
		}

		// On passe à l'ensemble d'états suivant
		a_traiter = a_traiter->suiv;
	}

	// On libère la mémoire
	liberer_chaine(a_traiter);

	// Transformation de l'automate déterminisé en AFD complet
	A_determinise.N++;
	unsigned int poubelle = A_determinise.N - 1;

	// Comme nous allons ajouter un état poubelle, on doit modifier la taille de l'allocation mémoire de A_determinise.F
	A.F = realloc(A.F, A_determinise.N * sizeof(int));

	// On doit tester la valeur de retour de realloc
	if (!A.F) {
		printf("Erreur lors de la réallocation mémoire: A.F\n");
		exit(1);
	}

	etat_final_OFF(A_determinise, poubelle); // L'état poubelle n'est pas final

	// Pour chaque état de A_determinise
	for (unsigned int q_depart = 0; q_depart < A_determinise.N; q_depart++) {

		// Pour chaque lettre de l'alphabet
		for (int i = 0; i < alphabet_size; i++) {

			char curr_letter = alphabet[i];

			// On va voir si la transition avec comme état de départ q et comme lettre alphabet_filtre[i] existe
			int manquante = 1;

			struct transition *check = A_determinise.T;
			while (check) {

				// Si on trouve une transition avec l'état courant et la lettre courante
				if (check->p == q_depart && check->a == curr_letter) {
					manquante = 0;
					break;
				}
				check = check->suiv;
			}

			// On vérifie maintenant si la transition existe
			if (manquante) {
				// On ajoute la transition (q, curr_letter, poubelle)
				A_determinise = ajoute_une_transition(A_determinise, q_depart, curr_letter, poubelle);
			}

		}
	}

	// On libère la mémoire de la liste chaînée / de l'alphabet
	liberer_chaine(a_traiter);
	free(alphabet);

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

	// On regarde d'abord si on veut reconnaître le mot vide (epsilon)
	if (strcmp(mot, "E") == 0) {
		retour = A.F[0];
		if (retour) printf("%s EST RECONNU PAR %s\n", mot, A.nom);
		else printf("%s N'est PAS reconnu par %s\n", mot, A.nom);
		return retour;
	}
	
	unsigned int pos = 0; // Pointeur pour sauvegarder notre position dans l'automate

	int index = 0; // Index pour parcourir le mot
	int mot_size = strlen(mot);

	// Tant qu'il reste des lettres dans le mot
	while (index < mot_size) {
		char lettre = mot[index];
		struct transition *t = A.T;

		// Recherche de la transition (pos, mot[0], q)
		while (t) {

			// Si on trouve la transition (pos, mot[0], q)
			if (t->p == pos && t->a == lettre) {
				pos = t->q;
				break;
			}
			t = t->suiv;

			// On vérifie si on se trouve sur l'état poubelle
			if (pos == A.N - 1) {
				// On regarde si on veut reconnaître le mot vide, on adapte le message en fonction
				printf("%s N'est PAS reconnu par %s\n", mot, A.nom);
				return retour;
			}
		}
		
		// Si on arrive ici, c'est que la transition (pos, mot[0], q) n'existe pas
		if (!t) {
			// On regarde si on veut reconnaître le mot vide, on adapte le message en fonction
			printf("%s N'est PAS reconnu par %s\n", mot, A.nom);
			return retour;
		}
		index++;
	}

	retour = A.F[pos]; // Si l'état dans lequel est final alors le mot est reconnu, sinon non
	if (retour) {
		printf("%s EST RECONNU PAR %s\n", mot, A.nom);
		
	} else {
		printf("%s N'est PAS reconnu par %s\n", mot, A.nom);
	}
	return retour;
}