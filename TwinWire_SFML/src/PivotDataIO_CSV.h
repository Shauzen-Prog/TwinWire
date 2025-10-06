#pragma once
#include <string>
class SpriteAnimator;

struct PivotDataIO_CSV
{
    static bool save(const SpriteAnimator& anim, const std::string& path);
    static bool load(SpriteAnimator& anim, const std::string& path, bool overwriteRectAndDur = false);
};
