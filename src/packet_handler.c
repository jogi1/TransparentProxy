/***************************************************************************
 *   Copyright (C) 2008 by Juergen Legler   *
 *   jogihoogi(placetheapehere)t-online.de   *
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

#include <assert.h>

#include "packet_handler.h"



#include "common.h"




#include "structs.h"
#include "decrypt_functions.h"
#include "encrypt_functions.h"
#include "check_functions.h"
#include "debugging.h"


extern bool	has_unknown_types, direction;

int msg_readcount;
bool msg_badread;


bool	print_header_info;

sizebuf_t	net_message;

bool send_new_packet;

char *dst_ip = NULL;
char *src_ip = NULL;

int	in_packet_count, out_packet_count;





void  Clear_Packet_Struct(packet_struct_t *ps)
{

    //free(ps->packet_backup);
    //free(ps->new_packet);
    free(ps->payload);

    free(ps->raw_pkhdr);

    Delete_Packet_Struct_List(ps);

    //memset(ps,0,sizeof(packet_struct_t));

	//free (ps);



}


unsigned short IP_Header_Checksum(unsigned short* addr,char len)
{
    /* This is a simplified version that expects even number of bytes*/
    register long sum = 0;

    while (len > 1)
    {
        sum += *addr++;
        len -= 2;
    }
    while (sum>>16) sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

packet_struct_t *incoming;
packet_struct_t *current_packet;


bool	Decrypt_Packet(packet_struct_t	*packet){

	Dprintf(1,"packet type: %d\n",packet->type);

	if (packet->type == IN)
		Decrypt_Incoming(packet);
	else
		Decrypt_Outgoing(packet);

	return	false;
}





bool  Handle_Packet (unsigned char *original_packet, int original_packet_size,unsigned  char *new_packet  , int *new_packet_size)
{
	packet_struct_t ps_new;
	char		*destination_ip;
	int		z;
	static int x;

	memset(&ps_new,0,sizeof(packet_struct_t));

	ps_new.raw_pkhdr = malloc(sizeof(raw_packet_header_t));
	memcpy(ps_new.raw_pkhdr,original_packet,sizeof(raw_packet_header_t));
	
	destination_ip	= inet_ntoa(ps_new.raw_pkhdr->iphdr.ip_dst);

	if (strstr("192.168.222.2",destination_ip)){
		ps_new.type = IN;
		in_packet_count++;
		if (dst_ip == NULL)
			dst_ip = strdup(destination_ip);

	}else{
		ps_new.type = OUT;
		out_packet_count++;
		if (src_ip == NULL)
			src_ip = strdup(destination_ip);
	}

	ps_new.netfilter_id = x++;
	
	ps_new.packet_backup = (unsigned char *)original_packet;
	ps_new.packet_length = original_packet_size;

	
	ps_new.payload = malloc(original_packet_size - sizeof(raw_packet_header_t));
	memcpy(ps_new.payload,ps_new.packet_backup + sizeof(raw_packet_header_t), original_packet_size - sizeof(raw_packet_header_t));
	ps_new.payload_size	= original_packet_size - sizeof(raw_packet_header_t);
	ps_new.header_size	=	sizeof(raw_packet_header_t);

	ps_new.new_packet = new_packet;
	

	//for (i=0;i<=original_packet_size;i++)
	//	printf("%3d: %3d\n",i,original_packet[i]);


	
	Decrypt_Packet(&ps_new);
//	printf("grabme %d\n",ps_new.has_unknown_types );
	if (ps_new.has_unknown_types ){
		memcpy(new_packet,original_packet,original_packet_size);
		*new_packet_size = original_packet_size;
		Dprintf(1,"sending original packet\n");
	}else{
		Check_Packet(&ps_new);
		Encrypt_Packet(&ps_new);
		*new_packet_size = ps_new.new_packet_size;
		Dprintf(1,"sending reencoded packet\n");
	}

	z = ps_new.type;



	Delete_Packet_Struct_List(&ps_new);


	

	return z;


}





