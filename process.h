#include "main.h"

void get_handle()
{
    global->GameWindow = nullptr;
    while (!(global->GameWindow = FindWindowA("Valve001", 0)))
    {
        Sleep(100);
    }
}

