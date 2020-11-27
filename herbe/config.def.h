#include "../config.h"

static const char *background_color = background;
static const char *border_color = gray;
static const char *font_color = foreground;
static const char *font_pattern = toolfont;
static const unsigned line_spacing = 5;
static const unsigned int padding = 10;

static const unsigned int width = 250;
static const unsigned int border_size = 1;
static const unsigned int pos_x = 15;
static const unsigned int pos_y = 35;

enum corners { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };
enum corners corner = TOP_RIGHT;

static unsigned int duration = 5; /* in seconds */

#define DISMISS_BUTTON Button1
#define ACTION_BUTTON Button3
