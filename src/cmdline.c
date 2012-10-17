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
extern 	int	debug_level;
char		*listen_ip;
extern	int	use_ncurses;

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int Parse_Switch(char *sswitch, char *argument){

	switch (sswitch[1]){
		case 'd':
			debug_level = atoi(argument);
			return 1;
	
		case 'i':
			listen_ip = strdup(argument);
			return 1;

		case 'n':
			use_ncurses = 1;
			return 0;



	}

	return 0;

}



void Parse_Cmdline ( int argc, char **argv){
	int i;
	for (i = 1; i< argc;i++)
		if (argv[i][0] == '-')
			i += Parse_Switch(argv[i],argv[i+1]);




}
