
#ifndef GUILIB_STDGUILIB_H
# define GUILIB_STDGUILIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <guilib.h>

void gui_draw_circle(struct gui_window *window, unsigned x, unsigned y,
		     unsigned radius, unsigned color);
void gui_draw_line(struct gui_window *window, unsigned x1, unsigned y1,
		   unsigned x2, unsigned y2, unsigned color);
void gui_draw_borders(struct gui_window *window, unsigned width, unsigned color);
float gui_get_fps(void);

#ifdef __cplusplus
}
#endif

#endif /* GUILIB_STDGUILIB_H */

