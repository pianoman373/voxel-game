#pragma once

#include <imgui.h>

struct Console {
    char                  InputBuf[256];
    ImVector<char*>       Items;
    bool                  ScrollToBottom;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> Commands;

    Console();

    ~Console();

    void ClearLog();

    void AddLog(const char* fmt, ...) IM_PRINTFARGS(2);

    void Draw();

    void ExecCommand(const char* command_line);

    static int TextEditCallbackStub(ImGuiTextEditCallbackData* data);

    int TextEditCallback(ImGuiTextEditCallbackData* data);
};
