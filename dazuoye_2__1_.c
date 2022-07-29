#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct nonw {
	char word[80];
	int value;
	struct nonw *next;
} * nonlist[800000] = {};
struct stopw {
	char word[80];
	struct stopw *next;
} * slist[500000] = {};
struct dictw {
	char word[80];
	struct dictw *next;
} * dlist[100000] = {};
struct par {
	char s[20]; //章节号
	double value;  //总词频
	int order;  //序号
	double TNkd[100];
	double TFkd[100];
	double SIM;
} plist[50000] = {};

void createslist(FILE *fp);
void createdlist(FILE *pp);
void createlist(FILE *in);
int isnon(char *word);
int isdic(char *word);
unsigned int hash(char *str);
int comp(const void *e1, const void *e2);
int top = 0;
char k[100][80] = {}, k_num = 0; // k为关键词数组, k_num关键词个数
int m=0;
int DNk[100]= {}; //关键词出现的文档个数
double IDFk[100] = {};

int main(int argc, char *argv[]) {
	int N = 0;
	FILE *in = NULL, *fp = NULL, *out= NULL, *pp= NULL;
	in = fopen("article.txt", "r");
	pp = fopen("dictionary.txt", "r");
	out = fopen("results.txt", "w");
	fp = fopen("stopwords.txt", "r");
	int i = 0, j = 0;
	int len = 0;
	len = strlen(argv[1]);
	for (i = 0; i < len; i++)
		N = N * 10 + (argv[1][i] - '0');
	for (i = 2; i < argc; i++) {
		strcpy(k[j], argv[i]);
		j++;
	}
	k_num = j;
	createslist(fp);
	createdlist(pp);
	createlist(in);

	for (m = 0; m < k_num; m++) {
		//计算每个关键词出现次数
		if ( DNk[m] == 0) {
			IDFk[m] = 0;
			continue;
		}
		IDFk[m] = log10(top+1.0) - log10(DNk[m]); //计算逆文档频率
	}

	for (i = 0; i < top; i++) {
		for (m = 0; m < k_num; m++) {
			//计算每个单词k在每个文档d出现词频TFkd
			plist[i].TFkd[m] = 100 * plist[i].TNkd[m] / plist[i].value;
			plist[i].SIM += plist[i].TFkd[m] * IDFk[m];
		}
	}

	qsort(plist, top+2, sizeof(struct par), comp);

	for (i = 0; i < 5; i++) {
		if (fabs(plist[i].SIM)<0.0000000000000001)
			break;
		printf("%.6lf %d %s\n", plist[i].SIM, plist[i].order+1, plist[i].s);
	}
	int xx = 0;
	if( top + 1 > N)
		xx = N;
	else
		xx = top + 1;
	for (i = 0; i < xx; i++) {
		if (fabs(plist[i].SIM)<0.0000000000000001)
			break;
		fprintf(out, "%.6lf %d %s\n", plist[i].SIM, plist[i].order+1, plist[i].s);
	}

	fclose(fp);
	fclose(pp);
	fclose(in);
	fclose(out);
	return 0;
}

void createlist(FILE *in) {
	int c = 0;
	char word[80] = {}, *p = word;
	fscanf(in, "%s", &plist[top].s); //获取章节号

	while ((c = fgetc(in)) != EOF) {
		if (c >= 'a' && c <= 'z') {
			*p = c;
			p++;
		} else if (c >= 'A' && c <= 'Z') {
			*p = c + 'a' - 'A';
			p++;
		} else {
			if (p != word) {
				*p = 0;
				if (isnon(word) == 1 && isdic(word) == 1) {
					//不在停用表且在字典
					plist[top].value++;
					for (m = 0; m < k_num; m++) {
						//计算每个关键词出现次数
						if (strcmp(k[m], word) == 0)
							plist[top].TNkd[m]++;
					}
				}
				p = word;
			}

			if (c == '\f') {
				plist[top].order = top;
				for (m = 0; m < k_num; m++) {
					//累加每个关键词出现的文档个数
					if (plist[top].TNkd[m] != 0)
						DNk[m]++;
				}
				top++;

				fscanf(in, "%s", &plist[top].s); //获取章节号
			}
		}
	}

	plist[top].order = top; //最后一段没有\f
	for (m = 0; m < k_num; m++) {
		//累加每个关键词出现的文档个数
		if (plist[top].TNkd[m] != 0)
			DNk[m]++;
	}
}

int comp(const void *e1, const void *e2) {
	struct par *m1 = (struct par *)e1;
	struct par *m2 = (struct par *)e2;
	if (fabs(m1->SIM - m2->SIM) > 0.000000000000000001)
		return (m2->SIM - m1->SIM)>0.000000000000000001? 1 : -1;
	else
		return m1->order - m2->order;
}

void createslist(FILE *fp) {
	// 创建停用表
	char word[80] = {};
	while (fscanf(fp, "%s", word) != EOF) {
		int num = hash(word);
		struct stopw *r;
		r = slist[num];
		if (r == NULL) {
			slist[num] = (struct stopw *)malloc(sizeof(struct stopw));
			strcpy(slist[num]->word, word);
			slist[num]->next = NULL;
		} else {
			r = (struct stopw *)malloc(sizeof(struct stopw));
			strcpy(r->word, word);
			r->next = slist[num];
			slist[num] = r;
		}
	}
}

void createdlist(FILE *pp) {
	// 创建字典表
	char word[80] = {};
	while (fscanf(pp, "%s", word) != EOF) {
		int num = hash(word);
		struct dictw *r;
		r = dlist[num];
		if (r == NULL) {
			dlist[num] = (struct dictw *)malloc(sizeof(struct dictw));
			strcpy(dlist[num]->word, word);
			dlist[num]->next = NULL;
		} else {
			r = (struct dictw *)malloc(sizeof(struct dictw));
			strcpy(r->word, word);
			r->next = dlist[num];
			dlist[num] = r;
		}
	}
}

int isnon(char *word) {
	//不在停用表返回1
	int n1 = hash(word);
	struct stopw *te = slist[n1];
	while (te != NULL) {
		if (strcmp(te->word, word) == 0)
			return 0;
		te = te->next;
	}
	return 1;
}

int isdic(char *word) {
	//在字典返回1
	int n1 = hash(word);
	struct dictw *te = dlist[n1];
	while (te != NULL) {
		if (strcmp(te->word, word) == 0)
			return 1;
		te = te->next;
	}
	return 0;
}

unsigned int hash(char *str) {
	// hash表
	unsigned int h = 0;
	char *s = NULL;
	for (s = str; *s != '\0'; s++)
		h = 37 * h + *s;
	return h % 60000;
}


