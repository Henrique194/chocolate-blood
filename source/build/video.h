#pragma once

extern char video_palette[768];
extern char* video_buffer;
extern int video_pages;
extern int video_row_stride;
extern int video_page_stride;
extern int video_xdim, video_ydim;
extern int video_graphics;

void Video_Set(int graphics, int w, int h);
void Video_BlitPage(int32_t page);


