#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct nonw
{
    char word[30];
    int value;
    struct nonw *next;
} * nonlist[500000] = {};
struct stopw
{
    char word[30];
    struct stopw *next;
} * slist[6000] = {};
struct dictw
{
    char word[30];
    struct dictw *next;
} * dlist[500000] = {};
struct par
{
    char s[10]; //章节号
    int value;  //总词频
    int order;  //序号
    int TNkd[100];
    double TFkd[100];
    double SIM;
    char vword[300][30];
    int valuelist[300];
} plist[1000] = {};

void createslist(FILE *fp);
void createdlist(FILE *pp);
void createlist(FILE *in);
int isnon(char *word);
int isdic(char *word);
unsigned int hash(char *str);
int comp(const void *e1, const void *e2);
int top = 0, vtop = -1;
char k[100][30], k_num = 0, s[10], ss[50]; // k为关键词数组, k_num关键词个数
char key[100][30];
char trash;
int m;
int DNk[100]; //关键词出现的文档个数
int IDFk[100];

int main(int argc, char *argv[])
{
    int N = 0;
    FILE *in, *fp, *out, *pp;
    in = fopen("article.txt", "r");
    pp = fopen("dictionary.txt", "r");
    out = fopen("results.txt", "w");
    fp = fopen("stopwords.txt", "r");

    int i, j = 0;
    int len;
    len = strlen(argv[1]);
    for (i = 0; i < len; i++)
        N = N * 10 + (argv[1][i] - '0');
    for (i = 1; i < argc; i++)
    {
        strcpy(k[j], argv[i]);
        j++;
    }
    createslist(fp);
    createdlist(pp);
    createlist(in);
    for (m = 0; m < k_num; m++)
    {
        //计算每个关键词出现次数
        IDFk[m] = log10(k_num) - log10(DNk[m]); //计算逆文档频率
    }

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
        for (m = 0; m < k_num; m++)
        {
            //计算每个单词k在每个文档d出现词频TFkd
            plist[i].TFkd[m] = (double)plist[i].TNkd[m] / plist[i].value;
            plist[i].SIM += plist[i].TFkd[m] * IDFk[m];
        }
    }

    qsort(plist, top, sizeof(struct par), comp);

    for (i = 0; i < 5; i++)
    {
        printf("%.6lf %d %s\n", plist[i].SIM, plist[i].order, plist[i].s);
    }
    for (i = 0; i < N; i++)
    {
        fprintf(out, "%.6lf %d %s\n", plist[i].SIM, plist[i].order, plist[i].s);
    }

    fclose(fp);
    fclose(pp);
    fclose(in);
    fclose(out);
}

void createlist(FILE *in)
{
    int c;
    char word[50], *p = word, juzi[1500], *q = juzi;
    fscanf(in, "%s", &plist[top].s); //获取章节号

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
                if (isnon(word) == 1 && isdic(word) == 1)
                {
                    //不在停用表且在字典
                    for (m = 0; m < k_num; m++)
                    {
                        //计算每个关键词出现次数
                        if (strcmp(k[m], word) == 0)
                            plist[top].TNkd[m]++;
                    }
                    int num = hash(word);
                    strcpy(plist[top].vword[++vtop], word);
                    plist[top].valuelist[vtop] = num;
                    struct nonw *r;
                    r = nonlist[num];
                    while (r != NULL)
                    {
                        if (strcmp(r->word, word) == 0)
                        {
                            r->value++; //频次
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
            if (c == '\f')
            {
                *q = 0;
                // strcpy(plist[top].s, juzi);
                plist[top].order = top;
                q = juzi;
                vtop = -1;

                for (m = 0; m < k_num; m++)
                {
                    //累加每个关键词出现的文档个数
                    if (plist[top].TNkd[m] != 0)
                        DNk[m]++;
                }
                top++;

                fscanf(in, "%s", &plist[top].s); //获取章节号
            }
        }
    }
}

int comp(const void *e1, const void *e2)
{
    struct par *m1 = (struct par *)e1;
    struct par *m2 = (struct par *)e2;
    if (m1->SIM != m2->SIM)
        return m2->SIM - m1->SIM;
    else
        return m1->order - m2->order;
}

void createslist(FILE *fp)
{
    // 创建停用表
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

void createdlist(FILE *pp)
{
    // 创建字典表
    char word[30];
    while (fscanf(pp, "%s", word) != EOF)
    {
        int num = hash(word);
        struct dictw *r;
        r = dlist[num];
        if (r == NULL)
        {
            dlist[num] = (struct dictw *)malloc(sizeof(struct dictw));
            strcpy(dlist[num]->word, word);
            dlist[num]->next = NULL;
        }
        else
        {
            r = (struct dictw *)malloc(sizeof(struct dictw));
            strcpy(r->word, word);
            r->next = dlist[num];
            dlist[num] = r;
        }
    }
}

int isnon(char *word)
{
    //不在停用表返回1
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

int isdic(char *word)
{
    //在字典返回1
    int n1 = hash(word);
    struct dictw *te = dlist[n1];
    while (te != NULL)
    {
        if (strcmp(te->word, word) == 0)
            return 1;
        te = te->next;
    }
    return 0;
}

unsigned int hash(char *str)
{
    // hash表
    unsigned int h = 0;
    char *s;
    for (s = str; *s != '\0'; s++)
        h = 37 * h + *s;
    return h % 6000;
}

