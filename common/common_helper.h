#pragma once

#include <string>
#include <iostream>

void PrintSubtitle(std::string_view title)
{
    constexpr std::size_t width = 20;
    const std::size_t contentWidth = std::min(title.size(), width);
    const std::size_t leftPadding = (width - contentWidth) / 2;
    const std::size_t rightPadding = width - contentWidth - leftPadding;
    std::cout << "======"
              << std::string(leftPadding, ' ')
              << title.substr(0, contentWidth)
              << std::string(rightPadding, ' ')
              << "======\n";
}

#define SUBTITLE(str) PrintSubtitle(str)