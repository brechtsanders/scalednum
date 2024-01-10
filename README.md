# scalednum
Cross-platform C library for representing numbers in human readable form using magnitudes (kilo, mega, giga, ...).

## minimal example
The following application:
```c
#include "scalednum.h"
#include <stdio.h>

int main ()
{
  scalednum w = scalednum_create(3, SCALEDNUM_KILO1000 | SCALEDNUM_LONGPREFIX, "Watts", "Watt");
  scalednum_print(w, 2400000);
  scalednum_free(w);
  return 0;
}
```
will return the following output:
```raw
2.40 megaWatts
```

## GitHub Actions CI
[![GitHub-CI for scalednum](https://github.com/brechtsanders/scalednum/actions/workflows/scalednum.yml/badge.svg)](https://github.com/brechtsanders/scalednum/actions/workflows/scalednum.yml)
