#include "scalednum.h"
#include <stdio.h>

int display_magnitude_info (double base, double exponent, const char* const prefixnames[2], void* callbackdata)
{
  printf("%.0f ^ %.0f : %s (%s)\n", base, exponent, prefixnames[HUMANREADABLE_LONGPREFIX], prefixnames[HUMANREADABLE_SHORTPREFIX]);
  return 0;
}

int main ()
{
  humanreadable hum;
  hum = humanreadable_create(3, HUMANREADABLE_KILO1000 | HUMANREADABLE_LONGPREFIX, "Watts", "Watt");
  humanreadable_iterate_magnitudes(hum, display_magnitude_info, NULL);

  printf("------------------------------------------------------------------------------------\n");

  double n = 0.0000009;
  while (n <= 100000000000000000000000.0) {
  //while (n > 0) {
    //printf("%" PRId64 "\t", n);
    printf("%40.8f\t", n);
    humanreadable_print(hum, n);
    printf("\t");
    humanreadable_print(hum, -n);
    printf("\n");
    //n = n * 10 + 4;
    n = n * 10.5;
  }
  humanreadable_free(hum);

  printf("------------------------------------------------------------------------------------\n");

  hum = humanreadable_create(3, HUMANREADABLE_KILO1024 | HUMANREADABLE_SHORTPREFIX, "B", NULL);
  n = 1;
  while (n <= 100000000000000000000000.0) {
  //while (n > 0) {
    //printf("%" PRId64 "\t", n);
    printf("%40.8f\t", n);
    //human_readable_number(n, &magnitude_kilo1000, 3);
    humanreadable_print(hum, n);
    printf("\t");
    humanreadable_print(hum, -n);
    printf("\n");
    n = n * 32;
  }
  humanreadable_free(hum);

  printf("------------------------------------------------------------------------------------\n");

  int i;
  n = 0;
  for (i = 0; i <= 3; i++) {
    hum = humanreadable_create(i, HUMANREADABLE_KILO1000 | HUMANREADABLE_SHORTPREFIX | HUMANREADABLE_NOSPACE, "W", NULL);
    humanreadable_print(hum, n);
    printf("\n");
    humanreadable_free(hum);
  }
  n = 0.000001;
  for (i = 0; i <= 3; i++) {
    hum = humanreadable_create(i, HUMANREADABLE_KILO1000 | HUMANREADABLE_LONGPREFIX, "Calories", "Calorie");
    humanreadable_print(hum, n);
    printf("\n");
    humanreadable_free(hum);
  }
  return 0;
}
