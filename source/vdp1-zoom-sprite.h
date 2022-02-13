#ifndef __VDP1_ZOOM_SPRITE
#define __VDP1_ZOOM_SPRITE
#include "init.h"








#define ANIMATION_FRAME_COUNT    (14)
#define ANIMATION_FRAME_DURATION (3)






static int16_vec2_t _display = INT16_VEC2_INITIALIZER(ZOOM_POINT_WIDTH, ZOOM_POINT_HEIGHT);
static int16_vec2_t _zoom_point = INT16_VEC2_INITIALIZER(0, 0);
static uint16_t _zoom_point_value = VDP1_CMDT_ZOOM_POINT_CENTER;
static smpc_peripheral_digital_t _digital;







static volatile uint32_t _frt_count = 0;

static void _sprite_config(void);
static void _polygon_pointer_config(void);

static uint32_t _frame_time_calculate(void);

static void _vblank_out_handler(void *);
static void _cpu_frt_ovi_handler(void);
#endif
