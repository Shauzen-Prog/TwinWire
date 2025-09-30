#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <unordered_map>
#include <string>
#include <memory>
#include <optional>

class ResouceManager
{
public:
    using TexturePtr = std::shared_ptr<sf::Texture>;
    using FontPtr = std::shared_ptr<sf::Font>;
    using SoundPtr = std::shared_ptr<sf::SoundBuffer>;

    // --- Textures
    TexturePtr getTexture(const std::string& path);
    TexturePtr getTexture(const std::string& path,
                          const std::optional<sf::IntRect>& area);

    // --- Fonts
    FontPtr getFont(const std::string& path);

    // --- Sounds
    SoundPtr getSound(const std::string& path);

    // Limpiar todo el cache (libera memoria de recursos no refenciado fuera)
    void clear();

private:
    template<typename T>
    using Cache = std::unordered_map<std::string, std::shared_ptr<T>>;

    Cache<sf::Texture> m_textures;
    Cache<sf::Font> m_fonts;
    Cache<sf::SoundBuffer> m_sounds;

    // Helpers
    static std::string makeTextureKey(const std::string& path,
                                     const std::optional<sf::IntRect>& area);
};
