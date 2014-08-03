/**************************************************************************
* Copyright (C) 2008 thierry lorthiois (lorthiois@bbsoft.fr)
*
* taskbar
*
**************************************************************************/

#ifndef TASKBAR_H
#define TASKBAR_H

#include "task.h"
#include "taskbarname.h"

enum { TASKBAR_NORMAL, TASKBAR_ACTIVE, TASKBAR_STATE_COUNT };
extern GHashTable* win_to_task_table;
extern Task* task_active;
extern Task* task_drag;
extern int taskbar_enabled;

class TaskbarBase : public Area {
    Pixmap state_pixmap_[TASKBAR_STATE_COUNT];

  public:
    Pixmap state_pixmap(size_t i) const;
    TaskbarBase& set_state_pixmap(size_t i, Pixmap value);
    TaskbarBase& reset_state_pixmap(size_t i);
};

class Taskbarname : public TaskbarBase {
    std::string name_;

  public:
    std::string const& name() const;
    Taskbarname& set_name(std::string const& name);
};

// tint3 uses one taskbar per desktop.
class Taskbar : public TaskbarBase {
  public:
    int desktop;
    int text_width;
    Taskbarname bar_name;
};

class Global_taskbar : public TaskbarBase {
  public:
    Area area_name;
    Background* background[TASKBAR_STATE_COUNT];
    Background* background_name[TASKBAR_STATE_COUNT];
};


// default global data
void default_taskbar();

// freed memory
void cleanup_taskbar();

void init_taskbar();
void init_taskbar_panel(void* p);

void draw_taskbar(void* obj, cairo_t* c);
void taskbar_remove_task(gpointer key, gpointer value, gpointer user_data);
Task* task_get_task(Window win);
GPtrArray* task_get_tasks(Window win);
void task_refresh_tasklist();

int  resize_taskbar(void* obj);
void on_change_taskbar(void* obj);
void set_taskbar_state(Taskbar* tskbar, size_t state);

// show/hide taskbar according to current desktop
void visible_taskbar(void* p);


#endif

