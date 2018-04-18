// Struktury pre spajane zoznamy
struct Prvok 
{
	char znak, ciel;
	struct Prvok *dalsi;
};

struct Zoznam
{
	struct Prvok *prvy;
};

struct Zoznam *zoznam_vytvor(char znak)
{
	struct Zoznam *z = (struct Zoznam *)malloc(sizeof(struct Zoznam));
	z->prvy = NULL;
	return z;
};

struct Prvok *prvok_vytvor(char ciel, struct Prvok *dalsi, char farba)
{

};

void zoznam_vloz(struct Zoznam *z, char ciel, char farba)
{

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

// Pre kazdy vyznacny bod chceme spajany zoznam susednych vyznacnych bodov
struct Zoznam *z[26];