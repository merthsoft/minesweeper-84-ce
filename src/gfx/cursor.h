#ifndef cursor_include_file
#define cursor_include_file

#ifdef __cplusplus
extern "C" {
#endif

#define cursor_width 20
#define cursor_height 20
#define cursor_size 402
#define cursor ((gfx_sprite_t*)cursor_data)
extern unsigned char cursor_data[402];

#ifdef __cplusplus
}
#endif

#endif
