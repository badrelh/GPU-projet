#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char *bonjour[] = {"Good morning", "Bonjour", "Buon Giorno",
                         "Buenos días",  "Egun on", NULL};

const char *aurevoir[] = {"Bye",         "Au revoir", "Arrivederci",
                          "Hasta luego", "Adio",      NULL};

int main ()
{
#pragma omp parallel
  {
#pragma omp master
    {
      for (int i = 0; bonjour[i] != NULL; i++)
#pragma omp task depend(out: bonjour[i])
        printf ("%s (%d)\n", bonjour[i], omp_get_thread_num ());
      for (int i = 0; aurevoir[i] != NULL; i++)
#pragma omp task  depend(in: bonjour[i])
		 printf ("%s (%d)\n", aurevoir[i], omp_get_thread_num ());
    }
  }
  return 0;
}
