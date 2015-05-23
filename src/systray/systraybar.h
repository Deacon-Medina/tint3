/**************************************************************************
* Copyright (C) 2009 thierry lorthiois (lorthiois@bbsoft.fr)
*
* systraybar
* systray implementation come from 'docker-1.5' by Ben Jansens,
* and from systray/xembed specification (freedesktop.org).
*
**************************************************************************/

#ifndef SYSTRAYBAR_H
#define SYSTRAYBAR_H

#include <X11/extensions/Xdamage.h>

#include <list>

#include "util/common.h"
#include "util/area.h"
#include "util/timer.h"
#include "systray/tray_window.h"

// XEMBED messages
#define XEMBED_EMBEDDED_NOTIFY 0
// Flags for _XEMBED_INFO
#define XEMBED_MAPPED (1 << 0)

class Systraybar : public Area {
  void RemoveIconInternal(TrayWindow* traywin);

 public:
  std::list<TrayWindow*> list_icons;
  int sort;
  int alpha, saturation, brightness;
  int icon_size, icons_per_column, icons_per_row, margin_;

  void DrawForeground(cairo_t*) override;
  void OnChangeLayout() override;
  bool Resize() override;

  size_t VisibleIcons();
  bool AddIcon(Window id);
  void RemoveIcon(TrayWindow* traywin);
  void Clear();

  // systray protocol
  // many tray icon don't manage stop/restart of the systray manager
  void StartNet();
  void StopNet();
  void NetMessage(XClientMessageEvent* e);

  static void InitPanel(Panel* panel);

#ifdef _TINT3_DEBUG

  std::string GetFriendlyName() const override;

#endif  // _TINT3_DEBUG
};

// net_sel_win != None when protocol started
extern Window net_sel_win;
extern Systraybar systray;
extern int refresh_systray;
extern bool systray_enabled;
extern int systray_max_icon_size;

// default global data
void DefaultSystray();

// freed memory
void CleanupSystray();

// initialize protocol and panel position
void InitSystray();

void RefreshSystrayIcon();
void SystrayRenderIcon(TrayWindow* traywin);

#endif
