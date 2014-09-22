/**************************************************************************
* window :
* -
*
* Check COPYING file for Copyright
*
**************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <glib.h>
#include <pango/pangocairo.h>

#include <string>
#include <vector>


void set_active(Window win);
void set_desktop(int desktop);
void set_close(Window win);
int server_get_current_desktop();
std::vector<std::string> server_get_desktop_names();
int window_is_iconified(Window win);
int window_is_urgent(Window win);
int window_is_hidden(Window win);
int window_is_active(Window win);
int window_is_skip_taskbar(Window win);
int get_icon_count(gulong* data, int num);
gulong* get_best_icon(gulong* data, int icon_count, int num, int* iw, int* ih,
                      int best_icon_size);
void window_maximize_restore(Window win);
void window_toggle_shade(Window win);
void windows_set_desktop(Window win, int desktop);
int window_get_monitor(Window win);
Window window_get_active();

void get_text_size(PangoFontDescription* font, int* height_ink, int* height,
                   int panel_height, char const* text, int len);
void get_text_size2(PangoFontDescription* font, int* height_ink, int* height,
                    int* width, int panel_height, int panel_with, char const* text, int len);


#endif
