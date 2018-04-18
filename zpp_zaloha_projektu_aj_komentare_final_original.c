// projekt2.c -- Projekt2 - Tezeus a Minotaurus
// Fedor Bozik, 19.4.2017 13:05:41

/*
  Globalne premenne: 
  vyska - na zaznamenanie vysky bludiska/mapy
  sirka - na zaznamenanie sirky bludiska/mapy
  **farba - zaznamenava vsetky farby ktore sme v bludisku pouzili
  rgb - na ulozenie farieb ktorymi ofarbujeme mapu
  mapa - uklada znaky mapy zo vstupu

  (1. a 4. uloha) - kresli_mapu - vunuluje filter, prechadza bludisko podla zadaneho K, 
  otvori subor,zapise hlavicku bmp suboru potom prechadza bludisko a rozlisuje medzi 
  minotaurom, tezeom, stenami a chodbami a potom uzavrie subor
  
  (2. uloha) - vypise obsah zoznamov, resp. vypise susednost vyznacnych bodov zo
  zoznamov vsetkych pismen ktore sa pouzili ale aj tie ktore sa nepouzili
  
  (3. uloha) - kresli_miestnosti - podobne ako kresli_mapu - ale vufarbuje miestnosti
  nahodnymi farbami namiesto vyfarbovania jednou farbou
  
  hladaj - hlada mozne cesty bludiskom od daneho vyznacneho bodu po dalsi vyznacny bod do 
  vsetkych moznych smerov - ak sa cesta nasla zapise ju a vrati 1 - ak sa cesta nenasla 
  vrati 0
  
  vypis_mapu je debugovacia funkcia na vypisanie vstupnej mapy pre kontrolu ci sa spravne 
  nacitala
  
  nacitaj_mapu - nacitava mapy zo vstupu az do konca vstupu, alokuje dynamicky 
  premennu mapa a premennu farba, vytvori zoznamy pre vsetky pismena abecedy, ktore 
  mozme pouzit v bludisku - vunuluje vysku a sirku - nacitava az do konca vstupu
  ak sa podarilo nacitat tak sa spravi inicializacia pre bludiska a prip. sa nacitava
  dalej
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#pragma pack(push, 1)
struct BitmapFileHeader {
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
};
#pragma pack(pop)

struct BitmapInfoHeader {
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
};

void write_head(FILE *f, int width, int height)
{
	if (width % 4 != 0 || height % 4 != 0)
	{
		printf("Chyba: Vyska a sirka nie su delitelne 4.\n");
		return;
	}
	struct BitmapInfoHeader bih;
	bih.biSize = sizeof(struct BitmapInfoHeader);
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biSizeImage = bih.biWidth * bih.biHeight * 3;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	struct BitmapFileHeader bfh;
	bfh.bfType = 0x4D42;
	bfh.bfSize = sizeof(struct BitmapFileHeader) + sizeof(struct BitmapInfoHeader) + bih.biSizeImage;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = sizeof(struct BitmapFileHeader) + bih.biSize;

	fwrite(&bfh, sizeof(struct BitmapFileHeader), 1, f);
	fwrite(&bih, sizeof(struct BitmapInfoHeader), 1, f);
}

void write_pixel(FILE *f, unsigned char b, unsigned char g, unsigned char r)
{
	fwrite(&r, 1, 1, f);
	fwrite(&g, 1, 1, f);
	fwrite(&b, 1, 1, f);
}

// Globalne premenne
int vyska, sirka, **farba, rgb[100][3], filter[100];
char **mapa;
struct Zoznam *z[26]; // Pre kazdy vyznacny bod chceme spajany zoznam susednych vyznacnych bodov

// Struktury pre spajane zoznamy
struct Prvok 
{
	char ciel; //znak
	struct Prvok *dalsi;
	int farba; //pojdeme po takejto farbe
};

struct Zoznam
{
	struct Prvok *prvy;
	char znak;
};

struct Zoznam *zoznam_vytvor(char znak)
{
	struct Zoznam *z = (struct Zoznam *)calloc(1, sizeof(struct Zoznam));
	z->znak = znak;
	return z;
};

struct Prvok *prvok_vytvor(char ciel, struct Prvok *dalsi, char farba)
{
	struct Prvok *p = (struct Prvok *)calloc(1, sizeof(struct Prvok));
	p->ciel = ciel;
	p->farba = farba;
	p->dalsi = dalsi;
	return p;
};

void zoznam_vloz(struct Zoznam *z, char ciel, char farba)
{
	struct Prvok *p = prvok_vytvor(ciel, z->prvy, farba);
	if (z->prvy == NULL)
	{
		z->prvy = p;
		z->prvy->dalsi = NULL;
		return;
	}
	p->dalsi = z->prvy;
	z->prvy = p;
};

void zoznam_vypis(struct Zoznam *z)
{
	struct Prvok *p = z->prvy;
	printf("%c:", z->znak);
	while (p != NULL)
	{
		printf(" %c", p->ciel);
		p = p->dalsi;
	}
	printf("\n");
};

void prechod(char vb, int k) // Prechod bludiskom
{
	if (k == 0)
		return;
	struct Prvok *p = z[vb -'A']->prvy;
	while (p)
	{
		filter[p->farba] = 1; 
		prechod(p->ciel, (k - 1)); // idem do vyznacneho bodu p->ciel
		p = p->dalsi; 
	}
}

int hladaj(int r, int s, int c) // Hladanie v bludisku
{
	if (r < 0 || r >= vyska || s < 0 || s >= sirka || mapa[r][s] != '.' || farba[r][s] == c)
		return 0;
	farba[r][s] = c;
	hladaj(r + 1, s, c);
	hladaj(r - 1, s, c);
	hladaj(r, s + 1, c);
	hladaj(r, s - 1, c);
	return 1;
};

void vypis_mapu() // Vypisuje mapu do STDOUT
{
	int i, j;
	for (i = 0; i < vyska; i++)
	{
		for (j = 0; j < sirka; j++)
			printf("%c", mapa[i][j]);
		printf("\n");
	}
}

void vypis_susednost() // Vypisuje susednosti - Tretia uloha
{
	int i;
	for (i = 0; i < 26; i++)
		zoznam_vypis(z[i]);
    printf("\n");
}

// Nacitanie bludiska zo vstupu
int nacitaj_mapu()
{
	int i, j, k = 1, q, result = 0;
	srand(time(NULL));

	for (i = 0; i < 100; i++)
		for (j = 0; j < 3; j++)
			rgb[i][j] = rand() % 256;

	mapa = (char **)calloc(30, sizeof(char*));
	for (i = 0; i < 30; i++)
		mapa[i] = calloc(30, sizeof(char));

	farba = (int **)calloc(30, sizeof(int*));
	for (i = 0; i < 30; i++)
		farba[i] = calloc(30, sizeof(int));

	for (i = 0; i < 26; i++)
		z[i] = zoznam_vytvor('A' + i);

	vyska = 0;
	sirka = 0;

	while (fgets(mapa[vyska], 30, stdin))
	{
		if (strlen(mapa[vyska]) <= 1)
		{
			if (vyska == 0 && sirka == 0)
			{
				result = 0;
				break;
			}
			else
				break;
		}
		vyska++;
		result = 1;
	}
	if (strlen(mapa[0]) == 30)
		sirka = 30;
	else
		sirka = strlen(mapa[0]) - 1;

	if(result)
	{
		for (i = 0; i < vyska; i++)
			for (j = 0; j < sirka; j++)
				if (hladaj(i, j, k))
					k++;

		for (q = 1; q < k; q++)
		{
			int bod[26];
			for (i = 0; i < 26; i++)
				bod[i] = 0;
			for (i = 0; i < vyska; i++)
				for (j = 0; j < sirka; j++)
					if (farba[i][j] == q)
					{
						if (mapa[i + 1][j] >= 'A' && mapa[i + 1][j] <= 'Z')
							bod[mapa[i + 1][j] - 'A'] = 1;
						if (mapa[i - 1][j] >= 'A' && mapa[i - 1][j] <= 'Z')
							bod[mapa[i - 1][j] - 'A'] = 1;
						if (mapa[i][j + 1] >= 'A' && mapa[i][j + 1] <= 'Z')
							bod[mapa[i][j + 1] - 'A'] = 1;
						if (mapa[i][j - 1] >= 'A' && mapa[i][j - 1] <= 'Z')
							bod[mapa[i][j - 1] - 'A'] = 1;
					}
			for (i = 0; i < 26; i++)
				for (j = 0; j < 26; j++)
					if (i != j && bod[i] && bod[j])
						zoznam_vloz(z[i], 'A' + j, q); // pridat: z[i] pridat znak j
		}
	}
	return result;
};

void kresli_mapu(char *nazov_suboru, int k) // Vykreslenie mapy - Prva a stvrta uloha
{
	int q, y, x;
	for (q = 0; q < 100; q++)
		filter[q] = 0;

	prechod('T', k);

	FILE *f = fopen(nazov_suboru, "wb");
	int w = 40 * sirka, h = 40 * vyska;
	write_head(f, w, h);

	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
		{
			int i = (h - y) / 40, j = x / 40;
			switch (mapa[i][j])
			{
			case 'T': write_pixel(f, 0, 0, 255); break; // Tezeus
			case 'X': write_pixel(f, 255, 0, 0); break; // Minotaurus
			case '#': write_pixel(f, 0, 0, 0); break; // Steny
			case '.':
				if (filter[farba[i][j]])
					write_pixel(f, 64, 255, 64);
				else write_pixel(f, 255, 255, 255);
				break;
			default: write_pixel(f, 128, 128, 128); break; // Vyznacne body na sedo
			}
		}
	fclose(f);
};

void kresli_miestnosti(char* nazov_suboru) //Ofarbi miestnosti - Druha uloha
{
	FILE *f = fopen(nazov_suboru, "wb");
	int w = 40 * sirka, h = 40 * vyska;
	write_head(f, w, h);

	int x, y;

	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
		{
			int i = (h - y) / 40, j = x / 40;
			int c = farba[i][j];
			switch (mapa[i][j])
			{
			case 'T': write_pixel(f, 0, 0, 255); break;
			case 'X': write_pixel(f, 255, 0, 0); break;
			case '#': write_pixel(f, 0, 0, 0); break;
			case '.':
				write_pixel(f, rgb[c][0], rgb[c][1], rgb[c][2]);
				break;
			default: write_pixel(f, 128, 128, 128); break;
			}
		}
	fclose(f);
}

int main(void)
{
	if (nacitaj_mapu() == 1)
	{
		kresli_miestnosti("mapa1_farby_miestnosti.bmp");
		kresli_mapu("mapa1_k0.bmp", 0);
		kresli_mapu("mapa1_k1.bmp", 1);
		kresli_mapu("mapa1_k2.bmp", 2);
		kresli_mapu("mapa1_k3.bmp", 3);
		kresli_mapu("mapa1_k4.bmp", 4);
		kresli_mapu("mapa1_k5.bmp", 5);
		kresli_mapu("mapa1_k6.bmp", 6);
		kresli_mapu("mapa1_k7.bmp", 7);
		kresli_mapu("mapa1_k8.bmp", 8);
		kresli_mapu("mapa1_k9.bmp", 9);
		vypis_susednost();
	}

	if (nacitaj_mapu() == 1)
	{
		vypis_susednost();
		kresli_miestnosti("mapa2_farby_miestnosti.bmp");
		kresli_mapu("mapa2_k0.bmp", 0);
		kresli_mapu("mapa2_k1.bmp", 1);
		kresli_mapu("mapa2_k2.bmp", 2);
		kresli_mapu("mapa2_k3.bmp", 3);
		kresli_mapu("mapa2_k4.bmp", 4);
		kresli_mapu("mapa2_k5.bmp", 5);
		kresli_mapu("mapa2_k6.bmp", 6);
		kresli_mapu("mapa2_k7.bmp", 7);
		kresli_mapu("mapa2_k8.bmp", 8);
		kresli_mapu("mapa2_k9.bmp", 9);
	}

	if (nacitaj_mapu() == 1)
	{
		kresli_mapu("mapa3_k9.bmp", 9);
		kresli_mapu("mapa3_k8.bmp", 8);
		kresli_mapu("mapa3_k7.bmp", 7);
		kresli_mapu("mapa3_k6.bmp", 6);
		kresli_miestnosti("mapa3_farby_miestnosti.bmp");
		kresli_mapu("mapa3_k5.bmp", 5);
		kresli_mapu("mapa3_k4.bmp", 4);
		kresli_mapu("mapa3_k3.bmp", 3);
		kresli_mapu("mapa3_k2.bmp", 2);
		vypis_susednost();
		kresli_mapu("mapa3_k1.bmp", 1);
		kresli_mapu("mapa3_k0.bmp", 0);
	}
  
    if (nacitaj_mapu() == 1)
	{
		kresli_mapu("mapa4_k0.bmp", 0);
		vypis_susednost();
		kresli_miestnosti("mapa4_farby_miestnosti.bmp");
		kresli_mapu("mapa4_k1.bmp", 1);
		kresli_mapu("mapa4_k2.bmp", 2);
		kresli_mapu("mapa4_k3.bmp", 3);
		kresli_mapu("mapa4_k4.bmp", 4);
		kresli_mapu("mapa4_k5.bmp", 5);
		kresli_mapu("mapa4_k6.bmp", 6);
		kresli_mapu("mapa4_k7.bmp", 7);
		kresli_mapu("mapa4_k8.bmp", 8);
		kresli_mapu("mapa4_k9.bmp", 9);
	}

	return 0;
}