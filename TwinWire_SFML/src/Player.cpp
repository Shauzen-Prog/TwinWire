#include "Player.h"
#include <cmath>
#include <optional>

// -------- ANIMACIONES -------------------

static std::vector<FrameMeta> leftHandSustainAttackAnim()
{
    return
    {
        /* LeftHandSustain[1/5] */F(0,32,19,32,9.f),
        /* LeftHandSustain[2/5] */F(19,32,20,32,9.f),
        /* LeftHandSustain[3/5] */F(40,32,23,32,9.f),
        /* LeftHandSustain[4/5] */F(63,32,31,32,9.f),
        /* LeftHandSustain[5/5] */F(94,32,31,32,9.f),
    };
}

static std::vector<FrameMeta> leftHandReleasedAttackAnim()
{
    return
    {
        /* LeftHandReleased[1/5] */F(125,32,31,32,9.f),
        /* LeftHandReleased[2/5] */F(156,32,27,32,9.f),
        /* LeftHandReleased[3/5] */F(183,32,23,32,9.f),
        /* LeftHandReleased[4/5] */F(206,32,20,32,9.f),
        /* LeftHandReleased[5/5] */F(226,32,20,32,9.f),
    };
}

static std::vector<FrameMeta> rightHandSustainAttackAnim()
{
    return {
        /* RightHandSustain[1/5] */F(0,64,18,32,9.f),
        /* RightHandSustain[2/5] */F(18,64,20,32,9.f),
        /* RightHandSustain[3/5] */F(38,64,23,32,9.f),
        /* RightHandSustain[4/5] */F(61,64,27,32,9.f),
        /* RightHandSustain[5/5] */F(88,64,27,32,9.f)
    };
}

static std::vector<FrameMeta> rightHandReleasedAttackAnim()
{
    return {
        /* RightHandReleased[1/5] */F(115,64,25,32,9.f),
        /* RightHandReleased[2/5] */F(140,64,21,32,9.f),
        /* RightHandReleased[3/5] */F(161,64,23,32,9.f),
        /* RightHandReleased[4/5] */F(182,64,18,32,9.f),
        /* RightHandReleased[5/5] */F(200,64,18,32,9.f)
    };
}

static std::vector<FrameMeta> walking()
{
    return {
        /* Walking[1/8] */F(0,96,24,32,9.f),
        /* Walking[2/8] */F(24,96,25,32,9.f),
        /* Walking[3/8] */F(49,96,23,32,9.f),
        /* Walking[4/8] */F(72,96,26,32,9.f),
        /* Walking[5/8] */F(98,96,24,32,9.f),
        /* Walking[6/8] */F(122,96,23,32,9.f),
        /* Walking[7/8] */F(145,96,23,32,9.f),
        /* Walking[8/8] */F(168,96,25,32,9.f)
    };
}

static std::vector<FrameMeta> idle()
{
    return {
        /* Idle[1/8] */F(0,128,19,32,7.f),
        /* Idle[2/8] */F(19,128,19,32,7.f),
        /* Idle[3/8] */F(38,128,20,32,7.f),
        /* Idle[4/8] */F(58,128,20,32,7.f),
        /* Idle[5/8] */F(78,128,22,32,7.f),
        /* Idle[6/8] */F(100,128,22,32,7.f),
        /* Idle[7/8] */F(122,128,21,32,7.f),
        /* Idle[8/8] */F(143,128,19,32,7.f)
    };
}

static std::vector<FrameMeta> die()
{
    return {
        /* Die[1/12] */F(0,0,19,32,9.f),
        /* Die[2/12] */F(19,0,20,32,9.f),
        /* Die[3/12] */F(60,0,23,32,9.f),
        /* Die[4/12] */F(83,0,33,32,9.f),
        /* Die[5/12] */F(116,0,38,32,9.f),
        /* Die[6/12] */F(154,0,38,32,9.f),
        /* Die[7/12] */F(230,0,40,32,9.f),
        /* Die[8/12] */F(270,0,40,32,9.f),
        /* Die[9/12] */F(310,0,42,32,9.f),
        /* Die[10/12] */F(352,0,42,32,9.f),
        /* Die[11/12] */F(394,0,42,32,9.f),
        /* Die[12/12] */F(273,49,42,32,9.f)
    };
}


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

    m_filB.setMaxLength(290.f);
    m_filB.setExtendSpeed(1200.f);
    m_filB.setRetractSpeed(1600.f);
    m_filB.setLockOriginOnExtend(true);

    m_filA.setColor(sf::Color(50, 220, 255));  // A = cian
    m_filB.setColor(sf::Color(255, 230, 80));  // B = amarillo

    // cacheo una vez para no tener copias
    m_frames[AnimId::LHSustain] = leftHandSustainAttackAnim();
    m_frames[AnimId::LHRelease] = leftHandReleasedAttackAnim();
    m_frames[AnimId::RHSustain] = rightHandSustainAttackAnim();
    m_frames[AnimId::RHRelease] = rightHandReleasedAttackAnim();
    m_frames[AnimId::Walk]      = walking();
    m_frames[AnimId::Idle]      = idle();
    m_frames[AnimId::Die]       = die();

    m_anim.setPivotOffset(m_tuning.pivotOffset);
    setVisualScale(m_tuning.visualScale);
    // inicia con uno de default
    play(AnimId::Idle, /*loop=*/true);
}


void Player::play(AnimId id, bool loop, bool holdLast) {
    // Aseguro textura y sale de cualquier pausa previa
    m_anim.setTexture(*m_sheet);
    m_anim.setPaused(false);

    const std::vector<FrameMeta>& seq = m_frames[id];
    
    if (!loop && holdLast)
    {
        // Reproducir una sola vez y quedarse estatico en el ultimo frame
        m_anim.setHoldOnEnd(true);
        m_anim.setFrames(seq, /*loop=*/false);
        m_currentAnim = id;
        return;
    }

    // Caso general (loop infinito o one-shot sin hold)
    m_anim.setHoldOnEnd(false);
    m_anim.setFrames(seq, loop);
    m_currentAnim = id;  
}

void Player::playLoopLastFrame(AnimId id)
{
    m_anim.setTexture(*m_sheet);
    m_anim.setPaused(false);

    m_anim.setHoldOnEnd(false);
    m_anim.loopLastFrame(true);
    m_anim.setFrames(m_frames[id], false);
}

// --- Sockets ---

sf::Vector2f Player::handSocketLocal(Hand h) const {
    return (h == Hand::Right) ? m_tuning.socketRightLocal
                              : m_tuning.socketLeftLocal;
}

void Player::setHandSocketLocal(Hand h, sf::Vector2f p) {
    if (h == Hand::Right) m_tuning.socketRightLocal = p;
    else                  m_tuning.socketLeftLocal  = p;
}

void Player::adjustHandSocket(Hand h, sf::Vector2f d) {
    setHandSocketLocal(h, handSocketLocal(h) + d);
}

sf::Vector2f Player::handSocketWorld(Hand h) const {
    return m_anim.sprite().getTransform().transformPoint(handSocketLocal(h));
}

// ---------------------------------------

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

void Player::lockMovement(float seconds, float slowFactor)
{
    m_moveLockTimer  = std::max(m_moveLockTimer, seconds); // si ya estaba bloqueado por mas tiempo, no acortar
    m_moveSlowFactor = std::clamp(slowFactor, 0.f, 1.f);
}

void Player::playIfChanged(AnimId id, bool loop, bool holdLast)
{
    if (m_currentAnim == id) return; 
    m_anim.setTexture(*m_sheet);
    if (!loop && holdLast) {
        // Reproduce una vez y congela en el ultimo frame
        m_anim.playOnceHoldLast(m_frames[id]);
    } else {
        m_anim.setFrames(m_frames[id], loop);
    }
    m_currentAnim = id; 
}

void Player::updateFacingFromVelocity()
{
    // Flip horizontal con scale.x
    sf::Vector2f s = m_sprite.getScale();
    if (m_velX > 0.f && s.x < 0.f)  m_sprite.setScale({ -s.x, s.y });
    if (m_velX < 0.f && s.x > 0.f)  m_sprite.setScale({ -s.x, s.y });
}


void Player::setTuning(const Tuning& t) {
    m_tuning = t;
    m_anim.setPivotOffset(m_tuning.pivotOffset);
    setVisualScale(m_tuning.visualScale);
}

sf::Vector2f Player::getFeetWorld() const {
    // Por convención: origen del sprite = “pies”
    return m_anim.sprite().getPosition();
}

sf::FloatRect Player::aabb() const {
    // Misma fórmula en todos lados, basada en pies (pivot)
    const sf::Vector2f feet = getFeetWorld();
    const sf::Vector2f origin{
        feet.x - m_tuning.hbSize.x * 0.5f + m_tuning.hbOffset.x,
        feet.y - m_tuning.hbSize.y         + m_tuning.hbOffset.y
    };
    return sf::FloatRect(origin, m_tuning.hbSize);
}

void Player::update(float dt, const sf::RenderWindow& window)
{
    // Tick del timer ANTES de leer input
    if (m_moveLockTimer > 0.f) {
        m_moveLockTimer -= dt;
        if (m_moveLockTimer <= 0.f) {
            m_moveLockTimer  = 0.f;
            m_moveSlowFactor = 1.f; // restaurar velocidad normal
        }
    }

    const bool A_wasExt      = m_filA.isExtending();
    const bool A_wasAttached = m_filA.isAttached();
    const bool B_wasExt      = m_filB.isExtending();
    const bool B_wasAttached = m_filB.isAttached();


    m_anim.update(dt);
    // Lee input (solo X por ahora)
    float dirX = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dirX -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dirX += 1.f;

    // Aplica gate a la velocidad
    const float baseSpeed = m_speed; // velocidad base
    const float speed = isMovementLocked() ? (baseSpeed * m_moveSlowFactor) : baseSpeed;

    // Integracion de movimiento (una sola vez)
    m_velX   = dirX * speed;
    m_vel.x  = m_velX;  
    
    // Flip visual segun direccion
    if (dirX != 0.f) updateFacingFromVelocity();

    // Anim swap Walk/Idle
    if (m_currentAnim != AnimId::Die)
    {
        if (std::abs(m_vel.x) > 1.f || std::abs(m_vel.y) > 1.f)
            playIfChanged(AnimId::Walk, true);
        else
            playIfChanged(AnimId::Idle, true);
    }
    
    
    
    m_pos += m_vel * dt;
    m_sprite.setPosition(m_pos);

    const sf::Vector2u win = window.getSize();
    sf::FloatRect r = aabb(); // basado en pies (sprite position)

    float dx = 0.f;
    if (r.position.x < 0.f) {
        dx = -r.position.x;                         
    } else if (r.position.x + r.size.x > win.x) {
        dx = win.x - (r.position.x + r.size.x);     
    }

    if (std::abs(dx) > 1e-4f) {
        m_pos.x += dx;             
        m_sprite.setPosition(m_pos); 
        
        if ((dx < 0.f && m_vel.x > 0.f) || (dx > 0.f && m_vel.x < 0.f)) {
            m_vel.x = 0.f;
            m_velX  = 0.f;
        }
    }
    
    // --- tecla R: recall de emergencia ---
    const bool recallDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);
    if (!m_prevRecall && recallDown) {
        // soft gate para que no se pise con el walking
        lockMovement(0.5f, 1.f);

        // Release segun que mano este activa
        const bool A_active = m_filA.isExtending() || m_filA.isRetracting() || m_filA.isAttached();
        const bool B_active = m_filB.isExtending() || m_filB.isRetracting() || m_filB.isAttached();

        if (A_active && !B_active)
            play(AnimId::LHRelease, /*loop=*/false, /*holdLast=*/true);
        else if (B_active && !A_active) {
            play(AnimId::RHRelease, /*loop=*/false, /*holdLast=*/true);
        } else {
            // Si ambos o ninguno: elegir uno (opción simple)
            play(AnimId::LHRelease, /*loop=*/false, /*holdLast=*/true);
        }

        m_filA.forceRetract();
        m_filB.forceRetract();
    }
    m_prevRecall = recallDown;
    
    m_filA.setColor(sf::Color::Cyan);
    
    // Origen y actualizacion por frame
    m_filA.updateOrigin(handSocketWorld(Hand::Right)); // A = Right
    m_filB.updateOrigin(handSocketWorld(Hand::Left));  // B = Left
    m_filA.update(dt);
    m_filB.update(dt);

    // --- detectar "miss" si falló y empieza a retraerse ---
    const bool A_missEdge = (A_wasExt && !A_wasAttached) && (m_filA.isRetracting() && !m_filA.isAttached());
    const bool B_missEdge = (B_wasExt && !B_wasAttached) && (m_filB.isRetracting() && !m_filB.isAttached());

    if (A_missEdge) 
    {
        Player_playReleaseForHand(*this, Hand::Right);
        lockMovement(0.f, 0.f);
    }
    if (B_missEdge)
    {
        Player_playReleaseForHand(*this, Hand::Left);
        lockMovement(0.f, 0.f);
    }
    
    if (m_filA.isAttached() && m_filB.isAttached()) {
        IChockeable* aobj = m_filA.attachedObject();
        IChockeable* bobj = m_filB.attachedObject();
        if (aobj && aobj == bobj) {
            aobj->onChoke(m_filA.attachPoint(), m_filB.attachPoint()); // el pilar se “apaga”
            m_filA.forceRetract();
            m_filB.forceRetract();
            Player_playReleaseForHand(*this, Hand::Left);
            Player_playReleaseForHand(*this, Hand::Right);
        }
    }
}

void Player::draw(sf::RenderTarget& target) const
{
    // el filamento va por detras por que se dibuja primero
    m_filA.draw(target);
    m_filB.draw(target);
    target.draw(m_sprite);
}

void Player::handleEvent(const sf::Event& ev, const sf::RenderWindow& window)
{
    // Clicks por evento (edge limpio)
    if (const auto* mb = ev.getIf<sf::Event::MouseButtonPressed>())
    {
        // Target en mundo
        const sf::Vector2i mpPix   = sf::Mouse::getPosition(window);
        const sf::Vector2f mpWorld = window.mapPixelToCoords(mpPix);

        // Al presionar, oriento hacia el lado del click antes de disparar
        const float handX = handSocketWorld(Hand::Right).x;
        m_facingRight = (mpWorld.x >= handX);
        applyVisualTransform();

        // Estado actual de los filamentos
        const bool A_free     = m_filA.canFire();
        const bool B_free     = m_filB.canFire();
        const bool A_attached = m_filA.isAttached();

        switch (mb->button) {
        case sf::Mouse::Button::Left: {
                // LMB -> Filament A (Right hand)
                if (A_free && B_free) {
                    lockMovement(0.12f, 0.0f);
                    playLoopLastFrame(AnimId::LHSustain); // anim de mano izquierda 
                    m_filA.fireStraight(handSocketWorld(Hand::Right), mpWorld, /*canAttach=*/true);
                    if (m_playSfx)
                        m_playSfx("../../../../res/Assets/Audio/SFX/FilamentA.wav", 1.f, 1.f);
                }
                break;
        }
        case sf::Mouse::Button::Right: {
                // RMB -> Filament B (Left hand)
                if (B_free && (A_free || A_attached)) {
                    lockMovement(0.12f, 0.0f);
                    playLoopLastFrame(AnimId::RHSustain); // anim de mano derecha 
                    const bool canAttachB = A_attached; // si A ya está agarrado, B puede enganchar
                    m_filB.fireStraight(handSocketWorld(Hand::Left), mpWorld, canAttachB);
                    if (m_playSfx)
                        m_playSfx("../../../../res/Assets/Audio/SFX/FilamentB.wav", 1.f, 1.f);
                }
                break;
        }
        default: break;
        }
    }

    // Recall con R (por evento en vez de polling)
    if (const auto* kp = ev.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::R) {
            lockMovement(0.10f, 0.2f);

            const bool A_active = m_filA.isExtending() || m_filA.isRetracting() || m_filA.isAttached();
            const bool B_active = m_filB.isExtending() || m_filB.isRetracting() || m_filB.isAttached();

            if      (A_active && !B_active) play(AnimId::LHRelease, /*loop=*/false, /*holdLast=*/true);
            else if (B_active && !A_active) play(AnimId::RHRelease, /*loop=*/false, /*holdLast=*/true);
            else                             play(AnimId::LHRelease, /*loop=*/false, /*holdLast=*/true);

            m_filA.forceRetract();
            m_filB.forceRetract();
        }
    }
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


sf::Vector2f Player::computeShootDirToMouse(const sf::RenderWindow& window, Hand h) const
{
    const sf::Vector2f origin = handSocketWorld(h);
    const sf::Vector2i mpPix  = sf::Mouse::getPosition(window);
    const sf::Vector2f target = window.mapPixelToCoords(mpPix); 

    sf::Vector2f d = target - origin;
    const float len2 = d.x*d.x + d.y*d.y;
    if (len2 <= 1e-6f) return { 1.f, 0.f };
    const float inv = 1.f / std::sqrt(len2);
    return d * inv;
}

void Player::Player_playReleaseForHand(Player& self, Hand h)
{
    if (h == Hand::Right) self.play(AnimId::RHRelease, /*loop=*/false, /*holdLast=*/true);
    else                  self.play(AnimId::LHRelease, /*loop=*/false, /*holdLast=*/true);
}

void Player::setVisualScale(float s) {
    m_tuning.visualScale = s;
    m_visualScale = s;
    applyVisualTransform();
    
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

void Player::setHitboxSize(sf::Vector2f s) {
    m_tuning.hbSize.x = std::max(2.f, s.x);
    m_tuning.hbSize.y = std::max(2.f, s.y);
}
void Player::setHitboxOffset(sf::Vector2f o) {
    m_tuning.hbOffset = o;
}
void Player::adjustHitboxSize(sf::Vector2f d) {
    setHitboxSize(m_tuning.hbSize + d);
}
void Player::adjustHitboxOffset(sf::Vector2f d) {
    setHitboxOffset(m_tuning.hbOffset + d);
}

void Player::applyVisualTransform()
{
    // Escala X segun signo
    const float sx = m_facingRight ?  m_visualScale : -m_visualScale;
    m_sprite.setScale({ sx, m_visualScale });
}

