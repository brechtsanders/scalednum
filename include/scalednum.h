/**
 * @file scalednum.h
 * @brief scalednum library header file with main functions
 * @author Brecht Sanders
 *
 * This header file defines the functions needed for the basic library example
 */

#ifndef INCLUDED_SCALEDNUM_H
#define INCLUDED_SCALEDNUM_H

#include <stddef.h>



/*! \cond PRIVATE */
#if !defined(SCALEDNUM_EXPORT)
# if defined(_WIN32) && defined(scalednum_SHARED_EXPORTS)
#  define SCALEDNUM_EXPORT __declspec(dllexport)
# elif /*defined(_WIN32)*/defined(__MINGW32__) && !defined(STATIC) && !defined(STATIC_SCALEDNUM)
#  define SCALEDNUM_EXPORT __declspec(dllimport)
# else
#  define SCALEDNUM_EXPORT
# endif
#endif
/*! \endcond */



/*! \brief version number constants
 * \sa     scalednum_get_version()
 * \sa     scalednum_get_version_string()
 * \name   SCALEDNUM_VERSION_*
 * \{
 */
/*! \brief major version number */
#define SCALEDNUM_VERSION_MAJOR 0
/*! \brief minor version number */
#define SCALEDNUM_VERSION_MINOR 0
/*! \brief micro version number */
#define SCALEDNUM_VERSION_MICRO 2
/*! @} */

/*! \brief packed version number */
#define SCALEDNUM_VERSION (SCALEDNUM_VERSION_MAJOR * 0x01000000 + SCALEDNUM_VERSION_MINOR * 0x00010000 + SCALEDNUM_VERSION_MICRO * 0x00000100)

/*! \cond PRIVATE */
#define SCALEDNUM_VERSION_STRINGIZE_(major, minor, micro) #major"."#minor"."#micro
#define SCALEDNUM_VERSION_STRINGIZE(major, minor, micro) SCALEDNUM_VERSION_STRINGIZE_(major, minor, micro)
/*! \endcond */

/*! \brief string with dotted version number \hideinitializer */
#define SCALEDNUM_VERSION_STRING SCALEDNUM_VERSION_STRINGIZE(SCALEDNUM_VERSION_MAJOR, SCALEDNUM_VERSION_MINOR, SCALEDNUM_VERSION_MICRO)

/*! \brief string with name of scalednum library */
#define SCALEDNUM_NAME "scalednum"

/*! \brief string with name and version of scalednum library \hideinitializer */
#define SCALEDNUM_FULLNAME SCALEDNUM_NAME " " SCALEDNUM_VERSION_STRING



/*! \brief flags
 * \sa     scalednum_create()
 * \name   SCALEDNUM_VERSION_*
 * \{
 */
/*! \brief mask to isolate prefix type */
#define SCALEDNUM_PREFIXTYPE_MASK 0x0001
/*! \brief use short prefix */
#define SCALEDNUM_SHORTPREFIX     0x0000
/*! \brief use long prefix */
#define SCALEDNUM_LONGPREFIX      0x0001

/*! \brief don't insert a space between value and magnitude */
#define SCALEDNUM_NOSPACE         0x0002

/*! \brief whole number, so base magnitude values will not have digits after the floating point */
#define SCALEDNUM_NOFRACTION      0x0004

/*! \brief always show sign (except for 0) */
#define SCALEDNUM_FORCESIGN       0x0008

/*! \brief mask to isolate magnitude type */
#define SCALEDNUM_MAGNITUDE_MASK  0xFF00
/*! \brief use 1000 for kilo */
#define SCALEDNUM_KILO1000        0x0000
/*! \brief use 1024 for kilo (implies whole numbers) */
#define SCALEDNUM_KILO1024       (0x0100 | SCALEDNUM_NOFRACTION)
/*! @} */



#ifdef __cplusplus
extern "C" {
#endif

/*! \brief get scalednum library version string
 * \param  pmajor        pointer to integer that will receive major version number
 * \param  pminor        pointer to integer that will receive minor version number
 * \param  pmicro        pointer to integer that will receive micro version number
 * \sa     scalednum_get_version_string()
 */
SCALEDNUM_EXPORT void scalednum_get_version (int* pmajor, int* pminor, int* pmicro);

/*! \brief get scalednum library version string
 * \return version string
 * \sa     scalednum_get_version()
 */
SCALEDNUM_EXPORT const char* scalednum_get_version_string ();

/*! \brief handle type used by scalednum library
 * \sa     scalednum_create()
 * \sa     scalednum_free()
 */
typedef struct scalednum_struct* scalednum;

/*! \brief create scalednum handle
 * \param  significantdigits    total number of significant digits to display
 * \param  flags                flags to use (combination of SCALEDNUM_VERSION_*)
 * \param  suffixplural         plural suffix (or NULL if no suffix)
 * \param  suffixsingular       singular suffix (or NULL to use plural suffix)
 * \return scalednum handle
 * \sa     SCALEDNUM_VERSION_*
 * \sa     scalednum_free()
 */
SCALEDNUM_EXPORT scalednum scalednum_create (unsigned int significantdigits, unsigned int flags, const char* suffixplural, const char* suffixsingular);

/*! \brief destroy scalednum handle
 * \param  handle               scalednum handle
 * \sa     scalednum_create()
 */
SCALEDNUM_EXPORT void scalednum_free (scalednum handle);

/*! \brief store human readable value in buffer
 * \param  handle               scalednum handle
 * \param  value                value
 * \param  buf                  memory buffer
 * \param  buflen               size of memory buffer (must be large enough to hold human readable representation and trailing '\0')
 * \return buffer size needed to store human readable representation (output to buffer will be truncated if buflen was too small)
 * \sa     scalednum_create()
 * \sa     scalednum_print()
 */
SCALEDNUM_EXPORT int scalednum_to_buffer (scalednum handle, double value, char* buf, size_t buflen);

/*! \brief print human readable value to console
 * \param  handle               scalednum handle
 * \param  value                value
 * \sa     scalednum_create()
 * \sa     scalednum_print()
 */
SCALEDNUM_EXPORT void scalednum_print (scalednum handle, double value);

/*! \brief function type used to iterate through posible magnitudes
 * \param  base                 base (multiplier is base ^ exponent)
 * \param  exponent             exponent (multiplier is base ^ exponent)
 * \param  prefixnames          array with short and long prefix names (e.g. ["k", "kilo"])
 * \param  callbackdata         callback data as passed to scalednum_iterate_magnitudes()
 * \return zero to continue or non-zero to abort
 * \sa     scalednum_iterate_magnitudes()
 * \sa     scalednum_create()
 */
typedef int (*scalednum_iterate_magnitudes_callback_fn) (double base, double exponent, const char* const prefixnames[2], void* callbackdata);

/*! \brief function type used to iterate through posible magnitudes
 * \param  handle               scalednum handle
 * \param  callbackfn           function of type scalednum_iterate_magnitudes_callback_fn to call for each entry
 * \param  callbackdata         callback data to be passed to callbackfn
 * \sa     scalednum_iterate_magnitudes_callback_fn()
 * \sa     scalednum_create()
 */
SCALEDNUM_EXPORT void scalednum_iterate_magnitudes (scalednum handle, scalednum_iterate_magnitudes_callback_fn callbackfn, void* callbackdata);

#ifdef __cplusplus
}
#endif

#endif
