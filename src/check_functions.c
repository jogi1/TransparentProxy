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
#include "debugging.h"

bool make_em_glow = 0;

char	*ack_string = NULL;

void Check_Command(char *s){
	extern bool send_ack_to_client ;

	if(strstr(s,"glow")){
		make_em_glow = !make_em_glow;
		if (ack_string)
			free(ack_string);
		ack_string = strdup("tproxy -- make em glow toggeled\n");
		send_ack_to_client = 1;
	}


}

void Edit_Playerinfo(packet_type_struct_t *p){
	byte buf[4096];
	int size;
	short int s;
	int effect_location;
	int	duc_f;
	int	x;
	extern int protoversion;
	
	bool has_no_effects = 0;

	// copy type and playernum
	//memcpy(&buf,p->data,2);
	size = 0;
	buf[size] = p->data[size];
	size++;
	buf[size] = p->data[size];
	size++;
	

	s = (short)(p->data[2] + (p->data[3] << 8));

	Dprintf(-1,"edit_playerinfo %d %d\n",s,protoversion);

	for (x=0;x<17;x++)
		if (s & (1 << x))
			Dprintf(-1,"%d: %d\n",x,(1<<x));


	if (!(s & PF_EFFECTS) && make_em_glow)
		has_no_effects = 1 ;
	// flags
	buf[size] = p->data[size];size++;
	buf[size] = p->data[size];size++;

	// Coord
	buf[size] = p->data[size];size++;
	buf[size] = p->data[size];size++;

	buf[size] = p->data[size];size++;
	buf[size] = p->data[size];size++;

	buf[size] = p->data[size];size++;
	buf[size] = p->data[size];size++;

	// frame
	buf[size] = p->data[size];size++;

	if (s & PF_MSEC){
		Dprintf(-1,"PF_MSEC\n");
		buf[size] = p->data[size];size++;
	}

	if (s & PF_COMMAND){
		// read a deltausercmd
		// flags
		Dprintf(-1,"PF_COMMAND\n");
		duc_f  = p->data[size];
		Dprintf(-1,"PF_COMMAND %d\n",p->data[size]);
		for (x=0;x<17;x++)
			if (p->data[size] & (1 << x))
				Dprintf(-1,"%d: %d\n",x,(1<<x));
		buf[size] = p->data[size];size++;
		if (protoversion == 26){
			if (duc_f & CM_ANGLE1){
				buf[size] = p->data[size];size++;
				buf[size] = p->data[size];size++;
			}
	
			
				buf[size] = p->data[size];size++;
				buf[size] = p->data[size];size++;
			
	
			if (duc_f & CM_ANGLE3){
				buf[size] = p->data[size];size++;
				buf[size] = p->data[size];size++;
			}
	
	
			if (duc_f & CM_FORWARD){
				buf[size] = p->data[size];size++;
			}
	
			if (duc_f & CM_SIDE){
				buf[size] = p->data[size];size++;
			}
	
			if (duc_f & CM_UP){
				buf[size] = p->data[size];size++;
			}

		}else{
			if (duc_f & CM_ANGLE1){
				buf[size] = p->data[size];size++;
				buf[size] = p->data[size];size++;
			}
	
			if (duc_f & CM_ANGLE2){
				buf[size] = p->data[size];size++;
				buf[size] = p->data[size];size++;
			}
	
			if (duc_f & CM_ANGLE3){
				buf[size] = p->data[size];size++;
				buf[size] = p->data[size];size++;
			}
	
			if (duc_f & CM_FORWARD){
				buf[size] = p->data[size];size++;
				buf[size] = p->data[size];size++;
			}
	
			if (duc_f & CM_SIDE){
				buf[size] = p->data[size];size++;
				buf[size] = p->data[size];size++;
			}
	
			if (duc_f & CM_UP){
				buf[size] = p->data[size];size++;
				buf[size] = p->data[size];size++;
			}
	
			
		}


		if (duc_f & CM_BUTTONS){
			buf[size] = p->data[size];size++;
		}

		if (duc_f & CM_IMPULSE){
			buf[size] = p->data[size];size++;
		}


		if (protoversion == 26){
			if (duc_f & CM_MSEC){
				buf[size] = p->data[size];size++;
			}
		}else{
			buf[size] = p->data[size];size++;
		}

	}

	if (s & PF_VELOCITY1){
		Dprintf(-1,"PF_VEL1\n");
		buf[size] = p->data[size];size++;
		buf[size] = p->data[size];size++;
	}

	if (s & PF_VELOCITY2){
		Dprintf(-1,"PF_VEL2\n");
		buf[size] = p->data[size];size++;
		buf[size] = p->data[size];size++;
	}

	if (s & PF_VELOCITY3){
		Dprintf(-1,"PF_VEL3\n");
		buf[size] = p->data[size];size++;
		buf[size] = p->data[size];size++;
	}

	if (s & PF_MODEL){
		Dprintf(-1,"PF_MODEL\n");
		buf[size] = p->data[size];size++;
	}

	if (s & PF_SKINNUM){
		Dprintf(-1,"PF_SKINNUM\n");
		buf[size] = p->data[size];size++;
	}

	
	effect_location = size;

	if (has_no_effects && make_em_glow)
		size++;

	if (s & PF_EFFECTS){
		Dprintf(-1,"PF_SKINNUM\n");
		if (has_no_effects && make_em_glow){
			buf[size] = p->data[size-1];size++;
		}else{
			buf[size] = p->data[size];size++;
		}
	}

	if (s & PF_WEAPONFRAME){
		Dprintf(-1,"PF_WEAPONFRAME\n");
		if (has_no_effects && make_em_glow){
			buf[size] = p->data[size-1];size++;
		}else{
			buf[size] = p->data[size];size++;
		}
	}

	if (has_no_effects && make_em_glow){
		s += PF_EFFECTS;
	}

	if (make_em_glow){
		buf[effect_location] = 1 + 4 + 64 + 128;
		Dprintf(-1,"??\n");
	}
	
	Dprintf(-1,"length -- old: %d - new: %d -- effect location: %d\n",p->length,size,effect_location);
	free(p->data);
	p->data = malloc(size);
	memset(p->data,0,size);
	memcpy(p->data,&buf,size);
	memcpy(p->data + 2,&s,2);
	for (x=0;x<p->length+3;x++)
		Dprintf(-1,"%3i: %3i - %3i\n",x,(byte *)p->data[x],(byte)buf[x]);
	p->length = size;


	

}


bool	send_ack_to_client = 0;

void Check_Outgoing (packet_struct_t *packet){
	packet_type_struct_t	*p;
	char			*s;
	extern bool		send_new_packet;
	int			i=0;
	

	p = packet->chained_packet;
	Dprintf(1,"Checking messages\n");
	while (p){
		if (p->data[0] == clc_stringcmd){
			if ((s=strstr((char *)p->data +1,"tproxy::"))){
				i++;
				Check_Command(s);
				p->ignore = 1;
				
			}
		}
		p=p->next;
	}
	Dprintf(1,"	%3i - altered/dropped messages\n",i);
	Dprintf(1,"	%3i  - send new packet\n",send_new_packet);

	
}


void Check_Incoming (packet_struct_t *packet){
	packet_type_struct_t	*p,*s;
	extern bool		send_new_packet;
	int			i=0;
	
	p = packet->chained_packet;
	Dprintf(1,"Checking messages\n");
	if (send_ack_to_client){
		
		s = malloc(sizeof(packet_type_struct_t));
		memset(s,0,sizeof(packet_type_struct_t));
		packet->chained_packet = s;
		s->next = p;
		s->length = strlen(ack_string)+3;
		s->data = malloc(strlen(ack_string)+3);
		memset(s->data,0,strlen(ack_string)+3);
		memcpy(s->data+2,ack_string,strlen(ack_string));
		s->data[0] = svc_print;
		s->data[1] = 3;
		send_ack_to_client = 0;
	}
	p = packet->chained_packet;

	while (p){
		if (p->data[0] == svc_playerinfo){
			Edit_Playerinfo(p);
			
		}
		p=p->next;
	}

	Dprintf(1,"%3i - altered/dropped messages\n",i);
	Dprintf(1,"%3i  - send new packet\n",send_new_packet);

	
}

void	Check_Packet(packet_struct_t *packet){
	
	if (!packet->type)
		Check_Outgoing(packet);
	else
		Check_Incoming(packet);

}
