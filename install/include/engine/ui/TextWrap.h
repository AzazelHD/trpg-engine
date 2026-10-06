#pragma once

#include <string>
#include <vector>

class Font;
class Renderer;

class TextWrap
{
public:
    static std::vector<std::string> wrap(Renderer *renderer,
                                         const Font *font,
                                         const std::string &text,
                                         float maxWidth);
};
