#include "engine/ui/TextWrap.h"

#include "engine/renderer/Font.h"
#include "engine/renderer/Renderer.h"

#include <sstream>

std::vector<std::string> TextWrap::wrap(Renderer *renderer,
                                        const Font *font,
                                        const std::string &text,
                                        float maxWidth)
{
    std::vector<std::string> lines;
    if (!renderer || !font || text.empty())
        return lines;

    // Split on manual line breaks first, then word-wrap each paragraph
    // independently — so an explicit \n always starts a new line, and
    // long paragraphs still wrap automatically within maxWidth.
    std::istringstream paragraphStream(text);
    std::string paragraph;

    while (std::getline(paragraphStream, paragraph, '\n'))
    {
        std::istringstream wordStream(paragraph);
        std::string word;
        std::string currentLine;

        while (wordStream >> word)
        {
            const std::string candidate = currentLine.empty() ? word : currentLine + " " + word;
            const float candidateWidth = renderer->measureText(font, candidate).x;
            if (candidateWidth <= maxWidth || currentLine.empty())
                currentLine = candidate;
            else
            {
                lines.push_back(currentLine);
                currentLine = word;
            }
        }

        // Push whatever's left, even if the paragraph was empty (keeps a
        // blank line for consecutive \n\n, instead of silently dropping it).
        lines.push_back(currentLine);
    }

    return lines;
}
