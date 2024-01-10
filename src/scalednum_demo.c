#include "scalednum.h"
#include <stdio.h>

int display_magnitude_info (double base, double exponent, const char* const prefixnames[2], void* callbackdata)
{
  printf("%.0f ^ %.0f : %s (%s)\n", base, exponent, prefixnames[SCALEDNUM_LONGPREFIX], prefixnames[SCALEDNUM_SHORTPREFIX]);
  return 0;
}

int main ()
{
  scalednum hum;
  hum = scalednum_create(3, SCALEDNUM_KILO1000 | SCALEDNUM_LONGPREFIX, "Watts", "Watt");
  scalednum_iterate_magnitudes(hum, display_magnitude_info, NULL);

  printf("------------------------------------------------------------------------------------\n");

  double n = 0.0000009;
  while (n <= 100000000000000000000000.0) {
  //while (n > 0) {
    //printf("%" PRId64 "\t", n);
    printf("%40.8f\t", n);
    scalednum_print(hum, n);
    printf("\t");
    scalednum_print(hum, -n);
    printf("\n");
    //n = n * 10 + 4;
    n = n * 10.5;
  }
  scalednum_free(hum);

  printf("------------------------------------------------------------------------------------\n");

  hum = scalednum_create(3, SCALEDNUM_KILO1024 | SCALEDNUM_SHORTPREFIX, "B", NULL);
  n = 1;
  while (n <= 100000000000000000000000.0) {
  //while (n > 0) {
    //printf("%" PRId64 "\t", n);
    printf("%40.8f\t", n);
    //human_readable_number(n, &magnitude_kilo1000, 3);
    scalednum_print(hum, n);
    printf("\t");
    scalednum_print(hum, -n);
    printf("\n");
    n = n * 32;
  }
  scalednum_free(hum);

  printf("------------------------------------------------------------------------------------\n");

  int i;
  n = 0;
  for (i = 0; i <= 3; i++) {
    hum = scalednum_create(i, SCALEDNUM_KILO1000 | SCALEDNUM_SHORTPREFIX | SCALEDNUM_NOSPACE, "W", NULL);
    scalednum_print(hum, n);
    printf("\n");
    scalednum_free(hum);
  }
  n = 0.000001;
  for (i = 0; i <= 3; i++) {
    hum = scalednum_create(i, SCALEDNUM_KILO1000 | SCALEDNUM_LONGPREFIX, "Calories", "Calorie");
    scalednum_print(hum, n);
    printf("\n");
    scalednum_free(hum);
  }
  return 0;
}
