/***************************************************************************
 *   Copyright (C) 2008 by Juergen Legler   *
 *   jogihoogi@t-online.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#include <netinet/ip.h>
#include <netinet/udp.h>

#include <linux/netfilter.h>		/* for NF_ACCEPT */
#include <libnetfilter_queue/libnetfilter_queue.h>

#include "structs.h"


int debug_level = -1;


void  Clear_Packet_Struct(packet_struct_t *ps);

void debug_unknown (packet_struct_t *packet, int	readcount){

	int i,j,x;
	extern struct nfq_q_handle *gqh;
	extern int current_packet_id;

	if (packet->type)
		x = sizeof(struct ip) + sizeof(struct udphdr) + readcount -1;
	else 
		x =  sizeof(struct ip) + sizeof(struct udphdr) + readcount -1;

	for (i=0,j=0;i<=packet->packet_length;i++){
		if (i==0)
			printf("IP Header start \n");
		if (i==sizeof(struct ip))
			printf("UDP Header start \n");
		if (i>= sizeof(struct ip) + sizeof(struct udphdr)){
			if (x == i)
				printf("	%3i %3i: %3i <-----\n",i,j++,packet->packet_backup[i]);
			else
				printf("	%3i %3i: %3i \n",i,j++,packet->packet_backup[i]);
		}else{
			if (x == i)
				printf("	%3i: %3i <-----\n",i,packet->packet_backup[i]);
			else
				printf("	%3i: %3i \n",i,packet->packet_backup[i]);

		}
		if (i==(sizeof(struct ip) + sizeof(struct udphdr) -1))
		printf("Header stop \n");
	}

	nfq_set_verdict(gqh, current_packet_id, NF_ACCEPT,0,0);
	Clear_Packet_Struct(packet);

	printf("%i\n",x);

	
	exit(1);

}

void Dprintf(int level,char *str, ...){
	
	va_list argptr;

	if (level > debug_level)
		return;

	va_start (argptr,str);
	vprintf(str,argptr);
	va_end(argptr);



}

