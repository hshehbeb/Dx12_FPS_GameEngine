#pragma once
#include <vector>

#include "../Core/UIWidgets/Image2D.h"
#include "../Core/UIWidgets/Image3D.h"

/**
 * represent a group of graphical chinese characters,
 * typically the text of a dialog
 */
struct TextCN
{
    std::vector<std::shared_ptr<ImageBase>> Images;
};
