LDFLAGS="-L/usr/local/opt/flex/lib"

run: main
	./main

main: automate.o main.o
	gcc -Wall -o main automate.o main.o
	clear

automate.o: automate.c automate.h
	gcc -c -Wall automate.c
	clear

main.o: main.c automate.h
	gcc -c -Wall main.c
	clear

main.c: regexp
	./regexp < test.1
	cat main.c

# Pour les fuites mémoires
valgrind: automate.c main.1.c
	gcc -g -ggdb3 automate.c main.1.c -o valgrind_check
# -g et -ggdb3 pour demander au compilateur de générer des informations de débogage dans le binaire
	valgrind --leak-check=full ./valgrind_check

# Pour le débogage
gdb: automate.c main.1.c
	gcc -g -ggdb3 automate.c main.1.c -o gdb_check
# -g et -ggdb3 pour demander au compilateur de générer des informations de débogage dans le binaire
	gdb ./gdb_check

exec: automate.c main.1.c
	gcc automate.c main.1.c -o main
	./main

# Commenter deux des trois lignes en fonction de l'ordi
# La deuxième et la troisième est normalement pour les MAC
# La première pour les autres systèmes
# il faut éventuellement remplacer -ll par -lfl
regexp: regexp.yy.c regexp.tab.c
	gcc -o $@ $^ -ly -ll
#	gcc -o $@ $^ ${LDFLAGS} -ly -lfl
#	gcc -mmacosx-version-min=13.1 -o $@ $^ ${LDFLAGS} -ly -lfl

regexp.tab.c: regexp.y
	bison -d --report=all $^
#	bison -d -v -g $^
# -d pour générer le fichier exo01.tab.h
# -v pour générer le fichier exo01.output
# -g pour générer le fichier exo01.vcg

regexp.yy.c: regexp.l
	flex  -o $@ $^

# Pour mettre à jour les packages
update_upgrade:
	sudo apt update
	sudo apt upgrade

install:
	sudo apt-get install -y libbison-dev

# Installer valgrind pour les fuites mémoires
install_valgrind:
	sudo apt install valgrind

# Installer gdb
install_gdb:
	sudo apt install gdb

clean:
	rm -f main
	rm -f gdb_check
	rm -f valgrind_check
	rm -f main.c
	rm -f *.o
	rm -f *.output
	rm -f *.tab.c *.tab.h
	rm -f *.yy.c
	rm -f regexp
	rm -f regexptest
	rm -rf ${LADIR}
	rm -f ${LADIR}.zip
	ls -als

reset:
	make clean
	clear

LADIR="IN520_Projet"
zip:
	rm -rf ${LADIR}
	mkdir ${LADIR}
	cp Makefile automate.h automate.c main.1.c ${LADIR}
#	cp regexp.l regexp.y ${LADIR}
	cp test.1 ${LADIR}
	rm -f ${LADIR}.zip
	zip -r ${LADIR}.zip ${LADIR}
	rm -rf ${LADIR}