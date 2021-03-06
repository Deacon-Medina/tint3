#ifndef TINT3_CLOCK_CLOCK_HH
#define TINT3_CLOCK_CLOCK_HH

#include <string>

#include "util/area.hh"
#include "util/common.hh"
#include "util/pango.hh"
#include "util/timer.hh"

class Clock : public Area {
 public:
  Color font_;
  int time1_posy_;
  int time2_posy_;

  void DrawForeground(cairo_t*) override;
  std::string GetTooltipText() override;
  bool Resize() override;

  bool HandlesClick(XEvent* event) override;
  bool OnClick(XEvent* event) override;

  static void InitPanel(Panel* panel);

#ifdef _TINT3_DEBUG

  std::string GetFriendlyName() const override;

#endif  // _TINT3_DEBUG

 private:
  std::string time1_;
  std::string time2_;
};

extern std::string time1_format;
extern std::string time1_timezone;
extern std::string time2_format;
extern std::string time2_timezone;
extern std::string time_tooltip_format;
extern std::string time_tooltip_timezone;
extern std::string clock_lclick_command;
extern std::string clock_rclick_command;
extern util::pango::FontDescriptionPtr time1_font_desc;
extern util::pango::FontDescriptionPtr time2_font_desc;
extern bool clock_enabled;

// default global data
void DefaultClock();

// freed memory
void CleanupClock(Timer& timer);

// initialize clock : y position, precision, ...
void InitClock(Timer& timer);

#endif  // TINT3_CLOCK_CLOCK_HH
