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
typedef struct  raw_packet_header_s
{
    struct ip     iphdr;
    struct udphdr  udp;
} raw_packet_header_t;

typedef struct packet_type_struct_s
{
    struct packet_type_struct_s	*next;
    unsigned char			type;
    unsigned char			*data;
    bool			ignore;
    int			length;

} packet_type_struct_t;

typedef struct packet_struct_s
{

    int         netfilter_id;

    unsigned char        *packet_backup;     //original recieved from netfilter with headers
    int         packet_length;

    unsigned char        *new_packet;        //if rebuild the new packet will be here
    int         new_packet_size;

    unsigned char        *payload;           //packet data without header
    int         payload_size;

    bool        has_unknown_types;  //when the packet is examined and an unknown type has come up this will be set to 1

    struct      raw_packet_header_s   *raw_pkhdr;       //
    struct      packet_type_struct_s  *chained_packet;  //  the packet will be

	bool	type;			// IN || OUT

	int	header_size;


	unsigned                sequence, sequence_ack;
	unsigned                reliable_ack, reliable_message;


} packet_struct_t;
