/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */
#include <yaul.h>
#include <stdio.h>
#include <stdlib.h>
#include "vdp1-zoom-sprite.h"

static vdp1_vram_partitions_t _vdp1_vram_partitions;


static inline bool __always_inline _digital_dirs_pressed(void)
{
        return (_digital.pressed.raw & PERIPHERAL_DIGITAL_DIRECTIONS) != 0x0000;
}



int main(void)
{
		
        _init(_vdp1_vram_partitions);
        dbgio_printf("init");
        cpu_frt_count_set(0);

        while (true) {
                //smpc_peripheral_process();
                //smpc_peripheral_digital_port(1, &_digital);

                //_sprite_config();
                //_polygon_pointer_config();

                //vdp1_sync_cmdt_list_put(_cmdt_list, 0);

                //vdp1_sync_render();

                vdp1_sync();
                vdp1_sync_wait();

                dbgio_flush();

                uint32_t result;
                result = _frame_time_calculate();

                char fixed[16];
                fix16_str(result, fixed, 7);

                dbgio_printf("[H[2J%sms\n", fixed);
        }

        return 0;
}

static uint32_t _frame_time_calculate(void)
{
        uint16_t frt;
        frt = cpu_frt_count_get();

        cpu_frt_count_set(0);

        uint32_t delta_fix;
        delta_fix = frt << 4;

        uint32_t divisor_fix;
        divisor_fix = CPU_FRT_NTSC_320_32_COUNT_1MS << 4;

        cpu_divu_32_32_set(delta_fix << 4, divisor_fix);

        /* Remember to wait ~40 cycles */
        for (uint32_t i = 0; i < 8; i++) {
                cpu_instr_nop();
                cpu_instr_nop();
                cpu_instr_nop();
                cpu_instr_nop();
                cpu_instr_nop();
        }

        uint32_t result;
        result = cpu_divu_quotient_get();
        result <<= 12;

        return result;
}

static void _sprite_config(void)
{
        vdp1_cmdt_param_char_base_set(_sprite.cmdt, (uint32_t)_sprite.tex_base);
        vdp1_cmdt_param_zoom_set(_sprite.cmdt, _zoom_point_value);
        vdp1_cmdt_param_vertex_set(_sprite.cmdt, CMDT_VTX_ZOOM_SPRITE_POINT, &_zoom_point);
        vdp1_cmdt_param_vertex_set(_sprite.cmdt, CMDT_VTX_ZOOM_SPRITE_DISPLAY, &_display);


}

static void _polygon_pointer_config(void)
{
        int16_vec2_t points[4];

        points[0].x = ZOOM_POINT_POINTER_SIZE + _polygon_pointer.position.x - 1;
        points[0].y = -ZOOM_POINT_POINTER_SIZE + _polygon_pointer.position.y;
        points[1].x = ZOOM_POINT_POINTER_SIZE + _polygon_pointer.position.x - 1;
        points[1].y = ZOOM_POINT_POINTER_SIZE + _polygon_pointer.position.y - 1;
        points[2].x = -ZOOM_POINT_POINTER_SIZE + _polygon_pointer.position.x;
        points[2].y = ZOOM_POINT_POINTER_SIZE + _polygon_pointer.position.y - 1;
        points[3].x = -ZOOM_POINT_POINTER_SIZE + _polygon_pointer.position.x;
        points[3].y = -ZOOM_POINT_POINTER_SIZE + _polygon_pointer.position.y;

        vdp1_cmdt_param_color_set(_polygon_pointer.cmdt, _polygon_pointer.color);

        vdp1_cmdt_param_vertex_set(_polygon_pointer.cmdt, CMDT_VTX_POLYGON_A, &points[0]);
        vdp1_cmdt_param_vertex_set(_polygon_pointer.cmdt, CMDT_VTX_POLYGON_B, &points[1]);
        vdp1_cmdt_param_vertex_set(_polygon_pointer.cmdt, CMDT_VTX_POLYGON_C, &points[2]);
        vdp1_cmdt_param_vertex_set(_polygon_pointer.cmdt, CMDT_VTX_POLYGON_D, &points[3]);
}


void _vblank_out_handler(void *work __unused)
{
        smpc_peripheral_intback_issue();
}

void _cpu_frt_ovi_handler(void)
{
}

void user_init(void)
{
        vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_A,
            VDP2_TVMD_VERT_240);

        vdp2_scrn_back_screen_color_set(VDP2_VRAM_ADDR(3, 0x01FFFE),
            COLOR_RGB1555(1, 0, 3, 15));

        vdp2_sprite_priority_set(0, 6);

        /* Setup default VDP1 environment */
        vdp1_env_default_set();

        vdp2_tvmd_display_set();

        //vdp_sync_vblank_out_set(_vblank_out_handler, NULL);

        cpu_frt_init(CPU_FRT_CLOCK_DIV_32);
        cpu_frt_ovi_set(_cpu_frt_ovi_handler);

        //vdp1_vram_partitions_get(&_vdp1_vram_partitions);

        vdp2_sync();
        vdp2_sync_wait();
}
