#pragma once
#include "CircleEntity.h"

// ============================================================
//  CircleEnemy — IA multi-couche v2 (ULTRA BANGER EDITION)
// ============================================================
//
//  Fonctionnalités :
//  - États émotionnels (Hunting / Fleeing / Grazing / Patrolling / Hiding)
//  - Mémoire de menace (danger gardé en mémoire N frames hors-vision)
//  - Steering d'interception : prédit la position future de la proie / du danger
//  - Flocking léger : cohésion vers les alliés quand elle erre
//  - Vitesse adaptative : plus rapide en fuite, variable selon état
//  - Curiosité croissante avec la faim (prise de risque accrue)
//  - Wall avoidance lissée + emergency bounce
// ============================================================

enum class AIState
{
    Patrolling,   // Aucun stimulus → patrouille douce avec flocking
    Hunting,      // Proie visible et sûre → interception active
    Grazing,      // Nourriture visible, pas de danger → seek détendu
    Fleeing,      // Danger visible ou souvenir récent de danger
    Hiding,        // Très gros danger, fuite vers un coin éloigné
};

struct ThreatMemory
{
    sf::Vector2f lastPosition;
    sf::Vector2f lastVelocity;
    float        timeAlive    = 0.f;
    float        maxLifetime  = 2.5f;   // secondes de mémoire après perte de vue
    bool         isActive     = false;

    void Update(float dt)
    {
        if (!isActive) return;
        timeAlive += dt;
        if (timeAlive >= maxLifetime)
            isActive = false;
        // projection simple de la menace mémorisée
        lastPosition += lastVelocity * dt * 0.3f;
    }

    void Set(sf::Vector2f pos, sf::Vector2f vel)
    {
        lastPosition = pos;
        lastVelocity = vel;
        timeAlive    = 0.f;
        isActive     = true;
    }

    float Urgency() const
    {
        if (!isActive) return 0.f;
        return 1.f - (timeAlive / maxLifetime);  // 1 = tout frais, 0 = presque oublié
    }
};

class CircleEnemy : public CircleEntity
{
private:
    // ── Vision & stats de base ──────────────────────────────
    float mVisionRange      = 260.f;
    float mInterceptLookahead = 0.35f;   // secondes d'anticipation de la proie

    // ── Vélocité lissée ────────────────────────────────────
    sf::Vector2f mVelocity  { 1.f, 0.f };

    // ── État émotionnel ────────────────────────────────────
    AIState      mState     = AIState::Patrolling;
    float        mStateTimer = 0.f;       // temps passé dans l'état courant

    // ── Mémoire de menace ──────────────────────────────────
    ThreatMemory mThreatMem;

    // ── Faim / curiosité ──────────────────────────────────
    float        mHungerTimer    = 0.f;   // temps sans manger
    float        mRiskThreshold  = 8.f;   // à partir de là, on prend des risques

    // ── Wander interne ────────────────────────────────────
    sf::Vector2f mWanderDir  { 1.f, 0.f };
    float        mWanderTimer = 0.f;

    // ── Vitesse courante (dynamique) ──────────────────────
    float        mCurrentSpeed  = 160.f;

public:
    bool            mIsSplit  = false;

    void TrySplit(sf::Vector2f directionOverride = {0.f, 0.f});
    static constexpr float kMinSplitRadius = 14.f;

public:
    void OnUpdate() override;

private:
    // Helpers
    sf::Vector2f Seek       (sf::Vector2f target) const;
    sf::Vector2f Flee       (sf::Vector2f threat) const;
    sf::Vector2f Intercept  (sf::Vector2f targetPos, sf::Vector2f targetVel) const;
    sf::Vector2f Wander     (float dt);
    sf::Vector2f WallAvoid  () const;
    sf::Vector2f FlockCenter(const std::vector<CircleEntity*>& entities) const;

    void TransitionTo(AIState next);
    float EffectiveVisionRange() const;
};