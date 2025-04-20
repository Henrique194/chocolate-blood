#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern char video_palette[768];
extern char* video_buffer;
extern int video_pages;
extern int video_row_stride;
extern int video_page_stride;
extern int video_xdim, video_ydim;
extern int video_graphics;
extern char* video_text_buffer;

void Video_Init();
void Video_Set(int graphics, int w, int h);
void Video_BlitPage(int32_t page);
void Video_Blit();
void Video_Text_Puts(const char* s);

#ifdef __cplusplus
}
#endif
