#pragma once

namespace
{
    #define DIALOG_FUNC(funcName, dialog)
}

/**
 * NOTE: should rely on Linker in order to be referenced
 */
class Script
{
public:
    static void EntryPoint();

    DIALOG_FUNC(SampleDialog, L"是吗", DIALOG(L"是", YesHandle));
};
