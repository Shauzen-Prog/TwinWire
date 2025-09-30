#include "ResouceManager.h"
#include <stdexcept>
#include <sstream>

// --------------- Helpers

std::string ResouceManager::makeTextureKey(const std::string& path,
                                        const std::optional<sf::IntRect>& area)
{
    std::ostringstream oss;
    oss << path;
    
    if (area.has_value())
    {
        const auto& r = *area;
        oss << "|rect:" 
            << r.position.x << "," << r.position.y << ","
            << r.size.x << "," << r.size.y;
    }
    
    return oss.str();
}

// Api publica

ResouceManager::TexturePtr ResouceManager::getTexture(const std::string& path)
{
    //Ruta simple sin mipmaps ni subrect
    return getTexture(path, std::nullopt);
}

ResouceManager::TexturePtr ResouceManager::getTexture(const std::string& path, const std::optional<sf::IntRect>& area)
{
    const std::string key = makeTextureKey(path, area);

    //cache hit
    if (auto it = m_textures.find(key); it != m_textures.end())
        return it->second;

    //cache miss -> cargar
    auto tex = std::make_shared<sf::Texture>();

    if (area.has_value())
    {
        if (!tex->loadFromFile(path, false ,*area))
            throw std::runtime_error("Error loading texture " + path);
    }
    else
    {
        if (!tex->loadFromFile(path, false))
            throw std::runtime_error("Error loading texture " + path);
    }

    m_textures.emplace(key, tex);
    return tex;
}

ResouceManager::FontPtr ResouceManager::getFont(const std::string& path)
{
    if (auto it = m_fonts.find(path); it != m_fonts.end())
        return it->second;

    auto font = std::make_shared<sf::Font>();
    if (!font->openFromFile(path))
        throw std::runtime_error("Error loading font " + path);
    
    m_fonts.emplace(path, font);
    return font;
}

ResouceManager::SoundPtr ResouceManager::getSound(const std::string& path)
{
    if (auto it = m_sounds.find(path); it != m_sounds.end())
        return it->second;

    auto snd = std::make_shared<sf::SoundBuffer>();
    if (!snd->loadFromFile(path))
        throw std::runtime_error("Error loading sound " + path);
    
    return snd;
}

void ResouceManager::clear()
{
    m_textures.clear();
    m_fonts.clear();
    m_sounds.clear();
}
