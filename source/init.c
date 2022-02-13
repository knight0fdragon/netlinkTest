#include "init.h"




vdp1_cmdt_list_t *_cmdt_list;
void *_romdisk;

static void _sprite_init(vdp1_vram_partitions_t _vdp1_vram_partitions)
{
        _sprite.anim_frame = 0;
        _sprite.anim_counter = 0;

        _sprite.tex_base = _vdp1_vram_partitions.texture_base;
        _sprite.pal_base = (void *)VDP2_CRAM_MODE_1_OFFSET(1, 0, 0x0000);

        const vdp1_cmdt_draw_mode_t draw_mode = {
                .bits.trans_pixel_disable = true,
                .bits.pre_clipping_disable = true,
                .bits.end_code_disable = true
        };

        const vdp1_cmdt_color_bank_t color_bank = {
                .type_0.data.dc = 0x0100
        };

        vdp1_cmdt_scaled_sprite_set(_sprite.cmdt);
        vdp1_cmdt_param_draw_mode_set(_sprite.cmdt, draw_mode);
        vdp1_cmdt_param_color_mode4_set(_sprite.cmdt, color_bank);

        vdp1_cmdt_param_size_set(_sprite.cmdt, ZOOM_POINT_WIDTH, ZOOM_POINT_HEIGHT);

        void *fh[2];
        void *p;
        size_t len;

        fh[0] = romdisk_open(_romdisk, ZOOM_TEX_PATH);
        assert(fh[0] != NULL);
        p = romdisk_direct(fh[0]);
        len = romdisk_total(fh[0]);
        scu_dma_transfer(0, _sprite.tex_base, p, len);

        fh[1] = romdisk_open(_romdisk, ZOOM_PAL_PATH);
        assert(fh[1] != NULL);
        p = romdisk_direct(fh[1]);
        len = romdisk_total(fh[1]);
        scu_dma_transfer(0, _sprite.pal_base, p, len);

        romdisk_close(fh[0]);
        romdisk_close(fh[1]);
}


static void _polygon_pointer_init(void)
{
        static const vdp1_cmdt_draw_mode_t draw_mode = {
                .raw = 0x0000,
                .bits.pre_clipping_disable = true
        };

        vdp1_cmdt_polygon_set(_polygon_pointer.cmdt);
        vdp1_cmdt_param_draw_mode_set(_polygon_pointer.cmdt, draw_mode);
}

static void _cmdt_list_init(vdp1_vram_partitions_t _vdp1_vram_partitions)
{
        static const int16_vec2_t system_clip_coord =
            INT16_VEC2_INITIALIZER(SCREEN_WIDTH - 1,
                                   SCREEN_HEIGHT - 1);

        static const int16_vec2_t local_coord_ul =
            INT16_VEC2_INITIALIZER(0, 0);

        static const int16_vec2_t local_coord_center =
            INT16_VEC2_INITIALIZER(SCREEN_WIDTH / 2,
                                   SCREEN_HEIGHT / 2);

        static const vdp1_cmdt_draw_mode_t polygon_draw_mode = {
                .raw                       = 0x0000,
                .bits.pre_clipping_disable = true
        };

        static const int16_vec2_t polygon_points[] = {
                INT16_VEC2_INITIALIZER(0, SCREEN_HEIGHT - 1),
                INT16_VEC2_INITIALIZER(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1),
                INT16_VEC2_INITIALIZER(SCREEN_WIDTH - 1,                 0),
                INT16_VEC2_INITIALIZER(               0,                 0)
        };

        _cmdt_list = vdp1_cmdt_list_alloc(VDP1_CMDT_ORDER_COUNT);

        (void)memset(&_cmdt_list->cmdts[0], 0x00, sizeof(vdp1_cmdt_t) * VDP1_CMDT_ORDER_COUNT);

        _cmdt_list->count = VDP1_CMDT_ORDER_COUNT;

        vdp1_cmdt_t * const cmdts =
            &_cmdt_list->cmdts[0];

        _sprite.cmdt = &cmdts[VDP1_CMDT_ORDER_SPRITE_INDEX];
        _polygon_pointer.cmdt = &cmdts[VDP1_CMDT_ORDER_POLYGON_POINTER_INDEX];

        _polygon_pointer_init();
        _sprite_init(_vdp1_vram_partitions);

        vdp1_cmdt_system_clip_coord_set(&cmdts[VDP1_CMDT_ORDER_SYSTEM_CLIP_COORDS_INDEX]);
        vdp1_cmdt_param_vertex_set(&cmdts[VDP1_CMDT_ORDER_SYSTEM_CLIP_COORDS_INDEX],
            CMDT_VTX_SYSTEM_CLIP,
            &system_clip_coord);

        vdp1_cmdt_local_coord_set(&cmdts[VDP1_CMDT_ORDER_CLEAR_LOCAL_COORDS_INDEX]);
        vdp1_cmdt_param_vertex_set(&cmdts[VDP1_CMDT_ORDER_CLEAR_LOCAL_COORDS_INDEX],
            CMDT_VTX_LOCAL_COORD,
            &local_coord_ul);

        vdp1_cmdt_polygon_set(&cmdts[VDP1_CMDT_ORDER_CLEAR_POLYGON_INDEX]);
        vdp1_cmdt_param_draw_mode_set(&cmdts[VDP1_CMDT_ORDER_CLEAR_POLYGON_INDEX],
            polygon_draw_mode);
        vdp1_cmdt_param_vertices_set(&cmdts[VDP1_CMDT_ORDER_CLEAR_POLYGON_INDEX],
            polygon_points);
        vdp1_cmdt_jump_skip_next(&cmdts[VDP1_CMDT_ORDER_CLEAR_POLYGON_INDEX]);

        vdp1_cmdt_local_coord_set(&cmdts[VDP1_CMDT_ORDER_LOCAL_COORDS_INDEX]);
        vdp1_cmdt_param_vertex_set(&cmdts[VDP1_CMDT_ORDER_LOCAL_COORDS_INDEX],
            CMDT_VTX_LOCAL_COORD,
            &local_coord_center);

        vdp1_cmdt_end_set(&cmdts[VDP1_CMDT_ORDER_DRAW_END_INDEX]);
}



void _init(vdp1_vram_partitions_t _vdp1_vram_partitions)
{
        dbgio_dev_default_init(DBGIO_DEV_VDP2_ASYNC);
        dbgio_dev_font_load();

        romdisk_init();

        _romdisk = romdisk_mount(root_romdisk);
        assert(_romdisk != NULL);

       // _cmdt_list_init(_vdp1_vram_partitions);
}
