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
char *svc_strings[] =
{
  "svc_bad",
  "svc_nop",
  "svc_disconnect",
  "svc_updatestat",
  "svc_version",		// [long] server version
  "svc_setview",		// [short] entity number
  "svc_sound",		// <see code>
  "svc_time",			// [float] server time
  "svc_print",		// [string] null terminated string
  "svc_stufftext",	// [string] stuffed into client's console buffer
  // the string should be \n terminated
  "svc_setangle",		// [vec3] set the view angle to this absolute value

  "svc_serverdata",	// [long] version ...
  "svc_lightstyle",	// [byte] [string]
  "svc_updatename",	// [byte] [string]
  "svc_updatefrags",	// [byte] [short]
  "svc_clientdata",	// <shortbits + data>
  "svc_stopsound",	// <see code>
  "svc_updatecolors",	// [byte] [byte]
  "svc_particle",		// [vec3] <variable>
  "svc_damage",		// [byte] impact [byte] blood [vec3] from

  "svc_spawnstatic",
  "OBSOLETE svc_spawnbinary",
  "svc_spawnbaseline",

  "svc_temp_entity",	// <variable>
  "svc_setpause",
  "svc_signonnum",
  "svc_centerprint",
  "svc_killedmonster",
  "svc_foundsecret",
  "svc_spawnstaticsound",
  "svc_intermission",
  "svc_finale",

  "svc_cdtrack",
  "svc_sellscreen",

  "svc_smallkick",
  "svc_bigkick",

  "svc_updateping",
  "svc_updateentertime",

  "svc_updatestatlong",
  "svc_muzzleflash",
  "svc_updateuserinfo",
  "svc_download",
  "svc_playerinfo",
  "svc_nails",
  "svc_choke",
  "svc_modellist",
  "svc_soundlist",
  "svc_packetentities",
  "svc_deltapacketentities",
  "svc_maxspeed",
  "svc_entgravity",

  "svc_setinfo",
  "svc_serverinfo",
  "svc_updatepl",
  "svc_nails2",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL",
  "NEW PROTOCOL"
};
