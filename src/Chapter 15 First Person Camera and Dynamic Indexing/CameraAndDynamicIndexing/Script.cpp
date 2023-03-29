#include "Script.h"

/**
 * DIALOG: 
 */
void Script::EntryPoint()
{
}

DIALOG_FUNC(L"是吗", DIALOG(L"是", YesHandle), OPTION(L"否", NoHandle));
