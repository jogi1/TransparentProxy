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
#include <linux/netfilter.h>	/* for NF_ACCEPT */
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

#include "parse_commands.h"
#include "svc_strings.h"

#include "debugging.h"

void Create_Packet_Struct(packet_struct_t * packet_struct, char type, byte *data, int length)
{
	packet_type_struct_t *p;

	//packet_struct_t       *t;

	p = packet_struct->chained_packet;


	if (p == NULL)
	{

		p = packet_struct->chained_packet = (packet_type_struct_t *) malloc(sizeof(packet_type_struct_t));


		memset(p, 0, sizeof(packet_type_struct_t));



	}
	else
	{
		while (p->next)
			p = p->next;
		p->next = (packet_type_struct_t *) malloc(sizeof(packet_type_struct_t));
		p = p->next;
		memset(p, 0, sizeof(packet_type_struct_t));

	}



	p->type = type;
	p->data = data;
	p->length = length;


}

void Delete_Packet_Struct_List(packet_struct_t * packet_struct)
{
	packet_type_struct_t *s, *p;

	p = packet_struct->chained_packet;

	packet_struct->chained_packet = NULL;

	while (p)
	{
		s = p->next;
		free(p->data);
		free(p);


		p = s;

		if (p == NULL)
			break;
	}


}


int last_sequence;

/*
	incoming
*/


void Split_Incoming(packet_struct_t *packet, bool record)
{
	int old_readcount = 0;
	extern int msg_readcount;
	byte *dptr;
	int type;
	extern message_t net_message;
	unsigned sequence, sequence_ack;
	unsigned reliable_ack, reliable_message;

	int i = 0;
	
	net_message.cursize = packet->payload_size;
	net_message.data = packet->payload;

	MSG_BeginReading();

	if (record)
	{
		sequence = MSG_ReadLong();
		sequence_ack = MSG_ReadLong();
		//MSG_ReadShort();


		reliable_message = sequence >> 31;
		reliable_ack = sequence_ack >> 31;

		sequence &= ~(1 << 31);
		sequence_ack &= ~(1 << 31);

		packet->sequence              = sequence;
		packet->sequence_ack          = sequence_ack;

		packet->reliable_message      = reliable_message;
		packet->reliable_ack          = reliable_ack;

		
		//printf("TEST !%i\n",(unsigned int *)net_message.data[0]);
		//printf("%i %i %i %i\n", sequence, reliable_message, sequence_ack, reliable_ack);
		//printf("%i %i %i %i\n", packet->sequence, packet->reliable_message, packet->sequence_ack, packet->reliable_ack);
		//printf ("%i %i %i %i\n",(int)((char *)packet->sequence),(int)((char *)packet->sequence)+1,(int)((char *)packet->sequence)+2,(int)((char *)packet->sequence)+3);

		

		Dprintf(1,"IN  --> s=%i(%i) a=%i(%i) %i\n",packet->sequence,packet->reliable_message,packet->sequence_ack,\
		packet->reliable_ack, net_message.cursize);


	}

	while (1)
	{
		old_readcount = msg_readcount;
		type = MSG_ReadByte();
		if (type == -1)
			break;

		i++;
		switch (type)
		{

			case svc_serverdata:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Serverdata();
				break;


			case svc_updatestat:
				Dprintf(1,"%s\n", svc_strings[type]);
				Updatestat();
				break;


			case svc_updatestatlong:
				Dprintf(1,"%s\n", svc_strings[type]);
				Updatestat_Long();
				break;


			case svc_playerinfo:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Playerinfo();
				break;


			case svc_packetentities:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Packetentities(false);
				break;


			case svc_serverinfo:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Serverinfo();
				break;

			case svc_cdtrack:
				Dprintf(1,"%s\n", svc_strings[type]);
				MSG_ReadByte();
				break;

			case svc_stufftext:
				Dprintf(1,"%s\n", svc_strings[type]);
				Dprintf(1,"%s", MSG_ReadString());
				break;

			case svc_updatefrags:
				Dprintf(1,"%s\n", svc_strings[type]);
				//players[MSG_ReadByte()].frags = MSG_ReadShort();
				MSG_ReadByte();
				MSG_ReadShort();

				break;

			case svc_print:
				Dprintf(1,"%s\n", svc_strings[type]);
				Dprintf(1,"LEVEL %i :  ",MSG_ReadByte());
				Dprintf(1,"%s\n",MSG_ReadString());
				break;

			case svc_spawnstaticsound:
				Dprintf(1,"%s\n", svc_strings[type]);
				MSG_ReadCoord();
				MSG_ReadCoord();
				MSG_ReadCoord();
				MSG_ReadByte();
				MSG_ReadByte();
				MSG_ReadByte();

				break;

			case svc_updateping:
				Dprintf(1,"%s\n", svc_strings[type]);
				players[MSG_ReadByte()].ping = MSG_ReadShort();
				break;

			case svc_updatepl:
				Dprintf(1,"%s\n", svc_strings[type]);
				players[MSG_ReadByte()].pl = MSG_ReadByte();
				break;

			case svc_updateentertime:
				Dprintf(1,"%s\n", svc_strings[type]);
				players[MSG_ReadByte()].entertime = MSG_ReadFloat();
				break;

			case svc_updateuserinfo:
				Dprintf(1,"%s\n", svc_strings[type]);
				Update_Userinfo();
				break;

			case svc_setinfo:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Setinfo();
				break;

			case svc_centerprint:
				Dprintf(1,"%s\n", svc_strings[type]);
				Dprintf(1,"%s\n", MSG_ReadString());
				break;

			case svc_soundlist:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Soundlist();
				break;

			case svc_modellist:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Modellist();
				break;

			case svc_smallkick:
				Dprintf(1,"%s\n", svc_strings[type]);
				break;

			case svc_bigkick:
				Dprintf(1,"%s\n", svc_strings[type]);
				break;

			case svc_chokecount:
				Dprintf(1,"%s\n", svc_strings[type]);
				Dprintf(1,"%i\n", MSG_ReadByte());
				break;

			case svc_stopsound:
				Dprintf(1,"%s\n", svc_strings[type]);
				Dprintf(1,"%i\n", MSG_ReadShort());
				break;

			case svc_setangle:
				Dprintf(1,"%s\n", svc_strings[type]);
				MSG_ReadAngle();
				MSG_ReadAngle();
				MSG_ReadAngle();
				break;


			case svc_lightstyle:
				Dprintf(1,"%s\n", svc_strings[type]);
//                      Dprintf(1,"%4i: %s\n",MSG_ReadByte(),MSG_ReadString());
//                      msg_readcount++;
				MSG_ReadByte();
				MSG_ReadString();
				break;


			case svc_muzzleflash:
				Dprintf(1,"%s\n", svc_strings[type]);
				Dprintf(1,"%4i\n", MSG_ReadShort());
				break;

			case svc_damage:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Damage();
				break;


			case svc_spawnstatic:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Static();
				break;

			case svc_spawnbaseline:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Spawnbaseline();
				break;

			case svc_deltapacketentities:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Packetentities(true);
				break;

			case svc_sound:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_Sound();
				break;

			case svc_disconnect:
				Dprintf(1,"%s\n", svc_strings[type]);
				break;

			case svc_entgravity:
				Dprintf(1,"%s\n", svc_strings[type]);
				MSG_ReadFloat();
				break;

			case svc_maxspeed:
				Dprintf(1,"%s\n", svc_strings[type]);
				MSG_ReadFloat();
				break;

			case svc_setview:
				Dprintf(1,"%s\n", svc_strings[type]);
				
				break;

			case svc_nop:
				Dprintf(1,"%s\n", svc_strings[type]);
				break;

			case svc_temp_entity:
				Dprintf(1,"%s\n", svc_strings[type]);
				Parse_TEnt();
				break;


			case svc_nails:
				Dprintf(1,"%s\n", svc_strings[type]);
				MSG_ReadByte();
				MSG_ReadByte();
				MSG_ReadByte();
				MSG_ReadByte();
				MSG_ReadByte();
				MSG_ReadByte();
				MSG_ReadByte();
				break;
				

			/*case svc_time:
				Dprintf(1,"%s\n", svc_strings[type]);
				MSG_ReadFloat();
				break;
	*/
			case nq_svc_signonnum:
				Dprintf(1,"%s\n", svc_strings[type]);
				MSG_ReadByte();
				break;

			default:
				Dprintf(1,"unknown type %i\n", type);
				packet->has_unknown_types = 1;
				debug_unknown(packet,msg_readcount);
				return;




		}

		if (record)
		{
			dptr = malloc(sizeof(char) * (msg_readcount - old_readcount));
			memcpy(dptr, &net_message.data[old_readcount], msg_readcount - old_readcount);
			Create_Packet_Struct(packet, type, dptr, msg_readcount - old_readcount);

		}
	}

		Dprintf(1,"%3i - recorded messages\n",i);

}


void Decrypt_Incoming(packet_struct_t * packet)
{
	Split_Incoming(packet, true);
};

/*
	outgoing
*/
extern bool print_header_info;


void Split_Outgoing(packet_struct_t * packet, bool record)
{
	usercmd_t old, new;
	int old_readcount, type;
	byte *dptr;

	int sequence, sequence_ack;
	int reliable_ack, reliable_message;
	extern bool send_new_packet;


	int i = 0;

	extern message_t net_message;



	
	net_message.cursize = packet->payload_size;
	net_message.data = packet->payload;
	
	MSG_BeginReading();
	
	//for (i=0;i<=packet->payload_size;i++)
	//	printf ("%3i : %3d\n",i,net_message.data[i]);

	if (record)
	{
		
		sequence = MSG_ReadLong();
		sequence_ack = MSG_ReadLong();
		int port = MSG_ReadShort();

		

		reliable_message = sequence >> 31;
		reliable_ack = sequence_ack >> 31;

		sequence &= ~(1 << 31);
		sequence_ack &= ~(1 << 31);

		packet->sequence              = sequence;
		packet->sequence_ack          = sequence_ack;

		packet->reliable_message      = reliable_message;
		packet->reliable_ack          = reliable_ack;
		
		Dprintf(1,"\nOUT <-- s=%i(%i) a=%i(%i) %i %i\n",packet->sequence,packet->reliable_message,packet->sequence_ack,\
		packet->reliable_ack, net_message.cursize, port);
		Dprintf(1,"Decrypt Outgoing:\n");



	}



	
	while (1)
	{
		old_readcount = msg_readcount;
		type = MSG_ReadByte();
		if (type == -1)
			break;
		i++;
		switch (type)
		{
			case clc_nop:
				Dprintf(1,"clc_nop\n");
				break;

			case clc_delta:
				Dprintf(1,"clc_deltasequence %i\n", MSG_ReadByte());
				break;

			case clc_move:
				Dprintf(1,"clc_move\n");
				MSG_ReadByte();
				MSG_ReadByte();
				MSG_ReadDeltaUsercmd_To_Server(&old, &new, protoversion);
				MSG_ReadDeltaUsercmd_To_Server(&old, &new, protoversion);
				MSG_ReadDeltaUsercmd_To_Server(&old, &new, protoversion);
				break;

			case clc_stringcmd:
				Dprintf(1,"clc_stringcmd\n");
				Dprintf(1,"	--> %s\n", MSG_ReadString());
				break;

			case clc_tmove:
				Dprintf(1,"clc_tmove\n");
				MSG_ReadCoord();
				MSG_ReadCoord();
				MSG_ReadCoord();
				//MSG_ReadShort();
				break;

			case clc_upload:
				Dprintf(1,"clc_upload\n");
				break;

			default:
				Dprintf(1,"clc_unknown %i\n", type);
				packet->has_unknown_types = 1;
				record = 0;
				send_new_packet = 0;
				return;

		}

		if (record)
		{
			dptr = malloc(sizeof(char) * (msg_readcount - old_readcount));
			memcpy(dptr, &net_message.data[old_readcount], msg_readcount - old_readcount);
			Create_Packet_Struct(packet, type, dptr, msg_readcount - old_readcount);

		}
	}

	Dprintf(1,"%3i - recorded messages\n",i);



}

bool is_outgoing;

void Decrypt_Outgoing(packet_struct_t * packet)
{
	
	Split_Outgoing(packet, true);

};
