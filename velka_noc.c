
// Velka noc

#include <stdio.h>
#include <stdlib.h>

#define MAXN 50

/* (1): do dvojrozmerneho pola d sa ukladaju vzdialenosti 
od kazdeho domu k ostatnym domom, n je pocet domov, 
ktory zadame na vstupe*/
int d[MAXN][MAXN], n; // n - kolko domov existuje
/* (2): do pola visited sa uklada informacia v ktorych 
domoch sme uz boli, nv je kolko domov sme uz presli*/
int visited[MAXN], nv; // nv - kolko domov sa preslo - aktualne
/* (3): do premennej best sa uklada najkratsia vzdialenost, 
do len sa pripocitava vzdialenost ked chodime po domoch */
int best, len; //best - najkratsia cesta  a len - aktualna dlzka

/* (4): rekurzivna funkcia, ktora najde najkratsiu 
cestu medzi domami, v oznacuje v ktorom dome prave sme */
void find(int v) //v - domcek v ktorom sme aktualne
{
  int i; 
  // (5): podmienka ci sme navstivili vsetky domy
  if (nv == n)
  {
	/* (6): ak je aktualna dlzka cesty kratsia nez doterajsia najkratsia, 
	alebo je toto prva cesta, tak ulozime ako najkratsiu cestu */
    if (best < 0 || best > len+d[v][0]) 
      best = len+d[v][0];
    return;
  }
  
  /* (7): prechadzame z aktualneho domu vsetky cesty okrem prvej, 
  pretoze prvou sa vraciame*/
  for (i = 1; i < n; i++)
    if (visited[i] == 0)
    {
      /* (8): pokracujeme v hladani ak hladame prvu cestu, 
	  alebo kym doterajsia cesta je kratsia nez najkratsia ulozena */
      if (best < 0 || best > len)
      {
        // (9): nastavime ze sme navsivli dom do ktoreho ideme
        visited[i] = 1; // (10): nastavime ze dom do ktoreho ideme sme uz navstivili
        nv++;
        len += d[v][i]; // (11): pripocitame cestu do celkovej aktualnej cesty

        find(i); // (12): pokracujeme v hladani cesty od dalsieho domu
 
        // (13): tu sa vraciame naspat a vymazeme navstevu posledneho domu
        len -= d[v][i]; // (14): odcitame vzdialenost do domu v ktorom sme boli
        nv--;
        visited[i] = 0; // (15): oznacime ze sme v danom dome este neboli
      }
    }
}

int main()
{
  int i,j;

  // (16): nacitame pocet domov a vzdialenosti k jednotlivym domom
  while (scanf("%d", &n) > 0)
  {
    // (17): nacitame vzdialenosti k domom
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
        scanf("%d", &d[i][j]);
    for (i = 0; i < n; i++)
      visited[i] = 0;
    nv = 1; /* (18): nastavime pocet navstivenych na 1, 
	pretoze uz sme v dome, kde zaciname */
    visited[0] = 1;

    len = 0; // (19): vynulujeme aktualnu vzdialenost
    best = -1; // (20): najkratsiu cestu nastavime na -1
    find(0); // (21): zaciname hladat v prvom dome, pretoze sa v nom nachadzame
    printf("%d\n", best); // (22): vypiseme najkratsiu cestu
  }
  
  return 0;
}
