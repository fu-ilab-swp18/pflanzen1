/* #define NATIVE 0 */
/* #define PHYNODE 1 */
/* #define SAMR 2 */

/* #define BOARD 0 */

/* #if BOARD == NATIVE */
/* #include "sensor/native/sensor.c" */
/* #elif BOARD == PHYNODE */
/* #include "sensor/pba_d_01_kw2x/sensor.c" */
/* #elif BOARD == SAMR */
/* #include "sensor/samr21_xpro/sensor.c" */
/* #else */
/* #error */
/* #endif */

#include "sensor.c"

#ifdef BOARD_NATIVE
#include "native/sensor.c"
#endif

#ifdef BOARD_SAMR21_XPRO
#include "samr21_xpro/sensor.c"
#endif

#ifdef BOARD_PBA_D_01_KW2X
#include "pba_d_01_kw2x/sensor.c"
#endif

int initialize_sensors(void);

int read_humidity(phydat_t *res);
int read_humidity_shell(int argc, char **argv);

int read_light(phydat_t *res);
int read_light_shell(int argc, char **argv);

