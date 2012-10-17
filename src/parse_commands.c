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

#define playermodel 1024;
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "msg_functions.h"

int protoversion ;
int numsounds;
int nummodels;
int type;
extern bool msg_badread;
extern int msg_readcount;

typedef struct player_info_s	{
	vec3_t	origin;
	vec3_t	viewangle;
	vec3_t	velocity;
	int	stats[1024];
	int 	frame;
	int	modelindex;
	int	skinnum;
	int	effects;
	int	weaponframe;
	int	frags;
	int	ping;
	int	pl;
	float	entertime;
	int	userid;
	char	*userinfo;
	char 	*name;

}	player_info_t;

player_info_t	players[32];


char *Get_Userinfo_Value_By_Key(char *key, char *userinfo)
{
  char buf[128];
  char *index = NULL;
  char *index_org = NULL;
  int _size = 0;


  sprintf(buf,"\\%s\\",key);
  index =       strstr(userinfo,buf);
  //printf("Getuserinfo: %s in %s\n",key,userinfo);
  //printf("%i\n",index);
  if (index == NULL)
    return NULL;
  index += strlen(buf);
  index_org = index;
  while (*index != '\\' && *index != 0)
    {
      _size++;
      index++;
    }

  return strndup(index_org,_size);

}



void Parse_Playerinfo (void){
	int num,flags, i;
	usercmd_t nullcmd,command;
	extern int gflags;

	player_info_t *p;

	num = MSG_ReadByte();



	p = &players[num];

	gflags = flags = MSG_ReadShort ();
	p->origin[0] = MSG_ReadCoord ();
	p->origin[1] = MSG_ReadCoord ();
	p->origin[2] = MSG_ReadCoord ();



	p->frame = MSG_ReadByte ();
	
	if (flags & PF_MSEC) {

		MSG_ReadByte ();

	} else {

	}

	if (flags & PF_COMMAND)
			MSG_ReadDeltaUsercmd (&nullcmd, &command, protoversion);


	for (i = 0; i < 3; i++) {
		if (flags & (PF_VELOCITY1 << i) )
			p->velocity[i] = MSG_ReadShort();
		else
			p->velocity[i] = 0;
	}
	if (flags & PF_MODEL)
		p->modelindex = MSG_ReadByte ();
	else
		p->modelindex = playermodel;
	if (flags & PF_SKINNUM)
		p->skinnum = MSG_ReadByte ();
	else
		p->skinnum = 0;
	if (flags & PF_EFFECTS)
		p->effects = MSG_ReadByte ();
	else
		p->effects = 0;
	if (flags & PF_WEAPONFRAME)
		p->weaponframe = MSG_ReadByte ();
	else
		p->weaponframe = 0;


		

}

void Parse_Packetentities (bool delta){
	int	from;
	int	word;
	int	bits;
	int	i;


	if (delta) {
		from = MSG_ReadByte ();

	} else {
	}


	while (1) {
		word = (unsigned short) MSG_ReadShort ();
		//printf("%i %i\n",msg_readcount,word);
		

		if (!word) {
			break;
		}

		if (msg_badread) {
			// something didn't parse right...
			printf ("msg_badread in packetentities\n");
			break;//exit(1);
		}

		word &= ~511;

		bits = word;

	              if (bits & U_MOREBITS)
	                {
	                  i = MSG_ReadByte();
	                  bits |= i;
	                }

	              if (bits & U_MODEL)
	                MSG_ReadByte();
	              if (bits & U_FRAME)
	                MSG_ReadByte();
	              if (bits & U_COLORMAP)
	                MSG_ReadByte();
	              if (bits & U_SKIN)
	                MSG_ReadByte();
	              if (bits & U_EFFECTS)
	                MSG_ReadByte();
	              if (bits & U_ORIGIN1)
	                MSG_ReadCoord();
	              if (bits & U_ORIGIN2)
	                MSG_ReadCoord();
	              if (bits & U_ORIGIN3)
	                MSG_ReadCoord();
	              if (bits & U_ANGLE1)
	                MSG_ReadAngle();
	              if (bits & U_ANGLE2)
	                MSG_ReadAngle();
	              if (bits & U_ANGLE3)
	                MSG_ReadAngle();
		
	
		
	}
}


void	Updatestat_Long(void){
	int i,j;
	i = MSG_ReadByte();
	j = MSG_ReadLong();
	//dprintf("%i %i\n",i,j);
	

	//cl.stats[stat] = value;


}

void	Updatestat(void){
	int i,j;
	i = MSG_ReadByte();
	j = MSG_ReadByte();
	//dprintf("%i %i\n",i,j);
	

	//cl.stats[stat] = value;


}

void	Parse_Serverinfo(void){
	char	*key,*value;

	key = 	strdup(MSG_ReadString());
	value = strdup(MSG_ReadString());

	//dprintf("key: %s - value : %s\n",key,value);
	free(key);
	free(value);

}

void	Parse_Serverdata(void){
	protoversion = MSG_ReadLong();			// protokol version
	MSG_ReadLong();			// server_count
	MSG_ReadString();		// gamedir
	MSG_ReadByte();			// playernumber
	MSG_ReadString();		// map name
	MSG_ReadFloat();		// gravity
	MSG_ReadFloat();		// stopspeed
	MSG_ReadFloat();		// maxspeed
	MSG_ReadFloat();		// spectatormaxspeed
	MSG_ReadFloat();		// accelerate
	MSG_ReadFloat();		// airaccelerate
	MSG_ReadFloat();		// wateraccelerate
	MSG_ReadFloat();		// friction
	MSG_ReadFloat();		// waterfriction
	MSG_ReadFloat();		// entgravity


}

void	Update_Userinfo(void){

	int num;
	player_info_t *p;
	num = MSG_ReadByte();
	p = &players[num];
	p->userid = MSG_ReadLong();
	if (p->userinfo)
		free(p->userinfo);
	p->userinfo = strdup(MSG_ReadString());
	p->name = Get_Userinfo_Value_By_Key("name",p->userinfo);

}	

void Parse_Setinfo(void){
	int num;
	char *s,*t;
	//player_info_t *p;
	num = MSG_ReadByte();
	//dprintf("%s %s\n",MSG_ReadString() ,MSG_ReadString());
	s=strdup(MSG_ReadString());
	t=strdup(MSG_ReadString());
	if (strstr(s,"name")){
		if(players[num].name)
			free(players[num].name);
		players[num].name = t;
	}
	free(s);
		
}


void Parse_Soundlist(void){
	char	*str;
	numsounds = MSG_ReadByte();
	
	while(1){
		str = MSG_ReadString();
		if(!str[0])
			break;
		//dprintf("%s\n",str);
		numsounds++;
	}

	

	MSG_ReadByte();
}

void Parse_Modellist(void){
	char	*str;
	nummodels = MSG_ReadByte();
	
	while(1){
		str = MSG_ReadString();
		if(!str[0])
			break;
		nummodels++;
		//dprintf("%s\n",str);
	}

	MSG_ReadByte();

}

void Parse_Damage(void){
	MSG_ReadByte();			//armor
	MSG_ReadByte();			//blood
	MSG_ReadCoord();		//coords
	MSG_ReadCoord();
	MSG_ReadCoord();
}


void Parse_Static(void){
	MSG_ReadByte();
	MSG_ReadByte();
	MSG_ReadByte();
	MSG_ReadByte();
	MSG_ReadCoord();
	MSG_ReadAngle();
	MSG_ReadCoord();
	MSG_ReadAngle();
	MSG_ReadCoord();
	MSG_ReadAngle();


}



void Parse_Spawnbaseline(void){
	MSG_ReadShort();
	MSG_ReadByte();
	MSG_ReadByte();
	MSG_ReadByte();
	MSG_ReadByte();
	MSG_ReadCoord();
	MSG_ReadAngle();
	MSG_ReadCoord();
	MSG_ReadAngle();
	MSG_ReadCoord();
	MSG_ReadAngle();


}

void Parse_TEnt(void){
	int type = MSG_ReadByte();

	switch (type){
		case 0:
		case 1:
		case 3:
		case 4:
		case 8:
		case 10:
		case 11:
		case 13:
			MSG_ReadCoord();
			MSG_ReadCoord();
			MSG_ReadCoord();
		break;

		case 5:
		case 6:
		case 7:
			MSG_ReadShort();
			MSG_ReadCoord();
			MSG_ReadCoord();
			MSG_ReadCoord();
			MSG_ReadCoord();
			MSG_ReadCoord();
			MSG_ReadCoord();
		break;

		case 2:
		case 12:
			MSG_ReadByte();
			MSG_ReadCoord();
			MSG_ReadCoord();
			MSG_ReadCoord();
		break;

}
}

void Parse_Sound(void){
	int channel;
				
				channel = MSG_ReadShort();
				if (channel & SND_VOLUME)
					MSG_ReadByte();
				if (channel & SND_ATTENUATION)
					MSG_ReadByte();
				MSG_ReadByte();
				MSG_ReadCoord();
				MSG_ReadCoord();
				MSG_ReadCoord();

}


































