#include "Player.h"
#include <cmath>

Player::Player(ResouceManager& rm, const std::string& sheetPath)
: m_rm(rm)
, m_sheet(m_rm.getTexture(sheetPath)) // obtengo la textura
, m_sprite(*m_sheet) // contruyo el sprite con la textura
, m_anim(m_sprite) // y el animator con el sprite
{
    m_sheet->setSmooth(true);
    m_sprite.setScale({ m_visualScale, m_visualScale });
    m_sprite.setPosition(m_pos);

    m_filA.setMaxLength(280.f);
    m_filA.setExtendSpeed(1200.f);
    m_filA.setRetractSpeed(1600.f);
    m_filA.setLockOriginOnExtend(true);

    m_filB.setMaxLength(280.f);
    m_filB.setExtendSpeed(1200.f);
    m_filB.setRetractSpeed(1600.f);
    m_filB.setLockOriginOnExtend(true);

    m_filA.setColor(sf::Color(50, 220, 255));  // A = cian
    m_filB.setColor(sf::Color(255, 230, 80));  // B = amarillo
}

void Player::setFrames(const std::vector<FrameMeta>& frames, bool loop)
{
    // El animator aplica rect + pivotX por frame y deja Y en base del rect
    m_anim.setTexture(*m_sheet);
    m_anim.setFrames(frames, loop);
    // Nota: el origen del sprite lo fija el animator en applyCurrentFrame()
}

void Player::handleInput()
{
    float x = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) x += 1.f;
    m_vel.x = x * m_speed;

    // Facing por movimiento
    if (m_vel.x > 1e-3f) m_facingRight = true;
    else if (m_vel.x < -1e-3f) m_facingRight = false;

    applyVisualTransform();
}

void Player::update(float dt, const sf::RenderWindow& window)
{
    // Mover
    m_pos += m_vel * dt;
    m_sprite.setPosition(m_pos);

    // --- tecla R: recall de emergencia ---
    const bool recallDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);
    if (!m_prevRecall && recallDown) {
        m_filA.forceRetract();
        m_filB.forceRetract();
    }
    m_prevRecall = recallDown;
    
    // --- Mouse (world)
    const sf::Vector2i mpPix   = sf::Mouse::getPosition(window);
    const sf::Vector2f mpWorld = window.mapPixelToCoords(mpPix);

    // --- Botones (SFML 3)
    const bool left  = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    const bool right = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
    
    // Bordes
    const bool edgeL = (!m_prevLeft  && left);
    const bool edgeR = (!m_prevRight && right);

    // Estado de cada hilo
    const bool A_free = m_filA.canFire();      // no animando, sin cooldown, no attached
    const bool B_free = m_filB.canFire();
    const bool A_attached = m_filA.isAttached();

    // Si hubo click (izq o der), orientarse hacia ese lado
    if (edgeL || edgeR) {
        const float handX = getHandSocketWorld().x;
        m_facingRight = (mpWorld.x >= handX);
        applyVisualTransform(); // actualizp el flip antes de disparar
    }

    // Reglas:
    // - Exclusividad normal: solo dispara si ambos estan libres
    // - EXCEPCION: si A esta Attached, permite disparar B (aunque A no este "free")
    // - Si se presionan ambos a la vez:
    //     * Si A esta Attached ⇒ disparamos B
    //     * Si no ⇒ prioridad izquierda
    if (edgeL && edgeR) {
        if (A_attached && B_free) {
            m_filB.fireStraight(getHandSocketWorld(), mpWorld, /*canAttach=*/true);
        } else if (A_free && B_free) {
            m_filA.fireStraight(getHandSocketWorld(), mpWorld, /*canAttach=*/true);
        }
    } else if (edgeL) {
        if (A_free && B_free) {
            m_filA.fireStraight(getHandSocketWorld(), mpWorld, /*canAttach=*/true);
        }
    } else if (edgeR) {
        if (B_free && (A_free || A_attached)) {
            const bool canAttachB = A_attached; // mantiene la regla: B solo se pega si A ya esta
            m_filB.fireStraight(getHandSocketWorld(), mpWorld, /*canAttach=*/canAttachB);
        }
    }

    m_prevLeft  = left;
    m_prevRight = right;

    m_filA.setColor(sf::Color::Cyan);
    
    // Origen y actualizacion por frame
    m_filA.updateOrigin(getHandSocketWorld());
    m_filB.updateOrigin(getHandSocketWorld());
    m_filA.update(dt);
    m_filB.update(dt);
    
    if (m_filA.isAttached() && m_filB.isAttached()) {
        IChockeable* aobj = m_filA.attachedObject();
        IChockeable* bobj = m_filB.attachedObject();
        if (aobj && aobj == bobj) {
            aobj->onChoke(m_filA.attachPoint(), m_filB.attachPoint()); // el pilar se “apaga”
            m_filA.forceRetract();
            m_filB.forceRetract();
        }
    }
}

void Player::draw(sf::RenderTarget& target) const
{
    // el filamento va por detras por que se dibuja primero
    // Dibujá primero para que queden atras del player
    m_filA.draw(target);
    m_filB.draw(target);
    target.draw(m_sprite);
}

void Player::setPosition(sf::Vector2f p)
{
    m_pos = p;
    m_sprite.setPosition(m_pos);
}

sf::Vector2f Player::getPosition() const
{
    return m_pos;
}

void Player::setHandSocketLocal(sf::Vector2f local)
{
    m_handSocketLocal = local;
}

sf::Vector2f Player::getHandSocketWorld() const
{
    // Usa el transform completo del sprite (posición, escala/flip, origen por frame)
    return m_sprite.getTransform().transformPoint(m_handSocketLocal);
}

sf::Vector2f Player::computeShootDirToMouse(const sf::RenderWindow& window) const
{
    const sf::Vector2f origin = getHandSocketWorld();
    const sf::Vector2i mpPix  = sf::Mouse::getPosition(window);
    const sf::Vector2f target = window.mapPixelToCoords(mpPix); 

    sf::Vector2f d = target - origin;
    const float len2 = d.x*d.x + d.y*d.y;
    if (len2 <= 1e-6f) return { 1.f, 0.f };
    const float inv = 1.f / std::sqrt(len2);
    return d * inv;
}

void Player::setVisualScale(float s) {
    m_visualScale = s;
    applyVisualTransform();
    // aplica inmediatamente respetando el flip
    const float sx = m_facingRight ? 1.f : -1.f;
    m_sprite.setScale({ sx * m_visualScale, m_visualScale });
}

void Player::setChokeQuery(IChokeQuery* q)
{
    m_chokeQuery = q;

    auto ray = [q](const sf::Vector2f& a,
                   const sf::Vector2f& b,
                   sf::Vector2f& outHit) -> IChockeable*
    {
        return q ? q->RaycastChoke(a, b, outHit) : nullptr;
    };

    if (m_chokeQuery)
        m_filA.setRaycast(ray);
    else
        m_filA.setRaycast(nullptr);
    
    if (m_chokeQuery)
        m_filB.setRaycast(ray);
    else
        m_filB.setRaycast(nullptr);
}

void Player::applyVisualTransform()
{
    // Escala X segun signo
    const float sx = m_facingRight ?  m_visualScale : -m_visualScale;
    m_sprite.setScale({ sx, m_visualScale });
}

