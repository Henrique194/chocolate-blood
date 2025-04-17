#pragma once

extern char kb_byte;

void Sys_Init(int argc, char** argv);
void Sys_HandleEvents();
void Sys_SetTimer(int divider, void (*handler)());
void Sys_SetKeyboardHandler(void (*handler)());
void Sys_GetMouseDelta(float* dx, float* dy);
int Sys_GetMouseButtons();
void sys_puts(const char* s);
void sys_printf(const char* s, ...);
