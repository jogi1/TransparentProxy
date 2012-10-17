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

int use_ncurses = 0;
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdbool.h>
#include "common.h"
#include "ncurses.h"

static WINDOW *mainwnd;
static WINDOW *screen;
WINDOW *my_win;
char memstr[1024];

double mem_usage;

int gflags;

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
	char	*name;


}	player_info_t;

extern player_info_t	players[32];

#define VERSION "0.0.1"

void screen_init(void) {


	if (!use_ncurses)
		return;

	mainwnd = initscr();
	noecho();
	cbreak();
	nodelay(mainwnd, TRUE);
	refresh(); // 1)
	wrefresh(mainwnd);
	screen = newwin(30, 75, 0, 0);
	//box(screen, ACS_VLINE, ACS_HLINE);
}

void update_status(void);

void update_display(void) {
	extern int out_packet_count;
	extern int in_packet_count;
	extern char *dst_ip;
	extern char *src_ip;
	extern int pid;
	

	int i;



	if (!use_ncurses)
		return;

	update_status();

	curs_set(0);
	mvwprintw(screen,0,0,"Transparent Quakeworld Proxy  v. %s  pid - %i - %i",VERSION,pid,gflags);
	mvwprintw(screen,1,0,"IN  %15s : %10d",dst_ip,in_packet_count);
	mvwprintw(screen,2,0,"OUT %15s : %10d",src_ip,out_packet_count);
	
	for (i=0;i<=32;i++)
		mvwprintw(screen,3+i,0,"%3i : %s",i,players[i].name);


	wrefresh(screen);
	refresh();
}

/*
double DoubleTime (void)
{
  struct timeval tp;
  struct timezone tzp;
  static int secbase;

  gettimeofday(&tp, &tzp);

  if (!secbase)
    {
      secbase = tp.tv_sec;
      return tp.tv_usec/1000000.0;
    }

  return (tp.tv_sec - secbase) + tp.tv_usec / 1000000.0;

}
*/

void update_status(void){
	static int count,last_count,real_count;

	static char stat[81];
	static char statm[81];
	static bool set_buf;

	extern int pid;

	if (!set_buf){
		sprintf(stat,"/proc/%d/stat",pid);
		sprintf(statm,"/proc/%d/statm",pid);
		set_buf = 1;
	}


	count++;

	//update every 140 packets, roughly 1 sec

	if (count >= last_count + 140)
		return;
	
	real_count++;
/*
	f = fopen(statm,"r");

	fscanf(f,"%d %d %d %d %d %d %d",&statm_i[0] ,
					&statm_i[1] ,
					&statm_i[2] ,
					&statm_i[3] ,
					&statm_i[4] ,
					&statm_i[5] ,
					&statm_i[6]);
	
	fclose(f);

	f = fopen(statm,"r");
	fgets(memstr,1024,f);
	fclose(f);
	

	cum_size += statm_i[0];
	mem_usage = (cum_size / real_count) / (1024 * 1024);
*/
	
	
	
	

}

