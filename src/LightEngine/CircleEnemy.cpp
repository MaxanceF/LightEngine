#include "CircleEnemy.h"
#include "SampleScene.h"

#include <cmath>
#include <algorithm>
#include <iostream>
#include <unordered_map>

// ============================================================
//  UTILITAIRES MATHS
// ============================================================

static sf::Vector2f Normalize(sf::Vector2f v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len < 0.0001f) return { 1.f, 0.f };
    return { v.x / len, v.y / len };
}

static float Length(sf::Vector2f v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

static float Distance(sf::Vector2f a, sf::Vector2f b)
{
    float dx = a.x - b.x, dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

static sf::Vector2f Lerp(sf::Vector2f a, sf::Vector2f b, float t)
{
    return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
}

static sf::Vector2f Clamp(sf::Vector2f v, float maxLen)
{
    float len = Length(v);
    if (len > maxLen && len > 0.0001f)
        return { v.x / len * maxLen, v.y / len * maxLen };
    return v;
}

static bool IsSafeFood(sf::Vector2f pos)
{
    const float margin = 80.f;
    return !(pos.x < margin || pos.x > 1280.f - margin ||
             pos.y < margin || pos.y > 720.f  - margin);
}

// Estimation naïve de la vélocité d'une entité (delta de pos sur dt)
// On conserve un sous-map statique pour cela — clé = pointeur, valeur = (pos, vel)
static std::unordered_map<CircleEntity*, sf::Vector2f> sPrevPositions;

static sf::Vector2f EstimateVelocity(CircleEntity* e, float dt)
{
    sf::Vector2f cur = e->GetPosition();
    auto it = sPrevPositions.find(e);
    sf::Vector2f vel = (it != sPrevPositions.end() && dt > 0.001f)
                        ? (cur - it->second) * (1.f / dt)
                        : sf::Vector2f{ 0.f, 0.f };
    sPrevPositions[e] = cur;
    return vel;
}

// ============================================================
//  HELPERS DE STEERING
// ============================================================

sf::Vector2f CircleEnemy::Seek(sf::Vector2f target) const
{
    return Normalize(target - GetPosition());
}

sf::Vector2f CircleEnemy::Flee(sf::Vector2f threat) const
{
    return Normalize(GetPosition() - threat);
}

sf::Vector2f CircleEnemy::Intercept(sf::Vector2f targetPos, sf::Vector2f targetVel) const
{
    // Prédit la position de la cible dans mInterceptLookahead secondes
    sf::Vector2f future = targetPos + targetVel * mInterceptLookahead;
    return Seek(future);
}

sf::Vector2f CircleEnemy::Wander(float dt)
{
    mWanderTimer += dt;

    // Changer légèrement la direction de wander toutes les 0.6–1.2 s
    if (mWanderTimer > 0.6f + (rand() % 100) * 0.006f)
    {
        mWanderTimer = 0.f;
        // Petite rotation aléatoire autour de la direction courante
        float angle = ((float)(rand() % 200) - 100.f) * 0.01f; // ±1 rad
        float c = std::cos(angle), s = std::sin(angle);
        mWanderDir = Normalize({
            mWanderDir.x * c - mWanderDir.y * s,
            mWanderDir.x * s + mWanderDir.y * c
        });
    }
    return mWanderDir;
}

sf::Vector2f CircleEnemy::WallAvoid() const
{
    const float W = 1280.f, H = 720.f;
    const float margin = 60.f;
    sf::Vector2f pos = GetPosition();
    sf::Vector2f avoid{ 0.f, 0.f };

    // Force inversement proportionnelle à la distance au mur
    auto Push = [&](float dist) -> float {
        if (dist <= 0.f) return 1.f;
        return std::max(0.f, 1.f - dist / margin);
    };

    avoid.x += Push(pos.x - 0.f)         * 1.f;
    avoid.x -= Push(W - pos.x)           * 1.f;
    avoid.y += Push(pos.y - 0.f)         * 1.f;
    avoid.y -= Push(H - pos.y)           * 1.f;

    if (Length(avoid) < 0.0001f) return avoid;
    return Normalize(avoid);
}

sf::Vector2f CircleEnemy::FlockCenter(const std::vector<CircleEntity*>& entities) const
{
    sf::Vector2f myPos = GetPosition();
    sf::Vector2f sum{ 0.f, 0.f };
    int count = 0;
    const float flockRange = 120.f;

    for (CircleEntity* e : entities)
    {
        if (e == this || !e->IsTag(2)) continue;  // seulement les alliés
        float d = Distance(myPos, e->GetPosition());
        if (d < flockRange && d > 0.1f)
        {
            sum.x += e->GetPosition().x;
            sum.y += e->GetPosition().y;
            ++count;
        }
    }

    if (count == 0) return { 0.f, 0.f };
    sf::Vector2f center{ sum.x / count, sum.y / count };
    return Seek(center);
}

float CircleEnemy::EffectiveVisionRange() const
{
    // La faim augmente la portée de vision (jusqu'à +40%)
    float hunger01 = std::min(mHungerTimer / mRiskThreshold, 1.f);
    return mVisionRange * (1.f + hunger01 * 0.4f);
}

void CircleEnemy::TransitionTo(AIState next)
{
    mState     = next;
    mStateTimer = 0.f;
}

// ============================================================
//  UPDATE PRINCIPAL
// ============================================================

void CircleEnemy::OnUpdate()
{
    auto* scene = GetScene<SampleScene>();
    float dt    = GetDeltaTime();

    mStateTimer  += dt;
    mHungerTimer += dt;
    mThreatMem.Update(dt);

    sf::Vector2f myPos  = GetPosition();
    float        myR    = GetRadius();
    float        vision = EffectiveVisionRange();

    // ────────────────────────────────────────────────────────
    //  SCAN DU MONDE
    // ────────────────────────────────────────────────────────
    const auto& entities = scene->GetEntities();

    CircleEntity* closestDanger = nullptr;
    CircleEntity* closestPrey   = nullptr;
    CircleEntity* closestFood   = nullptr;

    float dangerDist = vision;
    float preyDist   = vision;
    float foodDist   = vision;

    sf::Vector2f dangerVel{ 0.f, 0.f };
    sf::Vector2f preyVel  { 0.f, 0.f };

    for (CircleEntity* e : entities)
    {
        if (e == this) continue;

        float dist = Distance(myPos, e->GetPosition());
        if (dist > vision) continue;

        sf::Vector2f vel = EstimateVelocity(e, dt);

        // Nourriture (tag 1)
        if (e->IsTag(1) && IsSafeFood(e->GetPosition()))
        {
            if (dist < foodDist) { closestFood = e; foodDist = dist; }
        }

        // Ennemis (tag 2)
        if (e->IsTag(2))
        {
            float ratio = e->GetRadius() / myR;

            if (ratio > 1.15f)  // ennemi plus gros → danger
            {
                if (dist < dangerDist)
                {
                    closestDanger = e;
                    dangerDist    = dist;
                    dangerVel     = vel;
                }
            }
            else if (ratio < 0.85f)  // ennemi plus petit → proie
            {
                if (dist < preyDist)
                {
                    closestPrey = e;
                    preyDist    = dist;
                    preyVel     = vel;
                }
            }
        }
    }

    // Mise à jour de la mémoire de menace
    if (closestDanger)
        mThreatMem.Set(closestDanger->GetPosition(), dangerVel);

    // ────────────────────────────────────────────────────────
    //  MACHINE À ÉTATS
    // ────────────────────────────────────────────────────────

    bool hungry     = mHungerTimer > mRiskThreshold;
    bool hugeDanger = closestDanger && (closestDanger->GetRadius() / myR > 1.8f);

    // Transitions de priorité
    if (closestDanger)
        TransitionTo(hugeDanger ? AIState::Hiding : AIState::Fleeing);
    else if (mThreatMem.isActive && mThreatMem.Urgency() > 0.3f)
        TransitionTo(AIState::Fleeing);   // souvenir encore frais
    else if (closestPrey && (hungry || !closestFood))
        TransitionTo(AIState::Hunting);
    else if (closestFood)
        TransitionTo(AIState::Grazing);
    else
        TransitionTo(AIState::Patrolling);

    // ────────────────────────────────────────────────────────
    //  CALCUL DU VECTEUR DE STEERING
    // ────────────────────────────────────────────────────────
    sf::Vector2f desiredDir{ 0.f, 0.f };
    float        targetSpeed = 160.f;

    const sf::Vector2f wallForce = WallAvoid();
    float wallWeight = 0.6f;

    // Urgence mur : si on touche presque le bord, wallForce explose
    {
        const float W = 1280.f, H = 720.f, em = 10.f;
        if (myPos.x < em || myPos.x > W - em || myPos.y < em || myPos.y > H - em)
            wallWeight = 4.f;
    }

    switch (mState)
    {
    // ── FLEEING ──────────────────────────────────────────
    case AIState::Fleeing:
    {
        sf::Vector2f fleeDir;

        if (closestDanger)
        {
            // Interception inverse : on fuit LOIN de où le danger va êtreq
            sf::Vector2f dangerFuture = closestDanger->GetPosition() + dangerVel * 0.5f;
            fleeDir = Normalize(myPos - dangerFuture);
        }
        else if (mThreatMem.isActive)
        {
            // Fuite depuis la mémoire, attenuée par l'urgence
            fleeDir = Flee(mThreatMem.lastPosition) * mThreatMem.Urgency();
            if (Length(fleeDir) < 0.01f) fleeDir = mWanderDir;
        }

        if (!mIsSplit && closestDanger && GetRadius() > kMinSplitRadius * 1.5f
&& mStateTimer < 0.3f)   // seulement en début de fuite
        {
            sf::Vector2f towardDanger = Normalize(closestDanger->GetPosition() - GetPosition());
            TrySplit(towardDanger);  // le fragment part vers l'ennemi, le corps fuit
        }

        desiredDir  = fleeDir;
        wallWeight  = std::max(wallWeight, 1.8f);
        targetSpeed = 240.f;   // sprint
        break;
    }

    // ── HIDING ────────────────────────────────────────────
    case AIState::Hiding:
    {
        // Cherche le coin le plus loin du danger
        const float W = 1280.f, H = 720.f;
        sf::Vector2f corners[4] = {
            {W * 0.15f, H * 0.15f}, {W * 0.85f, H * 0.15f},
            {W * 0.15f, H * 0.85f}, {W * 0.85f, H * 0.85f}
        };
        sf::Vector2f dangerPos = closestDanger ? closestDanger->GetPosition()
                                               : mThreatMem.lastPosition;
        sf::Vector2f bestCorner = corners[0];
        float bestDist = -1.f;
        for (auto& c : corners)
        {
            float d = Distance(c, dangerPos);
            if (d > bestDist) { bestDist = d; bestCorner = c; }
        }
        desiredDir  = Seek(bestCorner);
        targetSpeed = 220.f;
        break;
    }

    // ── HUNTING ───────────────────────────────────────────
    case AIState::Hunting:
    {
        desiredDir  = Intercept(closestPrey->GetPosition(), preyVel);
        targetSpeed = 175.f;

        // Split offensif : si la proie est dans la portée de saut et qu'on est assez gros
        if (!mIsSplit && closestPrey && GetRadius() > kMinSplitRadius * 1.5f)
        {
            float splitRange = (GetRadius() + closestPrey->GetRadius()) * 3.5f;
            if (preyDist < splitRange)
            {
                sf::Vector2f toTarget = Normalize(closestPrey->GetPosition() - GetPosition());
                TrySplit(toTarget);
            }
        }
        break;
    }

    // ── GRAZING ───────────────────────────────────────────
    case AIState::Grazing:
    {
        desiredDir  = Seek(closestFood->GetPosition());
        targetSpeed = 145.f;
        break;
    }

    // ── PATROLLING ────────────────────────────────────────
    case AIState::Patrolling:
    {
        // Wander + légère cohésion de flock
        sf::Vector2f wander = Wander(dt);
        sf::Vector2f flock  = FlockCenter(entities);

        desiredDir  = Normalize(wander * 0.65f + flock * 0.35f);
        targetSpeed = 130.f;
        break;
    }
        
    }

    // Si le vecteur de direction est nul (cas extrême)
    if (Length(desiredDir) < 0.01f)
        desiredDir = Wander(dt);

    // ────────────────────────────────────────────────────────
    //  COMBINAISON STEERING + WALL
    // ────────────────────────────────────────────────────────
    sf::Vector2f finalDir = desiredDir + wallForce * wallWeight;
    finalDir = Normalize(finalDir);

    if (Length(finalDir) < 0.01f)
        finalDir = mWanderDir;

    // ────────────────────────────────────────────────────────
    //  LISSAGE DE LA VÉLOCITÉ (inertie différente selon état)
    // ────────────────────────────────────────────────────────
    float turnRate = 0.07f;

    switch (mState)
    {
    case AIState::Fleeing:
    case AIState::Hiding:   turnRate = 0.13f; break;   // réactif
    case AIState::Hunting:  turnRate = 0.09f; break;   // agile mais pas nerveux
    case AIState::Grazing:  turnRate = 0.06f; break;   // détendu
    case AIState::Patrolling: turnRate = 0.05f; break; // très doux
    }

    mVelocity = Lerp(mVelocity, finalDir, turnRate);

    if (Length(mVelocity) < 0.02f)
        mVelocity = Normalize(finalDir);

    // ────────────────────────────────────────────────────────
    //  VITESSE ADAPTATIVE (lissée aussi)
    // ────────────────────────────────────────────────────────
    mCurrentSpeed += (targetSpeed - mCurrentSpeed) * 0.06f;

    // ────────────────────────────────────────────────────────
    //  APPLICATION DU MOUVEMENT
    // ────────────────────────────────────────────────────────
    SetDirection(mVelocity.x, mVelocity.y, mCurrentSpeed);
}

void CircleEnemy::TrySplit(sf::Vector2f dirOverride)
{
    if (mIsSplit) return;                          // déjà splitté
    float r = GetRadius();
    if (r < kMinSplitRadius * 1.5f) return;       // trop petit

    // Nouveau rayon : conservation de surface  →  r_fragment = r / sqrt(2)
    float newR = r / std::sqrt(2.f);
    SetRadius(newR);

    // Direction de lancement
    sf::Vector2f launchDir = (Length(dirOverride) > 0.01f)
                              ? Normalize(dirOverride)
                              : mVelocity;
    if (Length(launchDir) < 0.01f) launchDir = { 1.f, 0.f };

    auto* scene    = GetScene<SampleScene>();
    auto* fragment = scene->CreateAndRegister<CircleFragment>(newR, mShape.getFillColor());
    fragment->SetPosition(GetPosition().x, GetPosition().y);
    fragment->SetDirection(launchDir.x, launchDir.y, CircleFragment::kLaunchSpeed);
    fragment->SetTag(2);
    fragment->mOwner  = this;

    mFragment = fragment;
    mIsSplit  = true;
}