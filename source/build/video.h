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
void Video_Text_SetCursor(int x, int y);
void Video_Text_Scroll(int lines, int attr, int x1, int y1, int x2, int y2);

#ifdef __cplusplus
}
#endif
