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


#include <netinet/in.h>
#include <linux/netfilter.h>		/* for NF_ACCEPT */
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <pcap.h>

#include <sys/socket.h>
#include <arpa/inet.h>




#include <libnetfilter_queue/libnetfilter_queue.h>

#include "structs.h"

#include "common.h"
#include "msg_functions.h"


void	Encrypt_Outgoing(packet_struct_t *packet){

	packet_type_struct_t	*p;
	int	new_size,current_size;
	int	i	= 0;


	p = packet->chained_packet;

	// get new packetsize
	current_size = new_size	=	packet->header_size + 10;//raw_pkhdr->iphdr.ip_hl +33;// 33 ;

	while (p){
		if (!p->ignore)
			new_size += p->length;
		p=p->next;
	}

	packet->new_packet_size = new_size ;
	

	// copying the header
	memcpy(packet->new_packet,packet->packet_backup, current_size);


	// putting together the rest
	p = packet->chained_packet;

	while (p){
		if (!p->ignore){
			memcpy(packet->new_packet + current_size, p->data, p->length);
			current_size += p->length;
		}else{
			i++;
		}
		
		p=p->next;
	}
	

	

};


void	Encrypt_Incoming(packet_struct_t *packet){



	packet_type_struct_t	*p;
	int	new_size,current_size;
	int	i	= 0;


	p = packet->chained_packet;

	// get new packetsize
	current_size = new_size	=	packet->header_size + 8;//raw_pkhdr->iphdr.ip_hl +33;// 33 ;

	while (p){
		if (!p->ignore)
			new_size += p->length;
		p=p->next;
	}

	packet->new_packet_size = new_size ;
	

	// copying the header
	memcpy(packet->new_packet,packet->packet_backup, current_size);


	// putting together the rest
	p = packet->chained_packet;

	while (p){
		if (!p->ignore){
			memcpy(packet->new_packet + current_size, p->data, p->length);
			current_size += p->length;
		}else{
			i++;
		}
		
		p=p->next;
	}
	

	return;
};



void	Encrypt_Packet(packet_struct_t *packet){
	if (!packet->type){
		Encrypt_Outgoing(packet);
	}else{
		Encrypt_Incoming(packet);
	}

};







