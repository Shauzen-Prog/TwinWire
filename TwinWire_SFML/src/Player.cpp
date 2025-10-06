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
        /* Idle[1/8] */F(0,128,19,32,9.f),
        /* Idle[2/8] */F(19,128,19,32,9.f),
        /* Idle[3/8] */F(38,128,20,32,9.f),
        /* Idle[4/8] */F(58,128,20,32,9.f),
        /* Idle[5/8] */F(78,128,22,32,9.f),
        /* Idle[6/8] */F(100,128,22,32,9.f),
        /* Idle[7/8] */F(122,128,21,32,9.f),
        /* Idle[8/8] */F(143,128,19,32,9.f)
    };
}

static std::vector<FrameMeta> makeDie()
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


//namespace { // datos “privados” del módulo
//    std::vector<FrameMeta> leftHandSustainAttackAnim();
//    std::vector<FrameMeta> leftHandReleasedAttackAnim();
//    std::vector<FrameMeta> rightHandSustainAttackAnim();
//    std::vector<FrameMeta> rightHandReleasedAttackAnim();
//    std::vector<FrameMeta> makeDie();
//}

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

    // cacheo una vez para no tener copias
    //m_frames[AnimId::LHSustain] = leftHandSustainAttackAnim();
    //m_frames[AnimId::LHRelease] = leftHandReleasedAttackAnim();
    //m_frames[AnimId::RHSustain] = rightHandSustainAttackAnim();
    //m_frames[AnimId::RHRelease] = rightHandReleasedAttackAnim();
    m_frames[AnimId::Walk]      = walking();
    m_frames[AnimId::Idle]      = idle();
    //m_frames[AnimId::Die]       = makeDie();

    // inicia con uno de default
    play(AnimId::Idle, /*loop=*/true);
}

// -------- ANIMACIONES -------------------

//static std::vector<FrameMeta> leftHandSustainAttackAnim()
//{
//    return
//    {
//        /* LeftHandSustain[1/5] */F(0,32,19,32,9.f),
//        /* LeftHandSustain[2/5] */F(19,32,20,32,9.f),
//        /* LeftHandSustain[3/5] */F(40,32,23,32,9.f),
//        /* LeftHandSustain[4/5] */F(63,32,31,32,9.f),
//        /* LeftHandSustain[5/5] */F(94,32,31,32,9.f),
//    };
//}
//
//static std::vector<FrameMeta> leftHandReleasedAttackAnim()
//{
//    return
//    {
//        /* LeftHandReleased[1/5] */F(125,32,31,32,9.f),
//        /* LeftHandReleased[2/5] */F(156,32,27,32,9.f),
//        /* LeftHandReleased[3/5] */F(183,32,23,32,9.f),
//        /* LeftHandReleased[4/5] */F(206,32,20,32,9.f),
//        /* LeftHandReleased[5/5] */F(226,32,20,32,9.f),
//    };
//}
//
//static std::vector<FrameMeta> rightHandSustainAttackAnim()
//{
//    return {
//        /* RightHandSustain[1/5] */F(0,64,18,32,9.f),
//        /* RightHandSustain[2/5] */F(18,64,20,32,9.f),
//        /* RightHandSustain[3/5] */F(38,64,23,32,9.f),
//        /* RightHandSustain[4/5] */F(61,64,27,32,9.f),
//        /* RightHandSustain[5/5] */F(88,64,27,32,9.f)
//    };
//}
//
//static std::vector<FrameMeta> rightHandReleasedAttackAnim()
//{
//    return {
//        /* RightHandReleased[1/5] */F(115,64,25,32,9.f),
//        /* RightHandReleased[2/5] */F(140,64,21,32,9.f),
//        /* RightHandReleased[3/5] */F(161,64,23,32,9.f),
//        /* RightHandReleased[4/5] */F(182,64,18,32,9.f),
//        /* RightHandReleased[5/5] */F(200,64,18,32,9.f)
//    };
//}
//
//static std::vector<FrameMeta> walking()
//{
//    return {
//        /* Walking[1/8] */F(0,96,24,32,9.f),
//        /* Walking[2/8] */F(24,96,25,32,9.f),
//        /* Walking[3/8] */F(49,96,23,32,9.f),
//        /* Walking[4/8] */F(72,96,26,32,9.f),
//        /* Walking[5/8] */F(98,96,24,32,9.f),
//        /* Walking[6/8] */F(122,96,23,32,9.f),
//        /* Walking[7/8] */F(145,96,23,32,9.f),
//        /* Walking[8/8] */F(168,96,25,32,9.f)
//    };
//}
//
//static std::vector<FrameMeta> idle()
//{
//    return {
//        /* Idle[1/8] */F(0,128,19,32,9.f),
//        /* Idle[2/8] */F(19,128,19,32,9.f),
//        /* Idle[3/8] */F(38,128,20,32,9.f),
//        /* Idle[4/8] */F(58,128,20,32,9.f),
//        /* Idle[5/8] */F(78,128,22,32,9.f),
//        /* Idle[6/8] */F(100,128,22,32,9.f),
//        /* Idle[7/8] */F(122,128,21,32,9.f),
//        /* Idle[8/8] */F(143,128,19,32,9.f)
//    };
//}
//
//static std::vector<FrameMeta> makeDie()
//{
//    return {
//        /* Die[1/12] */F(0,0,19,32,9.f),
//        /* Die[2/12] */F(19,0,20,32,9.f),
//        /* Die[3/12] */F(60,0,23,32,9.f),
//        /* Die[4/12] */F(83,0,33,32,9.f),
//        /* Die[5/12] */F(116,0,38,32,9.f),
//        /* Die[6/12] */F(154,0,38,32,9.f),
//        /* Die[7/12] */F(230,0,40,32,9.f),
//        /* Die[8/12] */F(270,0,40,32,9.f),
//        /* Die[9/12] */F(310,0,42,32,9.f),
//        /* Die[10/12] */F(352,0,42,32,9.f),
//        /* Die[11/12] */F(394,0,42,32,9.f),
//        /* Die[12/12] */F(273,49,42,32,9.f)
//    };
//}
//
void Player::play(AnimId id, bool loop, bool holdLast) {
    // Aseguro textura y sale de cualquier pausa previa
    m_anim.setTexture(*m_sheet);
    m_anim.setPaused(false);

    const std::vector<FrameMeta>& seq = m_frames[id];
    
    if (!loop && holdLast)
    {
        // Reproducir una sola vez y quedarse estatico en el ultimo frame
        m_anim.setHoldOnEnd(true);
        m_anim.loopLastFrame(false);
        m_anim.playOnceHoldLast(m_frames[id]);
        return;
    }

    // Caso general (loop infinito o one-shot sin hold)
    m_anim.setHoldOnEnd(false);
    m_anim.loopLastFrame(false);
    m_anim.setFrames(seq, loop);
}

void Player::playLoopLastFrame(AnimId id)
{
    m_anim.setTexture(*m_sheet);
    m_anim.setPaused(false);

    m_anim.setHoldOnEnd(false);
    m_anim.loopLastFrame(true);
    m_anim.setFrames(m_frames[id], false);
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
void Player::playIfChanged(AnimId id, bool loop, bool holdLast)
{
    if (m_currentAnim == id) return;
    play(id, loop, holdLast);  
    m_currentAnim = id;
}

void Player::updateFacingFromVelocity()
{
    // Flip horizontal con scale.x
    sf::Vector2f s = m_sprite.getScale();
    if (m_velX > 0.f && s.x < 0.f)  m_sprite.setScale({ -s.x, s.y });
    if (m_velX < 0.f && s.x > 0.f)  m_sprite.setScale({ -s.x, s.y });
}

void Player::update(float dt, const sf::RenderWindow& window)
{
    // --- INPUT lateral ---
    float dirX = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dirX -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dirX += 1.f;

    // Velocidad e integracion
    m_velX = dirX * m_speed;
    m_sprite.move({ m_velX * dt, 0.f});
    
    // Flip visual segun direccion
    if (dirX != 0.f) updateFacingFromVelocity();

    // Anim swap Walk/Idle
    if (dirX != 0.f) playIfChanged(AnimId::Walk, true);
    else playIfChanged(AnimId::Idle, true);

    m_anim.update(dt);
    
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

    // --- Botones 
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

