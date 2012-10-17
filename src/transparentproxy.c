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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <linux/if_ether.h>
#include <linux/netfilter.h>		/* for NF_ACCEPT */
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netdb.h>

#include <libnetfilter_queue/libnetfilter_queue.h>

#include "packet_handler.h"
#include "structs.h"
#include "cmdline.h"
#include "debugging.h"
#include "ncurses_view.h"

typedef unsigned char byte ;

int current_packet_id;
int	pid;

struct nfq_q_handle *gqh;

struct pseudohdr
{
	unsigned int saddr;
	unsigned int daddr;
	unsigned char zero;
	unsigned char protocol;
	unsigned short int length;
};


unsigned short csum (unsigned short *buf, int nwords)
{
	unsigned long sum;
	for (sum = 0; nwords > 0; nwords--) sum += *buf++;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}


u_long host_to_ip(char *host_name)
{
	static  u_long ip_bytes;
	struct hostent *res;
	res = gethostbyname(host_name);
	if (res == NULL) return (0);
	memcpy(&ip_bytes, res->h_addr, res->h_length);
	return (ip_bytes);
}

unsigned short checksum(unsigned short *addr, int len)
{
   int nleft=len;
   int sum=0;
   unsigned short *w=addr;
   unsigned short answer=0;

   while(nleft>1){
      sum+=*w++;
      nleft-=2;
   }
   if(nleft==1){
      *(unsigned char *)(&answer)=*(unsigned char *)w;
      sum+=answer;     
   }
   sum=(sum>>16)+(sum&0xffff);
   sum+=(sum>>16);
   answer=~sum;
   return answer;
}

static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
	      struct nfq_data *nfa, void *data)
{
	

	struct nfqnl_msg_packet_hdr *ph;
 	
	

	u_int32_t ret;
	byte *payload;
	byte new_payload[65535];
	int new_payload_size;
	int i;
	char *dst_ip,*src_ip;
	int x,y,z;
	static unsigned int packetcount;
	byte pseudodatagram[65535];
	struct pseudohdr *pseudoheader=(struct pseudohdr *) pseudodatagram;
	u_long	src_addr,dst_addr;
	
	u_int32_t id ;
	
	gqh = qh;

	pid = getpid();
	
	
	ph = nfq_get_msg_packet_hdr(nfa);
	if (ph){
		current_packet_id = id = ntohl(ph->packet_id);
	}

	ret = nfq_get_payload(nfa, &payload);


	Dprintf(0,"packet %d start\n",packetcount++);
	

	if (payload[sizeof(struct ip) + sizeof(struct udphdr)] == 255)
		if (payload[sizeof(struct ip) + sizeof(struct udphdr)+1] == 255)
			if (payload[sizeof(struct ip) + sizeof(struct udphdr)+2] == 255)
				if (payload[sizeof(struct ip) + sizeof(struct udphdr)+3] == 255){
			nfq_set_verdict(qh, id, NF_ACCEPT,0 , 0);
			return 0;
		}

	Dprintf(2,"ip  old checksum: %d\n",x = ((struct ip *)payload)->ip_sum);
	
	dst_ip	= strdup(inet_ntoa(((raw_packet_header_t *) payload)->iphdr.ip_dst));
	src_ip	= strdup(inet_ntoa(((raw_packet_header_t *) payload)->iphdr.ip_src));
	
	Dprintf(2,"%s -> %s\n",src_ip,dst_ip);
	memset(&new_payload,0,65535);
	memset(&pseudodatagram,0,65535);
	z = Handle_Packet((unsigned char *)payload,ret,(unsigned char *)&new_payload,&new_payload_size);


	((struct ip *)new_payload)->ip_len = htons(new_payload_size);

	// fixing ip header
	Dprintf(2,"udp old checksum: %d\n",((raw_packet_header_t *) payload)->udp.check);
	Dprintf(2,"udp old len: %d\n",ntohs(((raw_packet_header_t *) payload)->udp.len ));

	src_addr = host_to_ip(src_ip);
	dst_addr = host_to_ip(dst_ip);

	x = ntohs(((raw_packet_header_t *) new_payload)->udp.len);
	// new udp length
	
	if (ret != new_payload_size)
		((raw_packet_header_t *) new_payload)->udp.len = htons(ntohs(((raw_packet_header_t *) payload)->udp.len) - (ret-new_payload_size));
	else
		((raw_packet_header_t *) new_payload)->udp.len = ((raw_packet_header_t *) payload)->udp.len ;
	y = ntohs(((raw_packet_header_t *) new_payload)->udp.len);

	memcpy(&pseudoheader->saddr, &src_addr, 4);
	memcpy(&pseudoheader->daddr, &dst_addr, 4);

	((raw_packet_header_t *) new_payload)->udp.check = 0;
	
	pseudoheader->length = ((raw_packet_header_t *) new_payload)->udp.len;
	pseudoheader->protocol = 17;
	
	// copy pseudoheader
	memcpy(((unsigned char * )pseudoheader)+sizeof( struct pseudohdr),(byte *)&new_payload + sizeof(struct ip),ntohs( pseudoheader->length));
	
	// calc udp checksum
	((raw_packet_header_t *) new_payload)->udp.check = checksum((unsigned short *)pseudoheader,sizeof(struct pseudohdr) + ntohs(((raw_packet_header_t *) new_payload)->udp.len));
	Dprintf(2,"%d %d\n",sizeof(struct pseudohdr) ,ntohs(((raw_packet_header_t *) new_payload)->udp.len));

	Dprintf(2,"udp new checksum: %d\n",((raw_packet_header_t *) new_payload)->udp.check );
	Dprintf(2,"udp new len: %d\n",ntohs(((raw_packet_header_t *) new_payload)->udp.len ));

	// calc ip checksum
	((struct ip *)new_payload)->ip_sum = 0; 
	((struct ip *)new_payload)->ip_sum = checksum((unsigned short*)new_payload,sizeof(struct ip)); 

	Dprintf(2,"%d %d\n",((struct ip *)payload)->ip_len % 255,((struct ip *)new_payload)->ip_len % 255);
	Dprintf(2,"ip  new checksum: %d\n",y = ((struct ip *)new_payload)->ip_sum);

	

	if (debug_level > 2 ){
		printf("memcmp old and new: %d\n",x = memcmp(new_payload,payload,ret));

		
			//printf("?%3i: %3i -- %3i\n",i,payload[i],new_payload[i]);
		if (debug_level > 3){
			for (i=0;i<=ret;i++){
				if (i==0)
					printf("IP Header start \n");

				if (i==sizeof(struct ip))
					printf("UDP Header start \n");
				if (payload[i] == new_payload[i])
					printf("	%3i: %3i -- %3i\n",i,payload[i],new_payload[i]);
				else
					printf("	%3i: %3i -- %3i <-----\n",i,payload[i],new_payload[i]);
				if (i==(sizeof(struct ip) + sizeof(struct udphdr) -1))
					printf("Header stop \n");
			}
			//exit(1);
		}
	}

	Dprintf(0,"packet stop\n\n");

	
		
	nfq_set_verdict(qh, id, NF_ACCEPT,new_payload_size , new_payload);


	free(dst_ip);
	free(src_ip);
	//free(payload);
	update_display();
	return 0;
}

int main(int argc, char **argv)
{
	struct nfq_handle *h;
	struct nfq_q_handle *qh;
	struct nfnl_handle *nh;
	int fd;
	int rv;
	char buf[65535];

	Parse_Cmdline ( argc, argv);

	screen_init();


	Dprintf(2,"opening library handle\n");
	h = nfq_open();
	if (!h) {
		fprintf(stderr, "error during nfq_open()\n");
		exit(1);
	}

	Dprintf(2,"unbinding existing nf_queue handler for AF_INET (if any)\n");
	if (nfq_unbind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_unbind_pf()\n");
		exit(1);
	}

	Dprintf(2,"binding nfnetlink_queue as nf_queue handler for AF_INET\n");
	if (nfq_bind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_bind_pf()\n");
		exit(1);
	}

	Dprintf(2,"binding this socket to queue '0'\n");
	qh = nfq_create_queue(h,  0, &cb, NULL);
	if (!qh) {
		fprintf(stderr, "error during nfq_create_queue()\n");
		exit(1);
	}

	Dprintf(2,"setting copy_packet mode\n");
	if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 65535) < 0) {
		fprintf(stderr, "can't set packet_copy mode\n");
		exit(1);
	}

	nh = nfq_nfnlh(h);
	fd = nfnl_fd(nh);

	while ((rv = recv(fd, buf, sizeof(buf), 0)) && rv >= 0) {
		//printf("pkt received\n");
		nfq_handle_packet(h, buf, rv);
	}

	Dprintf(2,"unbinding from queue 0\n");
	nfq_destroy_queue(qh);

#ifdef INSANE
	/* normally, applications SHOULD NOT issue this command, since
	 * it detaches other programs/sockets from AF_INET, too ! */
	printf("unbinding from AF_INET\n");
	nfq_unbind_pf(h, AF_INET);
#endif

	printf("closing library handle\n");
	nfq_close(h);

	exit(0);
}

