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
#define __LITTLE_ENDIAN__Q__
#ifdef __BIG_ENDIAN__Q__
#define BigShort(x)(x)
#define BigLong(x)(x)
#define BigFloat(x)(x)
#define LittleShort(x)ShortSwap(x)
#define LittleLong(x)LongSwap(x)
#define LittleFloat(x)FloatSwap(x)
#elif defined(__LITTLE_ENDIAN__Q__)
#define BigShort(x)ShortSwap(x)
#define BigLong(x)LongSwap(x)
#define BigFloat(x)FloatSwap(x)
#define LittleShort(x)(x)
#define LittleLong(x)(x)
#define LittleFloat(x)(x)
#elif defined(__PDP_ENDIAN__Q__)
#define BigShort(x)ShortSwap(x)
#define BigLong(x)LongSwapPDP2Big(x)
#define BigFloat(x)FloatSwapPDP2Big(x)
#define LittleShort(x)(x)
#define LittleLong(x)LongSwapPDP2Lit(x)
#define LittleFloat(x)FloatSwapPDP2Lit(x)
#else
#error Unknown byte order type!
#endif

#include <stdbool.h>
#include "protocol.h"


#define DEBUG 1

#define CM_MSEC	(1 << 7)



#include <sys/time.h>


typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec5_t[5];

typedef unsigned char byte;



typedef struct sizebuf_s
  {
    bool	allowoverflow;	// if false, do a Sys_Error
    bool	overflowed;		// set to true if the buffer size failed
    byte	*data;
    int		maxsize;
    int		cursize;
  } sizebuf_t;



typedef struct message_s {
	unsigned char		*data;
	int		cursize;

} message_t;




#define VectorCopy(a,b)                       ((b)[0] = (a)[0], (b)[1] = (a)[1], (b)[2] = (a)[2])

#define VectorSubtract(a,b,c) ((c)[0] = (a)[0] - (b)[0], (c)[1] = (a)[1] - (b)[1], (c)[2] = (a)[2] - (b)[2])


typedef struct temp_entity_s
{
	vec3_t	pos;	// Position of temp entity.
	float	time;	// Time of temp entity.
	int		type;	// Type of temp entity.
} temp_entity_t;

typedef struct temp_entity_list_s
{
	temp_entity_t	list[MAX_TEMP_ENTITIES];
	int				count;
} temp_entity_list_t;

temp_entity_list_t	temp_entities;



// entity_state_t is the information conveyed from the server
// in an update message
typedef struct entity_state_s {
	int		number;			// edict index
	int		flags;			// nolerp, etc
	vec3_t	origin;
	vec3_t	angles;
	int		modelindex;
	int		frame;
	int		colormap;
	int		skinnum;
	int		effects;
} entity_state_t;

#ifdef SERVERONLY

#define	MAX_PACKET_ENTITIES	64

typedef struct packet_entities_s {
	int		num_entities;
	entity_state_t	entities[MAX_PACKET_ENTITIES];
} packet_entities_t;

#else

#define	MAX_MVD_PACKET_ENTITIES	300		
#define	MAX_PACKET_ENTITIES	64

typedef struct packet_entities_s {
	int		num_entities;
	entity_state_t	entities[MAX_MVD_PACKET_ENTITIES];
} packet_entities_t;

#endif

typedef struct usercmd_s {
	byte	msec;
	vec3_t	angles;
	short	forwardmove, sidemove, upmove;
	byte	buttons;
	byte	impulse;
} usercmd_t;








