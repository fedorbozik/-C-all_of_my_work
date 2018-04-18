// uloha-2.c -- Uloha 2
// Fedor Bozik, 14.3.2017 17:51:40

#include <stdio.h>
#include <stdlib.h>

// Utriedi n cisel v poli a
void utried(int *a, int n)
{
  // sem napis svoje riesenie
  int i, j, k = 0;
  
  int *pole = (int*)calloc(n + 1, sizeof(int));
  
  for(i = 0; i < n; i++)
	pole[a[i]]++;
  
  for(i = 0; i <= n; i++)
	if(pole[i] > 0)
    	for(j = 0; j < pole[i]; j++)
       		a[k++] = i;
}


int main(void)
{
  int i, *x, n;

  scanf("%d", &n);
  x = (int*)malloc(n * sizeof(int));
  for (i = 0; i < n; i++)
    scanf("%d", &x[i]);

  utried(x, n);
  
  printf("%d", x[0]);
  for (i = 1; i < n; i++)
  {
    printf(" %d", x[i]);
    if (x[i-1] > x[i])
    {
      printf(" -- CHYBA\n");
      return 0;
    }
  }
  printf("\n");

  printf("OK\n");
  return 0;
}