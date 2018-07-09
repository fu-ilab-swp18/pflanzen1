#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

#include "net/gnrc/rpl.h"
#include "net/sock/udp.h"
#include "net/ipv6/addr.h"


#ifdef _PFLANZEN_DEBUG
char PFLANZEN_DEBUG = _PFLANZEN_DEBUG;
#else
char PFLANZEN_DEBUG = 0;
#endif

int shell_debug ( int argc, char *argv[]) {
    if ( argc <= 1 || strcmp(argv[1], "on") == 0 ) {
        PFLANZEN_DEBUG = 1;
        printf("Debug prints activated. Run `%s off` to disable.\n", argv[0]);
    } else if ( argc > 1 && strcmp(argv[1], "off") == 0 ) {
        PFLANZEN_DEBUG = 0;
        printf("Debug prints have been turned off.\n");
    } else {
        printf("Usage: %s [on]|off\n", argv[0]);
        return 1;
    }
    return 0;
}

int shell_info ( int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    int rv;

    printf("Node role:         %s\n", NODE_ROLE);
    printf("Node ID:           %04X\n", NODE_ID);

    gnrc_netif_t *netif = NULL;
    ipv6_addr_t addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
    while ( true ) {
        netif = gnrc_netif_iter(netif);
        if ( netif == NULL )
            break;

        rv = gnrc_netif_ipv6_addrs_get(netif, addrs,
                GNRC_NETIF_IPV6_ADDRS_NUMOF*sizeof(ipv6_addr_t));
        if ( rv < 0 )
            continue;

        for ( unsigned int i = 0; i < (rv/sizeof(ipv6_addr_t)); i++ ) {
            ipv6_addr_t addr = addrs[i];
            if ( ipv6_addr_is_global(&addr) ) {
                printf("IP Address:        "); fflush(stdout);
                ipv6_addr_print(&addr); putchar('\n');
            }
        }
    }

    return 0;
}

int shell_exit ( int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    _exit(0);
}

nodeid_t nodeid_from_device ( void ) {
    /* return a node id that is specific to the device, iaw it is the same every
     * time a node is rebooted. It is ensured that 0 > nodeid > 0xff00.
     * (Implementation detail: Currently, we just use the least significant
     * 16 bits of the device's link local address.)
     * On error, returns 0.
     */
    int rv;
    gnrc_netif_t *netif = NULL;
    ipv6_addr_t addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
    nodeid_t nodeid;
    while ( true ) {
        netif = gnrc_netif_iter(netif);
        if ( netif == NULL )
            break;

        rv = gnrc_netif_ipv6_addrs_get(netif, addrs,
                GNRC_NETIF_IPV6_ADDRS_NUMOF*sizeof(ipv6_addr_t));
        if ( rv < 0 )
            continue;

        for ( unsigned int i = 0; i < (rv/sizeof(ipv6_addr_t)); i++ ) {
            ipv6_addr_t addr = addrs[i];
            if ( ipv6_addr_is_link_local(&addr) ) {
                nodeid = (addr.u8[14] << 8) + addr.u8[15];
                if ( nodeid >= 0xff00 )
                    nodeid &= 0x7fff;
                if ( nodeid < 1 )
                    nodeid |= 0x8000;

                if ( PFLANZEN_DEBUG) {
                    printf("Setting node ID to %04X from address: ", nodeid);
                    fflush(stdout); ipv6_addr_print(&addr); putchar('\n');
                }
                return nodeid;
            }
        }
    }
    // nothing found
    return 0;
}

// from https://stackoverflow.com/a/7776146/196244 , slightly adapted
void hexdump (char *desc, void *addr, int len) {
    /* Print some data as a hexdump (like `hexdump -C`)
     * @param desc: if not NULL, printed before the dump (plus colon and newline)
     * @param addr: start address of the data to dump
     * @param len: data length
     */

    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        } else if ((i % 16) == 8) {
            printf (" ");
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if (pc[i] == 0)
            buff[i % 16] = ':';
        else if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        if ((i % 16) == 8 )
            printf (" ");
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

#ifdef USE_LIBC_ERRORH
#include <error.h>
#else
void error(int status, int errnum, const char *format, ...) {
    fflush(stdout);

    fprintf(stderr, "error: ");

    if ( errnum != 0 ) {
/* maybe use this again later conditionally on architecturess that have it
        char buf[1024];
        strerror_r(errnum, buf, 1024);
        fprintf(stderr, "%s: ", buf);
*/
        fprintf(stderr, "%d: ", errnum);
    }

    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);

    fprintf(stderr, "\n");
    fflush(stderr);
    if ( status != 0 ) {
        _exit(status);
    }
}
#endif // USE_LIBC_ERRORH
