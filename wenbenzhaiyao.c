// rrm 数据结构大作业 小样本demo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct nonw
{
	char word[50];
	int value;
	struct nonw *next;
} * nonlist[5000] = {};
struct stopw
{
	char word[50];
	struct stopw *next;
} * slist[5000] = {};
struct par
{
	char s[3000];
	int value;
	int order;
	char vword[300][50];
	int valuelist[300];
} plist[100000] = {};
void createslist(FILE *fp);
void createlist(FILE *in);
int isnon(char *word);
unsigned int hash(char *str);
int comp(const void *e1, const void *e2);
int top = 0, vtop = -1;
int main()
{
	int N;
	FILE *in, *fp, *out;
	in = fopen("article.txt", "r");
	out = fopen("results.txt", "w");
	fp = fopen("stopwords.txt", "r");
	scanf("%d", &N);
	createslist(fp);
	createlist(in);
	int i, j;
	for (i = 0; i < top; i++)
	{
		for (j = 0;; j++)
		{
			if (plist[i].vword[j][0] == 0)
				break;
			int n = plist[i].valuelist[j];
			struct nonw *tem = nonlist[n];
			while (strcmp(tem->word, plist[i].vword[j]) != 0)
				tem = tem->next;
			plist[i].value += tem->value;
		}
	}
	qsort(plist, top, sizeof(struct par), comp);
	for (i = 0; i < 5; i++)
	{
		char *s = plist[i].s;
		while (*s == ' ')
			s++;
		printf("%d %s\n", plist[i].value, s);
	}
	for (i = 0; i < N; i++)
	{
		char *s = plist[i].s;
		while (*s == ' ')
			s++;
		fprintf(out, "%d %s\n", plist[i].value, s);
	}
}
void createslist(FILE *fp)
{
	char word[30];
	while (fscanf(fp, "%s", word) != EOF)
	{
		int num = hash(word);
		struct stopw *r;
		r = slist[num];
		if (r == NULL)
		{
			slist[num] = (struct stopw *)malloc(sizeof(struct stopw));
			strcpy(slist[num]->word, word);
			slist[num]->next = NULL;
		}
		else
		{
			r = (struct stopw *)malloc(sizeof(struct stopw));
			strcpy(r->word, word);
			r->next = slist[num];
			slist[num] = r;
		}
	}
}
void createlist(FILE *in)
{
	char c;
	char word[50], *p = word, juzi[3000], *q = juzi;
	while ((c = fgetc(in)) != EOF)
	{
		if (c >= 'a' && c <= 'z')
		{
			*p = c;
			p++;
			*q = c;
			q++;
		}
		else if (c >= 'A' && c <= 'Z')
		{
			*p = c + 'a' - 'A';
			p++;
			*q = c;
			q++;
		}
		else
		{
			if (p != word)
			{
				*p = 0;
				if (isnon(word))
				{
					int num = hash(word);
					strcpy(plist[top].vword[++vtop], word);
					plist[top].valuelist[vtop] = num;
					struct nonw *r;
					r = nonlist[num];
					while (r != NULL)
					{
						if (strcmp(r->word, word) == 0)
						{
							r->value++;
							break;
						}
						r = r->next;
					}
					if (r == NULL)
					{
						r = (struct nonw *)malloc(sizeof(struct nonw));
						strcpy(r->word, word);
						r->next = nonlist[num];
						r->value = 1;
						nonlist[num] = r;
					}
				}
				p = word;
			}
			*q = c;
			q++;
			if (c == '!' || c == '.' || c == '?')
			{
				*q = 0;
				strcpy(plist[top].s, juzi);
				plist[top].order = top;
				q = juzi;
				vtop = -1;
				top++;
			}
		}
	}
}
int isnon(char *word)
{
	int n1 = hash(word);
	struct stopw *te = slist[n1];
	while (te != NULL)
	{
		if (strcmp(te->word, word) == 0)
			return 0;
		te = te->next;
	}
	return 1;
}
int comp(const void *e1, const void *e2)
{
	struct par *m1 = (struct par *)e1;
	struct par *m2 = (struct par *)e2;
	if (m1->value != m2->value)
		return m2->value - m1->value;
	else
		return m1->order - m2->order;
}
unsigned int hash(char *str)
{
	unsigned int h = 0;
	char *s;
	for (s = str; *s != '\0'; s++)
		h = 37 * h + *s;
	return h % 5000;
}
