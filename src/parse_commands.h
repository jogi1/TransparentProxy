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

extern player_info_t players[32];
extern int protoversion;

char *Get_Userinfo_Value_By_Key(char *key, char *userinfo);




void Parse_Playerinfo (void);

void Parse_Packetentities (bool delta);

void	Updatestat_Long(void);

void	Updatestat(void);

void	Parse_Serverinfo(void);

void	Parse_Serverdata(void);

void	Update_Userinfo(void);

void Parse_Setinfo(void);


void Parse_Soundlist(void);

void Parse_Modellist(void);

void Parse_Damage(void);


void Parse_Static(void);



void Parse_Spawnbaseline(void);
void Parse_TEnt(void);

void Parse_Sound(void);

































