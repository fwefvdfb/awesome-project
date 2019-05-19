#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <stdbool.h>

//бесконечный путь будет равен числу 999
#define INF 999;

//определение максимального числа в матрице (не считая бесконечного пути)
const int max_value = 100;

//"шапка" -- число будет добавлятся к максимальному числу в матрице,
// если случайное число будет выше max_value число считается 999
const int cape = 25;

//копирование из 1 матрицы в другую
void matrix_copy_in(int *m1[],int*m2[],int n){
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<n;j++)
    {
      m2[i][j] = m1[i][j];
    }
  }
}

//случайное заполнение матрицы
void matrix_init(int *p[], int n)
{
  
  srand((unsigned int)time(NULL));
  int rv;
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<n;j++)
    {
      //проверка на главную диагональ (элементы главной диагонали = 0)
      if(i!=j)
      {
        rv = rand()%(max_value+cape);
  
        //если случайное число больше максимально
        //дозволенного значения -- число = бесконечному пути
        if(rv>max_value)
        {
          p[i][j]=INF;
        }else
        {
          p[i][j] = rv;
        }
      }
      else
      {
        p[i][j]=0;
      }
    }
  }
}

bool compare(int *arg1[],int *arg2[],int n)
{
  for(int i=0;i<n;i++)
  {
    for(int j=0;j<n;j++)
    {
      if(arg1[i][j] != arg2[i][j])
      {
        return false;
      }
    }
  }
  return true;
}

//метод для вывода матрицы
void matrix_print(int *p[],int n,char* str)
{

  int space, manip;
  printf("\n%s:",str); //вывод названия матрицы
  for(int i=0;i<n;i++)
  {
    printf("\n");
    for(int j=0;j<n;j++)
  {

    if(p[i][j] != 999)
    {
      //для наглядности все числа отделены дефизом,
      //все числа занимают 3 символа
      manip = 2;
      space = p[i][j] / 10;

      //счёт кол-ва разрядов числа
      while(space > 0)
      {
        space /= 10;
        manip--;
      }
      //заполнение дефизами
      for(int k = 0;k<manip;k++){
        printf("-");
      }
      printf("-%d-",p[i][j]);
    }
    else
    {
      printf("-INF-");
    }
    }

  }
}

//метод для реализации алгоритма Флойда-Воршелла
//без распараллеливания
void matrix_dothejob_np(int *p[],int n)
{
  int i,j,k,m=0;
  int cnt = n*n;
  for(k=0;k<n;k++){
  for(i=0;i<n;i++){
    for(j=0;j<1+i;j++){
      int rev_col = n-1-i+j;
      if((i-j)!=k && j!=k && p[j][i - j] > p[j][k] + p[k][i-j])
      {
        p[j][i - j] = p[j][k] + p[k][i-j];
      }
      if(rev_col!=k && (n-1-j)!=k && p[n-1-j][rev_col] > p[n-1-j][k] + p[k][rev_col])
      {
        p[n-1-j][rev_col] = p[n-1-j][k] + p[k][rev_col];
      }
    }
  }
  }
}

//метод для реализации алгоритма Флойда-Воршелла
//распараллеленная
void matrix_dothejob_p(int *p[],int n)
{
  int i,j,k,m=0;
  int cnt = n*n;
  for(k=0;k<n;k++){
  for(i=0;i<n;i++){
//параллельно выполняется расчёт диагоналей.
//Распараллеливание считается эффективным, т.к. нет конфликта к обращаемым ячейкам в матрице
#pragma omp parallel for shared(k,i,n) private(j)
    for(j=0;j<1+i;j++){
      int rev_col = n-1-i+j;
      if((i-j)!=k && j!=k && p[j][i - j] > p[j][k] + p[k][i-j])
      {
        p[j][i - j] = p[j][k] + p[k][i-j];
      }
      if(rev_col!=k && (n-1-j)!=k && p[n-1-j][rev_col] > p[n-1-j][k] + p[k][rev_col])
      {
        p[n-1-j][rev_col] = p[n-1-j][k] + p[k][rev_col];
      }
    }
  }
  }
}



//инициализация матрицы вручную
void matrix_manual_init(int *p[],int n)
{
  printf("manual");
  for(int i=0;i<n;i++)
  {
    for(int j =0;j<n;j++)
    {
        printf("\n%d,%d:",i,j);
        scanf("%d",&p[i][j]);
    }
  }
}

float elapsed_msecs(struct timeval s, struct timeval f)
{
  return (float) (1000.0 * (f.tv_sec - s.tv_sec) + (0.001 * (f.tv_usec - s.tv_usec)));
}


int main()
{
  struct timeval start;
  struct timeval finish;

  long cpu_time_p,cpu_time_np;

  bool flag;

  int *m = malloc(sizeof(int));

  printf("Insert N:");

  scanf("%d",m);

  flag = *m<20;

  int *W[*m];
  int *W_np[*m];

  //выделение памяти для 2 матриц
  for(int i = 0;i<*m;i++)
  {
    W[i]=malloc((*m)*sizeof(int));
    W_np[i]=malloc((*m)*sizeof(int));
  }


  //printf("Manual init?(1/0)");

  matrix_init(W,*m);

  matrix_copy_in(W,W_np,*m);

  if(flag)
  matrix_print(W_np,*m,"\nDefault matrix");

  gettimeofday(&start,0);

  matrix_dothejob_p(W,*m);

  gettimeofday(&finish,0);

  cpu_time_p = elapsed_msecs(start,finish);

  if(flag)
  matrix_print(W,*m,"\nparallel");

  gettimeofday(&start,0);

  matrix_dothejob_np(W_np,*m);

  gettimeofday(&finish,0);

  cpu_time_np = elapsed_msecs(start,finish);

  if(flag)
  matrix_print(W_np,*m,"\nnot parallel algorithm matrix");

  //сравнение матриц, если они равны ошибок не обнаружено
  if(compare(W,W_np,*m))
  {
    printf("\nresults are equal");
  }
  else
  {
    printf("\nresults are not equal");
  }
  printf("\nTime used with threads: %ld ms",cpu_time_p);
  printf("\nTime used without threads: %ld ms",cpu_time_np);



  for(int i = 0;i<*m;i++)
  {
    int *space = W[i];
    free(space);
    space = W_np[i];
    free(space);
  }
  free(m);

  getchar();
  getchar();
}
