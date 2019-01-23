build:
	gcc divvy_avl_analysis.c avl.c -o divvy_avl_analysis -std=c11 -Wall
clean:
	rm divvy_avl_analysis

run:
	clear
	./divvy_avl_analysis