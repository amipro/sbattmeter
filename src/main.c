/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Ahmed AbdelHamid 2010 <ahmedam@mail.usa.com>
 * 
 * SBattMeter is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SBattMeter is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include <gtk/gtk.h>



/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif


#define ICONS_PATH "/usr/share/sbattmeter/icons/"
#define SYS_BATT_PATH "/sys/class/power_supply/battery/"

typedef enum {BATT_NORMAL=0, BATT_CHARGING=1} BATT_STATUS;

GtkStatusIcon *status_icon_batt_meter;

int get_batt_level()
{
	int ret=-1;
	FILE *f=fopen(SYS_BATT_PATH "capacity","r");
	if(!f){
		return -1;
	}
	fscanf(f,"%d",&ret);
	fclose(f);

	return ret;
}

static BATT_STATUS get_batt_status()
{
	int ret=-1;
	FILE *f=fopen(SYS_BATT_PATH "charging_enabled","r");
	if(!f){
		return -1;
	}
	fscanf(f,"%d",&ret);
	fclose(f);

	return ret?BATT_CHARGING:BATT_NORMAL;
}

/*
 Battery levels:
 0 .. 10		empty
 10 .. 20	0
 20 .. 40	1
 40 .. 60	2
 60 .. 80	3
 80 .. 100	4
 100			5
 */
void update_status_icon(int level, BATT_STATUS status)
{
	gchar *icon;
	static int old_level=-1;
	static BATT_STATUS old_status=BATT_NORMAL;

	if(level==old_level && status==old_status)
		return;

	old_level=level;
	old_status=status;

	if(level<10 && status==BATT_NORMAL){
		icon=g_strdup("_empty");
	}else if(level==100){
		icon=g_strdup("_5");
	}else if(status==BATT_CHARGING){
		icon=g_strdup_printf("_ch_%d",level/20);
	}else{
		icon=g_strdup_printf("_%d",level/20);
	}

	gchar *path=g_strdup_printf("%sbatt%s.png",ICONS_PATH,icon);
	if(!status_icon_batt_meter)
		status_icon_batt_meter=gtk_status_icon_new_from_file(ICONS_PATH "network-strength-none.png");
	else
		gtk_status_icon_set_from_file(status_icon_batt_meter,path);

	gchar *tip=g_strdup_printf("%d%%",level);
	gtk_status_icon_set_tooltip_text(status_icon_batt_meter,tip);

	g_free(tip);
	g_free(path);
	g_free(icon);
}

static int batt_monitor(gpointer data)
{
	int level=get_batt_level();
	BATT_STATUS status=get_batt_status();

	update_status_icon(level,status);
	
	return TRUE;
}

int
main (int argc, char *argv[])
{
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	
	gtk_set_locale ();
	gtk_init (&argc, &argv);

	int level=get_batt_level();
	BATT_STATUS status=get_batt_status();

	if(level<0 || status<0){
		fprintf(stderr, "Error opening battery status files\n");
		return 1;
	}

	g_timeout_add_seconds(2,batt_monitor, NULL);
	
	gtk_main ();
	return 0;
}
