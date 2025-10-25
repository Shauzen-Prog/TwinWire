#include "SoundManager.h"
#include <algorithm>

SoundManager::SoundManager(ResouceManager& rm, int sfxPoolSize)
: m_rm(rm)
{
    m_pool.resize(std::max<int>(8, sfxPoolSize));
    
    // Musica por defecto sin loop ni path cargado
    m_musicStream.setLooping(true);
    applyVolumeToMusic();
}

// ---------------- Volumenes ----------------
void SoundManager::setMasterVolume(float v) {
    m_master = std::max(0.f, std::min(1.f, v));
    applyVolumesToActiveSfx();
    applyVolumeToMusic();
}
void SoundManager::setMusicVolume(float v)  {
    m_music = std::max(0.f, std::min(1.f, v));
    applyVolumeToMusic();
}
void SoundManager::setSfxVolume(float v)    {
    m_sfx = std::max(0.f, std::min(1.f, v));
    applyVolumesToActiveSfx();
}

void SoundManager::applyVolumesToActiveSfx()
{
    for (auto& v : m_pool) {
        if (v.inUse && v.sound) {
            v.sound->setVolume(currentSfxSfmlVolume(/*local*/1.f));
        }
    }
}
void SoundManager::applyVolumeToMusic()
{
    m_musicStream.setVolume(currentMusicSfmlVolume());
}

// ---------------- SFX ----------------
void SoundManager::playSfx(const std::string& path, float pitch, float vol)
{
    auto it = std::find_if(m_pool.begin(), m_pool.end(),
        [](const SfxVoice& v){
            return !v.inUse || !v.sound || v.sound->getStatus() == sf::Sound::Status::Stopped;
        });

    if (it == m_pool.end()) {
        // Si todas usadas, pisa la primera que este Stopped
        it = m_pool.begin();
    }

    // Carga buffer desde ResourceManager (cache)
    ResouceManager::SoundPtr buf = m_rm.getSound(path);
    if (!buf) return;
    
    if (!it->sound) {
        it->sound.emplace(*buf); 
    } else {
        it->sound->stop();
        it->sound->setBuffer(*buf);
    }

    it->sound->setPitch(std::max(0.01f, pitch));
    it->sound->setVolume(currentSfxSfmlVolume(std::max(0.f, std::min(1.f, vol))));
    it->sound->play();
    it->inUse = true;
}

// ---------------- Música ----------------
bool SoundManager::playMusic(const std::string& path, bool loop)
{
    if (!m_musicStream.openFromFile(path)) {
        m_hasMusic = false;
        return false;
    }
    m_musicStream.setLooping(loop);
    applyVolumeToMusic();
    m_musicStream.play();
    m_hasMusic = true;
    return true;
}
void SoundManager::stopMusic()
{
    if (!m_hasMusic) return;
    m_musicStream.stop();
    m_hasMusic = false;
}
void SoundManager::pauseMusic(bool pause)
{
    if (!m_hasMusic) return;
    if (pause) m_musicStream.pause();
    else       m_musicStream.play();
}

// ---------------- Tick ----------------
void SoundManager::update()
{
    for (auto& v : m_pool) {
        if (v.inUse) {
            if (!v.sound || v.sound->getStatus() == sf::Sound::Status::Stopped) {
                v.inUse = false;
                v.sound.reset();
            }
        }
    }
}

// ---------------- Persistencia ----------------
void SoundManager::saveVolumes()
{
    if (m_save) m_save(m_master, m_music, m_sfx);
}
void SoundManager::loadVolumes()
{
    if (!m_load) return;
    float M, Mu, S;
    if (m_load(M, Mu, S)) {
        m_master = std::max(0.f, std::min(1.f, M));
        m_music  = std::max(0.f, std::min(1.f, Mu));
        m_sfx    = std::max(0.f, std::min(1.f, S));
        applyVolumesToActiveSfx();
        applyVolumeToMusic();
    }
}


