/* Author : Jerome ODIER
 * Email : jerome.odier@lpsc.in2p3.fr
 */

/*-------------------------------------------------------------------------*/

#include "../include/cami.h"

/*-------------------------------------------------------------------------*/

#define closesocket(s) \
		close(s)

#define h_addr h_addr_list[0]

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

/*-------------------------------------------------------------------------*/
