#ifndef __INIT_H
#define __INIT_H
#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240

#define ZOOM_POINT_WIDTH                (64)
#define ZOOM_POINT_HEIGHT               (102)
#define ZOOM_POINT_POINTER_SIZE         (3)
#define ZOOM_POINT_COLOR_SELECT         COLOR_RGB1555(1,  0,  0, 31)
#define ZOOM_POINT_COLOR_WAIT           COLOR_RGB1555(1, 31,  0,  0)
#define ZOOM_POINT_COLOR_HIGHLIGHT      COLOR_RGB1555(1,  0, 31,  0)

#define VDP1_CMDT_ORDER_SYSTEM_CLIP_COORDS_INDEX        0
#define VDP1_CMDT_ORDER_CLEAR_LOCAL_COORDS_INDEX        1
#define VDP1_CMDT_ORDER_CLEAR_POLYGON_INDEX             2
#define VDP1_CMDT_ORDER_LOCAL_COORDS_INDEX              3
#define VDP1_CMDT_ORDER_SPRITE_INDEX                    4
#define VDP1_CMDT_ORDER_POLYGON_POINTER_INDEX           5
#define VDP1_CMDT_ORDER_DRAW_END_INDEX                  6
#define VDP1_CMDT_ORDER_COUNT                           7

#define ZOOM_TEX_PATH   "ZOOM.TEX"
#define ZOOM_PAL_PATH   "ZOOM.PAL"
extern callback_handler_t vblank_out_handler;
extern cpu_frt_ihr_t cpu_frt_ovi_handler;
extern void _init(vdp1_vram_partitions_t _vdp1_vram_partitions);
extern uint8_t root_romdisk[];
extern void *_romdisk;
extern vdp1_cmdt_list_t *_cmdt_list;
static struct {
        int16_vec2_t position;
        color_rgb1555_t color;

        vdp1_cmdt_t *cmdt;
} _polygon_pointer;

static struct {
        uint16_t anim_frame;
        uint16_t anim_counter;

        uint16_t *tex_base;
        uint16_t *pal_base;

        vdp1_cmdt_t *cmdt;
} _sprite;
#endif