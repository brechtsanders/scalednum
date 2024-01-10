#include "scalednum.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>

SCALEDNUM_EXPORT void scalednum_get_version (int* pmajor, int* pminor, int* pmicro)
{
  if (pmajor)
    *pmajor = SCALEDNUM_VERSION_MAJOR;
  if (pminor)
    *pminor = SCALEDNUM_VERSION_MINOR;
  if (pmicro)
    *pmicro = SCALEDNUM_VERSION_MICRO;
}

SCALEDNUM_EXPORT const char* scalednum_get_version_string ()
{
  return SCALEDNUM_VERSION_STRING;
}

//#define MINIMIZE_SIZE         //build smaller size version with shared datastructure (will use k/kilo instead of Ki/kibi)
#define HAVE_LOG10

struct magnitude_definition_struct {
  const double base;                                      //magnitude base number (e.g. 1000 for kilo,mega,giga,...)
  const int zeroindex;                                    //index of base magnitude (the magnitude used to represent value 0)
  const int maxindex;                                     //maximum index (points to largest magnitude)
  const struct magnitude_prefix_info_struct* prefixnames; //list of magnitude symbols and names
  const size_t prefix_longest_length[2];                  //lengths of longest prefix abbreviation and full prefix
};

struct magnitude_prefix_info_struct {
  const char* prefixname[2];    //[0] = magnitude prefix abbreviation, [1] = magnitude full prefix
};

const struct magnitude_prefix_info_struct magnitude_prefixes_kilo1000[] = {
  {{"q", "quecto"}},
  {{"r", "ronto"}},
  {{"y", "yocto"}},
  {{"z", "zepto"}},
  {{"a", "atto"}},
  {{"f", "femto"}},
  {{"p", "pico"}},
  {{"n", "nano"}},
  {{"u"/*"µ"*//*"\xB5"*/, "micro"}},
  {{"m", "milli"}},
  {{"",  ""}},
  {{"k", "kilo"}},
  {{"M", "mega"}},
  {{"G", "giga"}},
  {{"T", "tera"}},
  {{"P", "peta"}},
  {{"E", "exa"}},
  {{"Z", "zetta"}},
  {{"Y", "yotta"}},
  {{"R", "ronna"}},
  {{"Q", "quetta"}}
};

const struct magnitude_definition_struct magnitude_kilo1000 = {
  .base = 1000,
  .zeroindex = 10,
  .maxindex = 20,
  .prefixnames = magnitude_prefixes_kilo1000,
  .prefix_longest_length = {1, 6}
};

#ifndef MINIMIZE_SIZE
const struct magnitude_prefix_info_struct magnitude_prefixes_kilo1024[] = {
  {{"",  ""}},
  {{"Ki", "kibi"}},
  {{"Mi", "mebi"}},
  {{"Gi", "gini"}},
  {{"Ti", "tebi"}},
  {{"Pi", "pebi"}},
  {{"Ei", "exbi"}},
  {{"Zi", "zebi"}},
  {{"Yi", "yobi"}},
  {{"Ri", "robi"}},
  {{"Qi", "quebi"}}
};
#endif

const struct magnitude_definition_struct magnitude_kilo1024 = {
  .base = 1024,
  .zeroindex = 0,
#ifndef MINIMIZE_SIZE
  .maxindex = 10,
  .prefixnames = magnitude_prefixes_kilo1024,
  .prefix_longest_length = {2, 5}
#else
  .maxindex = magnitude_kilo1000.maxindex - magnitude_kilo1000.zeroindex,
  .prefixnames = &magnitude_kilo1000.prefixnames[magnitude_kilo1000.zeroindex],
  .prefix_longest_length = {1, 6}
#endif
};

/*
struct number_magnitude_struct {
  int power;                                    //power used for magitude base
  double scaledvalue;                           //value scaled to magnitude (value = scaledvalue * base ^ power)
  double fractiondigits;                        //digits needed after the floating point, to be used in: printf("%.*f", fractiondigits, value);
  struct magnitude_prefix_info_struct* prefix;  //prefix names
};
*/

struct scalednum_struct {
  unsigned int flags;                                             //flags used when scalednum_create() was called
  const struct magnitude_definition_struct* magnitudedefinition;  //magnitude definition to be used
  unsigned int significantdigits;                                 //number of significant digits to display
  unsigned int prefixindex;                                       //index to use in prefixnames
  char* suffixplural;                                             //plural suffix (or NULL if no suffix)
  char* suffixsingular;                                           //singular suffix (or NULL to use plural suffix)
#if 0
  size_t maxlength;                                               //maximum buffer length needed to display human readable value (including terminating '\0' character)
#endif
};

SCALEDNUM_EXPORT scalednum scalednum_create (unsigned int significantdigits, unsigned int flags, const char* suffixplural, const char* suffixsingular)
{
  struct scalednum_struct* handle;
  if ((handle = (struct scalednum_struct*)malloc(sizeof(struct scalednum_struct))) == NULL)
    return NULL;  //memory allocation error
  switch (flags & SCALEDNUM_MAGNITUDE_MASK) {
    case SCALEDNUM_KILO1000 & SCALEDNUM_MAGNITUDE_MASK:
      handle->magnitudedefinition = &magnitude_kilo1000;
      break;
    case SCALEDNUM_KILO1024 & SCALEDNUM_MAGNITUDE_MASK:
      handle->magnitudedefinition = &magnitude_kilo1024;
      break;
    default:
      free(handle);
      return NULL;  //invalid magnitude flag specified
  }
  handle->flags = flags;
  handle->significantdigits = (significantdigits < 1 ? 1 : significantdigits);
  handle->prefixindex = flags & SCALEDNUM_PREFIXTYPE_MASK;
  if (!suffixplural) {
    handle->suffixplural = NULL;
  } else {
    if ((handle->suffixplural = strdup(suffixplural)) == NULL) {
      free(handle);
      return NULL;  //memory allocation error
    }
  }
  if (!suffixsingular) {
    handle->suffixsingular = handle->suffixplural;
  } else {
    if ((handle->suffixsingular = strdup(suffixsingular)) == NULL) {
      if (handle->suffixplural)
        free(handle->suffixplural);
      free(handle);
      return NULL;  //memory allocation error
    }
  }
#if 0
  //determine maximum buffer size needed (not valid for numbers not representable by available magnitude)
  handle->maxlength = (suffixplural ? strlen(suffixplural) : 0);
  if (suffixsingular && strlen(suffixsingular) > handle->maxlength)
    handle->maxlength = strlen(suffixsingular);
  handle->maxlength += 1 /* sign */ + handle->significantdigits + 1 /* floating point character */ + (flags & SCALEDNUM_NOSPACE ? 0 : 1) + handle->magnitudedefinition->prefix_longest_length[handle->prefixindex] + 1 /* terminating '\0' character */;
#endif
  return handle;
}

SCALEDNUM_EXPORT void scalednum_free (scalednum handle)
{
  if (handle->suffixsingular) {
    if (handle->suffixsingular != handle->suffixplural)
      free(handle->suffixsingular);
  }
  if (handle->suffixplural)
    free(handle->suffixplural);
  free(handle);
}

SCALEDNUM_EXPORT void scalednum_iterate_magnitudes (scalednum handle, scalednum_iterate_magnitudes_callback_fn callbackfn, void* callbackdata)
{
  int i;
  for (i = 0; i <= handle->magnitudedefinition->maxindex; i++) {
    if ((*callbackfn)(handle->magnitudedefinition->base, i - handle->magnitudedefinition->zeroindex, handle->magnitudedefinition->prefixnames[i].prefixname, callbackdata) != 0)
      break;
  }
}

SCALEDNUM_EXPORT int scalednum_to_buffer (scalednum handle, double value, char* buf, size_t buflen)
{
  int power;
  double scaledvalue;
  unsigned int scaleddigits;
  //determine magnitude as a power of base
  power = (value == 0 ? 0 : floor(log(fabs(value)) / log(handle->magnitudedefinition->base)));
  //make sure power is in supported range
  if (power < -handle->magnitudedefinition->zeroindex)
    power = 0;
  else if (power > handle->magnitudedefinition->maxindex - handle->magnitudedefinition->zeroindex)
    power = handle->magnitudedefinition->maxindex - handle->magnitudedefinition->zeroindex;
  //scale value to selected magnitude
  scaledvalue = value / pow(handle->magnitudedefinition->base, power);
  //determine number of digits before floating point
#ifdef HAVE_LOG10
  scaleddigits = (value == 0 ? 1 : trunc(log10(fabs(scaledvalue))) + 1);
#else
  scaleddigits = (value == 0 ? 1 : trunc(log(fabs(scaledvalue)) / log(10)) + 1);
#endif
  return snprintf(buf, buflen, "%s%.*f%s%s%s", (value > 0 && handle->flags & SCALEDNUM_FORCESIGN ? "+" : ""), (scaleddigits < handle->significantdigits && !(power == 0 && handle->flags & SCALEDNUM_NOFRACTION)? handle->significantdigits - scaleddigits : 0), scaledvalue, (handle->flags & SCALEDNUM_NOSPACE ? "" : " "), handle->magnitudedefinition->prefixnames[handle->magnitudedefinition->zeroindex + power].prefixname[handle->prefixindex], (value == 1 && handle->suffixsingular ? handle->suffixsingular : (handle->suffixplural ? handle->suffixplural : "")));
}

SCALEDNUM_EXPORT void scalednum_print (scalednum handle, double value)
{
  char staticbuf[64];
  char* buf;
  size_t buflen;
  if ((buflen = scalednum_to_buffer(handle, value, staticbuf, sizeof(staticbuf))) < sizeof(staticbuf)) {
    printf("%s", staticbuf);
  } else if ((buf = (char*)malloc(++buflen)) == NULL) {
    printf("MEMORY ALLOCATION ERROR");
  } else {
    scalednum_to_buffer(handle, value, buf, buflen);
    printf("%s", buf);
    free(buf);
  }
}

