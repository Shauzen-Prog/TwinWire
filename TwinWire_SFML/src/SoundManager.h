#pragma once
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <optional>
#include <functional>
#include "ResouceManager.h"

class SoundManager
{
public:
    explicit SoundManager(ResouceManager& rm, int sfxPoolSize = 32);

    // Volumenes
    void setMasterVolume(float v);
    void setMusicVolume(float v);
    void setSfxVolume(float v);

    float masterVolume() const { return m_master; }
    float musicVolume()  const { return m_music;  }
    float sfxVolume()    const { return m_sfx;    }

    void playSfx(const std::string& path, float pitch = 1.f, float vol = 1.f);

    // Musica
    bool playMusic(const std::string& path, bool loop = true);
    void stopMusic();
    void pauseMusic(bool pause);

    void update();

    // Persistencia: set callbacks para guardar/cargar volumenes
    void setSaveVolumesCallback(std::function<void(float,float,float)> saver) { m_save = std::move(saver); }
    void setLoadVolumesCallback(std::function<bool(float&,float&,float&)> loader) { m_load = std::move(loader); }
    void saveVolumes();
    void loadVolumes();

private:
    ResouceManager& m_rm;

    // Mixer
    float m_master {1.f};
    float m_music  {1.f};
    float m_sfx    {1.f};

    // Musica 
    sf::Music m_musicStream;
    bool m_hasMusic {false};

    // Pool de sonidos cortos
    struct SfxVoice {
        std::optional<sf::Sound> sound;
        bool inUse {false};
    };
    std::vector<SfxVoice> m_pool;

    // Helpers
    static float toSfmlVolume(float v01) { 
        // Lineal simple 0..1 -> 0..100 
        float clamped = (v01 < 0.f) ? 0.f : (v01 > 1.f ? 1.f : v01);
        return clamped * 100.f; 
    }
    float currentMusicSfmlVolume() const { return toSfmlVolume(m_master * m_music); }
    float currentSfxSfmlVolume(float local) const { return toSfmlVolume(m_master * m_sfx * local); }

    void applyVolumesToActiveSfx();
    void applyVolumeToMusic();

    // Persistencia
    std::function<void(float,float,float)> m_save;
    std::function<bool(float&,float&,float&)> m_load;
};
