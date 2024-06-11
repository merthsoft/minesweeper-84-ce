#ifndef flagged_include_file
#define flagged_include_file

#ifdef __cplusplus
extern "C" {
#endif

#define flagged_width 20
#define flagged_height 20
#define flagged_size 402
#define flagged ((gfx_sprite_t*)flagged_data)
extern unsigned char flagged_data[402];

#ifdef __cplusplus
}
#endif

#endif
