#include "ScreenMelter.h"
#include "DesktopCapture.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

// ── Helpers ───────────────────────────────────────────────────

float ScreenMelter::RandF(float a, float b) const
{
    return a + (float)std::rand() / RAND_MAX * (b - a);
}

int ScreenMelter::RandI(int a, int b) const
{
    return a + std::rand() % (b - a + 1);
}

// ── Trigger ───────────────────────────────────────────────────

void ScreenMelter::Trigger()
{
    if (mPhase != Phase::Idle) return;

    CaptureDesktop();
    InitOverlayWindow();

    auto [W, H] = mCapture.getSize();

    InitSlices(H);
    InitDrips(W, H);
    InitGlitchBlocks(W, H);
    InitExtraEffects(W, H);

    mMeltImage = mCapture.getTexture().copyToImage();

    mColFallSpeed.resize(W);
    for (auto& v : mColFallSpeed)
        v = RandF(0.4f, 2.2f);

    mMeltTexture.create(W, H);
    mMeltTexture.update(mMeltImage);
    mMeltSprite.setTexture(mMeltTexture, true);

    mChromaBuffer.create(W, H);

    mPhase = Phase::Glitch;
    mTimer = 0.f;
    mTotal = 0.f;
}

void ScreenMelter::CaptureDesktop()
{
    sf::Image img = DesktopCapture::Capture();
    unsigned int W = img.getSize().x;
    unsigned int H = img.getSize().y;

    mCapture.create(W, H);
    mWorkBuffer.create(W, H);

    sf::Texture t;
    t.loadFromImage(img);
    mCapture.draw(sf::Sprite(t));
    mCapture.display();
}

void ScreenMelter::InitOverlayWindow()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    mOverlay.create(desktop, "", sf::Style::None);
    mOverlay.setPosition({ 0, 0 });
    mOverlay.setFramerateLimit(60);

    DesktopCapture::MakeOverlay(mOverlay);
}

// ── Init helpers ──────────────────────────────────────────────

void ScreenMelter::InitSlices(unsigned int H)
{
    mSlices.clear();
    mSlices.resize(H / 2 + 1);
}

void ScreenMelter::InitDrips(unsigned int W, unsigned int H)
{
    mDrips.clear();
    for (int i = 0; i < 80; i++)
    {
        Drip d;
        d.x     = RandF(0.f, (float)W);
        d.y     = RandF(0.f, (float)H * 0.5f);
        d.speed = RandF(20.f, 120.f);
        d.width = RandF(1.f, 9.f);
        d.len   = RandF(15.f, 80.f);
        d.color = sf::Color(RandI(180, 255), RandI(0, 40), RandI(0, 40), 200);
        mDrips.push_back(d);
    }
}

void ScreenMelter::InitGlitchBlocks(unsigned int W, unsigned int H)
{
    mGlitchBlocks.clear();
    for (int i = 0; i < 30; i++)
    {
        GlitchBlock g;
        g.rect  = { RandF(0.f, (float)W), RandF(0.f, (float)H),
                    RandF(10.f, 150.f),   RandF(2.f, 24.f) };
        g.color = sf::Color(RandI(180, 255), RandI(0, 50), RandI(0, 50), 210);
        g.life  = RandF(0.02f, 0.2f);
        mGlitchBlocks.push_back(g);
    }
}

// ── Update ────────────────────────────────────────────────────

void ScreenMelter::Update(float dt)
{
    if (!IsActive()) return;

    sf::Event event;
    while (mOverlay.pollEvent(event))
    {
        if (event.type == sf::Event::KeyPressed
            && event.key.code == sf::Keyboard::Escape)
        {
            mOverlay.close();
            mPhase = Phase::Done;
            return;
        }
    }

    mTotal += dt;
    mTimer += dt;

    auto [W, H] = mCapture.getSize();

    UpdateExtraEffects(dt, W, H);

    switch (mPhase)
    {
    case Phase::Glitch:
        UpdateGlitch(dt, W, H);
        if (mTimer >= kGlitchDuration)
        {
            mTimer = 0.f;
            mPhase = Phase::Scanlines;
        }
        break;

    case Phase::Scanlines:
        UpdateGlitch(dt, W, H);
        UpdateScanlines(dt, H);
        if (mTimer >= kScanlineDuration)
        {
            mMeltImage = mCapture.getTexture().copyToImage();
            mTimer = 0.f;
            mPhase = Phase::Melt;
        }
        break;

    case Phase::Melt:
        UpdateGlitch(dt, W, H);
        UpdateScanlines(dt, H);
        UpdateMelt(dt, W, H);
        if (mTimer >= kMeltDuration)
        {
            mOverlay.close();
            mPhase = Phase::Done;
        }
        break;

    default: break;
    }
}

void ScreenMelter::UpdateGlitch(float dt, unsigned int W, unsigned int H)
{
    float p   = std::min(mTimer / kGlitchDuration, 1.f);
    float amp = p * 45.f;

    for (int i = 0; i < (int)mSlices.size(); i++)
    {
        float target = amp * std::sin(mTotal * 5.f + i * 0.25f)
                     + (p > 0.6f ? RandF(-8.f, 8.f) : 0.f);
        mSlices[i].offsetX += (target - mSlices[i].offsetX) * 10.f * dt;
    }

    for (auto& g : mGlitchBlocks)
    {
        g.life -= dt;
        if (g.life <= 0.f)
        {
            g.rect  = { RandF(0.f, (float)W), RandF(0.f, (float)H),
                        RandF(10.f, 160.f),   RandF(2.f, 28.f) };
            g.color = sf::Color(RandI(160, 255), RandI(0, 60), RandI(0, 80), 220);
            g.life  = RandF(0.02f, 0.18f);
        }
    }
}

void ScreenMelter::UpdateScanlines(float dt, unsigned int H)
{
    float p = std::min(mTimer / kScanlineDuration, 1.f);
    for (auto& d : mDrips)
    {
        d.y += d.speed * (1.f + p * 5.f) * dt;
        if (d.y > (float)H + d.len)
            d.y = RandF(-(float)d.len, 0.f);
    }
}

void ScreenMelter::UpdateMelt(float dt, unsigned int W, unsigned int H)
{
    float p = std::min(mTimer / kMeltDuration, 1.f);

    for (unsigned int x = 0; x < W; x++)
    {
        int shift = (int)(
            p * mColFallSpeed[x] * (float)H * 0.65f
            + std::sin(x * 0.07f + mTotal * 2.f) * p * 25.f
        );
        if (shift <= 0) continue;

        for (int y = (int)H - 1; y >= 0; y--)
        {
            int src = y - shift;
            sf::Color c = (src >= 0)
                ? mMeltImage.getPixel(x, (unsigned int)src)
                : sf::Color(RandI(120, 220), 0, 0, 255);

            c.g = (sf::Uint8)(c.g * (1.f - p * 0.95f));
            c.b = (sf::Uint8)(c.b * (1.f - p * 0.95f));
            c.r = (sf::Uint8)std::min(255, (int)c.r + (int)(p * 90));

            mMeltImage.setPixel(x, (unsigned int)y, c);
        }
    }

    int noise = (int)(p * p * 2500.f);
    for (int i = 0; i < noise; i++)
    {
        mMeltImage.setPixel(
            (unsigned int)RandI(0, (int)W - 1),
            (unsigned int)RandI(0, (int)H - 1),
            sf::Color(RandI(180, 255), 0, 0, 255)
        );
    }

    mMeltTexture.update(mMeltImage);
}


// ── Draw ──────────────────────────────────────────────────────

void ScreenMelter::Draw()
{
    if (!IsActive()) return;

    auto [W, H] = mCapture.getSize();

    mOverlay.clear(sf::Color::Black);

    if (mPhase == Phase::Melt)
    {
        DrawMelt();
        DrawGlitchScanlines(W, H);
    }
    else
    {
        DrawGlitchScanlines(W, H);
    }

    DrawExtraEffects(W, H);

    mOverlay.display();
}

void ScreenMelter::DrawGlitchScanlines(unsigned int W, unsigned int H)
{
    mWorkBuffer.clear(sf::Color::Black);

    // Aberration chromatique : 3 passes R/G/B décalées
    if (!mChromaShifts.empty())
    {
        float ox = mChromaShifts[0].offsetR;
        float oy = mChromaShifts[0].offsetG;

        sf::Sprite chromaSpr(mPhase == Phase::Melt
            ? mMeltTexture
            : mCapture.getTexture());

        // Canal rouge
        chromaSpr.setColor(sf::Color(255, 0, 0, 120));
        chromaSpr.setPosition(ox, 0.f);
        chromaSpr.setScale(mZoomPulse, mZoomPulse);
        mWorkBuffer.draw(chromaSpr, sf::BlendAdd);

        // Canal vert
        chromaSpr.setColor(sf::Color(0, 255, 0, 120));
        chromaSpr.setPosition(0.f, oy);
        mWorkBuffer.draw(chromaSpr, sf::BlendAdd);

        // Canal bleu
        chromaSpr.setColor(sf::Color(0, 0, 255, 120));
        chromaSpr.setPosition(mChromaShifts[0].offsetB * 0.5f, 0.f);
        mWorkBuffer.draw(chromaSpr, sf::BlendAdd);

        chromaSpr.setColor(sf::Color::White);
        chromaSpr.setPosition(mShakeX, mShakeY);
        chromaSpr.setScale(mZoomPulse, mZoomPulse);
        mWorkBuffer.draw(chromaSpr, sf::BlendMultiply);
    }
    else
    {
        sf::Sprite base(mCapture.getTexture());
        base.setPosition(mShakeX, mShakeY);
        base.setScale(mZoomPulse, mZoomPulse);
        mWorkBuffer.draw(base);
    }

    // Slices décalées par-dessus
    sf::Sprite sliceSpr(mCapture.getTexture());
    for (int i = 0; i < (int)mSlices.size(); i++)
    {
        unsigned int y = (unsigned int)(i * 2);
        if (y >= H) break;

        float ox = mSlices[i].offsetX + mShakeX;
        if (std::abs(ox) < 0.5f) continue;

        sliceSpr.setTextureRect({ 0, (int)y, (int)W, 2 });
        sliceSpr.setPosition(ox, (float)y);
        mWorkBuffer.draw(sliceSpr);

        if (ox > 0.f)
        {
            sliceSpr.setPosition(ox - (float)W, (float)y);
            mWorkBuffer.draw(sliceSpr);
        }
        else
        {
            sliceSpr.setPosition(ox + (float)W, (float)y);
            mWorkBuffer.draw(sliceSpr);
        }
    }

    // Blocs glitch
    for (const auto& g : mGlitchBlocks)
    {
        if (RandI(0, 1) == 0) continue;
        sf::RectangleShape r({ g.rect.width, g.rect.height });
        r.setPosition(g.rect.left, g.rect.top);
        r.setFillColor(g.color);
        mWorkBuffer.draw(r, sf::BlendAdd);
    }

    // Blocs pixelisés
    for (const auto& pb : mPixelblocks)
    {
        if (RandI(0, 2) == 0) continue;
        int bs = pb.blockSize;
        int x0 = (int)pb.rect.left;
        int y0 = (int)pb.rect.top;
        int pw = (int)pb.rect.width;
        int ph = (int)pb.rect.height;

        for (int by = 0; by < ph; by += bs)
        for (int bx = 0; bx < pw; bx += bs)
        {
            int sx = std::min(x0 + bx, (int)W - 1);
            int sy = std::min(y0 + by, (int)H - 1);
            sf::Color c = mMeltImage.getPixel(
                (unsigned int)std::max(0, sx),
                (unsigned int)std::max(0, sy)
            );
            sf::RectangleShape block({ (float)bs, (float)bs });
            block.setPosition((float)(x0 + bx), (float)(y0 + by));
            block.setFillColor(c);
            mWorkBuffer.draw(block);
        }
    }

    // Bandes VCR sombres
    for (const auto& s : mScanbands)
    {
        sf::RectangleShape band({ (float)W, s.height });
        band.setPosition(0.f, s.y);
        band.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)s.alpha));
        mWorkBuffer.draw(band);
    }

    // VHS noise horizontal (lignes parasites)
    if ((int)(mVhsNoiseTimer * 30.f) % 3 == 0)
    {
        int lineCount = RandI(1, 5);
        for (int i = 0; i < lineCount; i++)
        {
            sf::RectangleShape noise({ RandF((float)W * 0.3f, (float)W), 1.f });
            noise.setPosition(RandF(0.f, (float)W * 0.3f), RandF(0.f, (float)H));
            noise.setFillColor(sf::Color(255, 255, 255, RandI(40, 120)));
            mWorkBuffer.draw(noise, sf::BlendAdd);
        }
    }

    // Scanlines + overlay rouge + drips
    if (mPhase == Phase::Scanlines || mPhase == Phase::Melt)
    {
        float p = std::min(mTimer / kScanlineDuration, 1.f);

        sf::RectangleShape overlay({ (float)W, (float)H });
        overlay.setFillColor(sf::Color(180, 0, 0, (sf::Uint8)(p * 50.f)));
        mWorkBuffer.draw(overlay);

        sf::RectangleShape line({ (float)W, 2.f });
        for (unsigned int y = 0; y < H; y += 4)
        {
            float alpha = p * 110.f * (0.5f + 0.5f * std::sin(y * 0.5f + mTotal * 6.f));
            line.setPosition(0.f, (float)y);
            line.setFillColor(sf::Color(210, 0, 0, (sf::Uint8)alpha));
            mWorkBuffer.draw(line);
        }

        for (const auto& d : mDrips)
        {
            sf::RectangleShape drip({ d.width, d.len });
            drip.setPosition(d.x, d.y - d.len);
            sf::Color c = d.color;
            c.a = (sf::Uint8)(p * 210.f);
            drip.setFillColor(c);
            mWorkBuffer.draw(drip);
        }
    }

    // Inversion couleurs (post-process sur l'image du workbuffer)
    if (mInvertActive)
    {
        sf::RectangleShape inv({ (float)W, (float)H });
        inv.setFillColor(sf::Color(255, 255, 255, 255));
        mWorkBuffer.draw(inv, sf::BlendMode(
            sf::BlendMode::OneMinusDstColor,
            sf::BlendMode::Zero,
            sf::BlendMode::Add,
            sf::BlendMode::OneMinusDstColor,
            sf::BlendMode::Zero,
            sf::BlendMode::Add
        ));
    }

    mWorkBuffer.display();
    mOverlay.draw(sf::Sprite(mWorkBuffer.getTexture()));
}

void ScreenMelter::DrawMelt()
{
    float p = std::min(mTimer / kMeltDuration, 1.f);

    sf::Sprite meltSpr = mMeltSprite;
    meltSpr.setPosition(mShakeX, mShakeY);
    meltSpr.setScale(mZoomPulse, mZoomPulse);
    mOverlay.draw(meltSpr);

    sf::RectangleShape vignette({
        (float)mCapture.getSize().x,
        (float)mCapture.getSize().y
    });
    vignette.setFillColor(sf::Color(150, 0, 0, (sf::Uint8)(p * 120.f)));
    mOverlay.draw(vignette, sf::BlendAdd);
}

// ── Projection 4D → 2D ────────────────────────────────────────

// Rotation dans un plan (i,j) d'un vecteur 4D
static void Rotate4D(float v[4], int i, int j, float angle)
{
    float c = std::cos(angle), s = std::sin(angle);
    float vi = v[i], vj = v[j];
    v[i] = vi * c - vj * s;
    v[j] = vi * s + vj * c;
}

// Retourne les 8 sommets du hypercube projetés en 2D
static std::vector<sf::Vector2f> ProjectHypercube(
    const ScreenMelter::Cube4D& cube, float cx, float cy)
{
    // Les 16 sommets du tesseract en 4D (±1 sur chaque axe)
    float verts[16][4];
    int idx = 0;
    for (int a = -1; a <= 1; a += 2)
    for (int b = -1; b <= 1; b += 2)
    for (int c2 = -1; c2 <= 1; c2 += 2)
    for (int d = -1; d <= 1; d += 2)
    {
        verts[idx][0] = (float)a;
        verts[idx][1] = (float)b;
        verts[idx][2] = (float)c2;
        verts[idx][3] = (float)d;
        idx++;
    }

    std::vector<sf::Vector2f> proj(16);
    for (int i = 0; i < 16; i++)
    {
        float v[4] = { verts[i][0], verts[i][1], verts[i][2], verts[i][3] };

        // 6 rotations dans les 6 plans du 4D
        Rotate4D(v, 0, 1, cube.angleXY);
        Rotate4D(v, 0, 2, cube.angleXZ);
        Rotate4D(v, 0, 3, cube.angleXW);
        Rotate4D(v, 1, 2, cube.angleYZ);
        Rotate4D(v, 1, 3, cube.angleYW);
        Rotate4D(v, 2, 3, cube.angleZW);

        // Projection perspective 4D→3D (distance W)
        float w4 = 1.f / (2.5f - v[3]);
        float x3 = v[0] * w4;
        float y3 = v[1] * w4;
        float z3 = v[2] * w4;

        // Projection perspective 3D→2D
        float w3 = 1.f / (3.f - z3);
        proj[i].x = cx + x3 * w3 * cube.scale;
        proj[i].y = cy + y3 * w3 * cube.scale;
    }
    return proj;
}

// Arêtes du tesseract : deux sommets sont connectés si
// ils diffèrent sur exactement 1 coordonnée binaire
static std::vector<std::pair<int,int>> HypercubeEdges()
{
    std::vector<std::pair<int,int>> edges;
    // Reconstruire les indices à partir du même encodage que ProjectHypercube
    int coords[16][4];
    int idx = 0;
    for (int a = -1; a <= 1; a += 2)
    for (int b = -1; b <= 1; b += 2)
    for (int c = -1; c <= 1; c += 2)
    for (int d = -1; d <= 1; d += 2)
    {
        coords[idx][0] = a; coords[idx][1] = b;
        coords[idx][2] = c; coords[idx][3] = d;
        idx++;
    }
    for (int i = 0; i < 16; i++)
    for (int j = i+1; j < 16; j++)
    {
        int diff = 0;
        for (int k = 0; k < 4; k++)
            if (coords[i][k] != coords[j][k]) diff++;
        if (diff == 1) edges.push_back({i, j});
    }
    return edges;
}

static const std::vector<std::pair<int,int>> kEdges = HypercubeEdges();

// ── Init ──────────────────────────────────────────────────────

void ScreenMelter::InitExtraEffects(unsigned int W, unsigned int H)
{
    mFontLoaded = mFont.loadFromFile("../../../res/Hack-Regular.ttf");

    // Aberration chromatique
    mChromaShifts.clear();
    for (int i = 0; i < 5; i++)
    {
        ChromaShift c;
        c.offsetR = RandF(-12.f, 12.f);
        c.offsetG = RandF(-8.f,  8.f);
        c.offsetB = RandF(-12.f, 12.f);
        c.life    = RandF(0.1f, 0.4f);
        mChromaShifts.push_back(c);
    }

    // Bandes VCR
    mScanbands.clear();
    for (int i = 0; i < 6; i++)
    {
        Scanband s;
        s.y      = RandF(0.f, (float)H);
        s.height = RandF(10.f, 60.f);
        s.alpha  = RandF(80.f, 180.f);
        s.speed  = RandF(80.f, 300.f) * (RandI(0,1) ? 1.f : -1.f);
        mScanbands.push_back(s);
    }

    // Blocs pixelisés
    mPixelblocks.clear();
    for (int i = 0; i < 12; i++)
    {
        Pixelblock p;
        p.rect      = { RandF(0.f,(float)W), RandF(0.f,(float)H),
                        RandF(40.f, 200.f),  RandF(30.f, 120.f) };
        p.blockSize = RandI(4, 20);
        p.life      = RandF(0.05f, 0.25f);
        mPixelblocks.push_back(p);
    }

    // Cubes 4D
    mCubes4D.clear();
    for (int i = 0; i < 4; i++)
    {
        Cube4D c;
        c.angleXY = RandF(0.f, 6.28f); c.angleXZ = RandF(0.f, 6.28f);
        c.angleXW = RandF(0.f, 6.28f); c.angleYZ = RandF(0.f, 6.28f);
        c.angleYW = RandF(0.f, 6.28f); c.angleZW = RandF(0.f, 6.28f);
        c.speedXY = RandF(0.3f, 1.2f); c.speedXZ = RandF(0.2f, 0.9f);
        c.speedXW = RandF(0.4f, 1.4f); c.speedYZ = RandF(0.1f, 0.8f);
        c.speedYW = RandF(0.3f, 1.1f); c.speedZW = RandF(0.2f, 1.0f);
        c.x       = RandF((float)W * 0.1f, (float)W * 0.9f);
        c.y       = RandF((float)H * 0.1f, (float)H * 0.9f);
        c.scale   = RandF(60.f, 140.f);
        c.alpha   = RandF(120.f, 200.f);
        mCubes4D.push_back(c);
    }

    // Glow pulses
    mGlowPulses.clear();
    for (int i = 0; i < 8; i++)
    {
        GlowPulse g;
        g.x         = RandF(0.f, (float)W);
        g.y         = RandF(0.f, (float)H);
        g.radius    = 0.f;
        g.maxRadius = RandF(80.f, 300.f);
        g.speed     = RandF(60.f, 200.f);
        g.alpha     = RandF(80.f, 160.f);
        int t       = RandI(0, 3);
        sf::Color cols[] = {
            sf::Color(0, 255, 180),
            sf::Color(255, 0, 120),
            sf::Color(0, 180, 255),
            sf::Color(180, 0, 255)
        };
        g.color = cols[t];
        mGlowPulses.push_back(g);
    }

    // Lignes cyber
    mCyberLines.clear();
    for (int i = 0; i < 20; i++)
    {
        CyberLine l;
        l.x1 = RandF(0.f,(float)W); l.y1 = RandF(0.f,(float)H);
        l.x2 = RandF(0.f,(float)W); l.y2 = RandF(0.f,(float)H);
        l.maxLife = RandF(0.1f, 0.5f);
        l.life    = l.maxLife;
        sf::Color cc[] = {
            sf::Color(0,255,180,200), sf::Color(255,0,120,200),
            sf::Color(0,180,255,200), sf::Color(255,255,0,180)
        };
        l.color = cc[RandI(0,3)];
        mCyberLines.push_back(l);
    }

    // Rings d'introspection
    mRings.clear();
    for (int i = 0; i < 6; i++)
    {
        IntrospectionRing r;
        r.x         = RandF((float)W*0.2f, (float)W*0.8f);
        r.y         = RandF((float)H*0.2f, (float)H*0.8f);
        r.radius    = 0.f;
        r.maxRadius = RandF(100.f, 400.f);
        r.thickness = RandF(1.f, 4.f);
        r.maxLife   = r.maxRadius / RandF(80.f, 200.f);
        r.life      = r.maxLife;
        sf::Color rc[] = {
            sf::Color(0,255,180), sf::Color(0,180,255),
            sf::Color(180,0,255), sf::Color(255,200,0)
        };
        r.color = rc[RandI(0,3)];
        mRings.push_back(r);
    }

    // Textes glitch
    const char* kTexts[] = {
        "SYSTEM FAILURE", "KERNEL PANIC", "0xDEADBEEF",
        "ACCESS DENIED", "BUFFER OVERFLOW", "SEGFAULT",
        "NULL POINTER", "STACK CORRUPT", "MEMORY LEAK",
        "FATAL ERROR", "CONNECTION LOST", "PROCESS KILLED"
    };
    mGlitchTexts.clear();
    for (int i = 0; i < 8; i++)
    {
        GlitchText t;
        t.x    = RandF(0.f, (float)W * 0.8f);
        t.y    = RandF(0.f, (float)H);
        t.life = RandF(0.2f, 1.f);
        t.text = kTexts[RandI(0, 11)];
        sf::Color tc[] = {
            sf::Color(0,255,180), sf::Color(255,50,50),
            sf::Color(0,200,255), sf::Color(255,255,0)
        };
        t.color = tc[RandI(0,3)];
        mGlitchTexts.push_back(t);
    }

    // Data streams
    mDataStreams.clear();
    const char kChars[] = "0123456789ABCDEF><|=-+*#@!?";
    for (int i = 0; i < 30; i++)
    {
        DataStream s;
        s.x       = RandF(0.f, (float)W);
        s.y       = RandF(-(float)H, 0.f);
        s.speed   = RandF(60.f, 200.f);
        s.spacing = RandF(14.f, 20.f);
        int len   = RandI(5, 20);
        for (int j = 0; j < len; j++)
            s.chars.push_back(kChars[RandI(0, (int)sizeof(kChars)-2)]);
        mDataStreams.push_back(s);
    }

    mVhsNoiseTimer = 0.f;
    mFlashTimer    = RandF(0.3f, 1.f);
    mFlashAlpha    = 0.f;
    mShakeTimer    = 0.f;
    mShakeX        = 0.f;
    mShakeY        = 0.f;
    mInvertTimer   = RandF(1.f, 2.5f);
    mInvertActive  = false;
    mZoomPulse     = 1.f;
    mGridOffset    = 0.f;
    mCyberTimer    = 0.f;
}

// ── UpdateExtraEffects ────────────────────────────────────────

void ScreenMelter::UpdateExtraEffects(float dt, unsigned int W, unsigned int H)
{
    float globalP = std::min(mTotal / (kGlitchDuration + kScanlineDuration + kMeltDuration), 1.f);

    mCyberTimer += dt;

    // Aberration chromatique
    for (auto& c : mChromaShifts)
    {
        c.life -= dt;
        if (c.life <= 0.f)
        {
            float amp = 8.f + globalP * 20.f;
            c.offsetR = RandF(-amp, amp);
            c.offsetG = RandF(-amp*0.5f, amp*0.5f);
            c.offsetB = RandF(-amp, amp);
            c.life    = RandF(0.05f, 0.3f);
        }
    }

    // Bandes VCR
    for (auto& s : mScanbands)
    {
        s.y += s.speed * dt;
        if (s.y > (float)H) s.y = -(float)s.height;
        if (s.y < -(float)s.height) s.y = (float)H;
    }

    // Blocs pixelisés
    for (auto& p : mPixelblocks)
    {
        p.life -= dt;
        if (p.life <= 0.f)
        {
            p.rect      = { RandF(0.f,(float)W),  RandF(0.f,(float)H),
                            RandF(40.f, 200.f),   RandF(30.f, 120.f) };
            p.blockSize = RandI(4, 20);
            p.life      = RandF(0.04f, 0.2f);
        }
    }

    // Cubes 4D
    for (auto& c : mCubes4D)
    {
        float speed = 1.f + globalP * 2.f;
        c.angleXY += c.speedXY * dt * speed;
        c.angleXZ += c.speedXZ * dt * speed;
        c.angleXW += c.speedXW * dt * speed;
        c.angleYZ += c.speedYZ * dt * speed;
        c.angleYW += c.speedYW * dt * speed;
        c.angleZW += c.speedZW * dt * speed;
    }

    // Glow pulses
    for (auto& g : mGlowPulses)
    {
        g.radius += g.speed * dt;
        if (g.radius > g.maxRadius)
        {
            g.radius    = 0.f;
            g.x         = RandF(0.f, (float)W);
            g.y         = RandF(0.f, (float)H);
            g.maxRadius = RandF(80.f, 300.f);
            g.speed     = RandF(60.f, 200.f);
        }
    }

    // Lignes cyber
    for (auto& l : mCyberLines)
    {
        l.life -= dt;
        if (l.life <= 0.f)
        {
            // Lignes en L (cyber grid style)
            l.x1 = RandF(0.f,(float)W);
            l.y1 = RandF(0.f,(float)H);
            if (RandI(0,1))
            {
                l.x2 = RandF(0.f,(float)W);
                l.y2 = l.y1;
            }
            else
            {
                l.x2 = l.x1;
                l.y2 = RandF(0.f,(float)H);
            }
            l.maxLife = RandF(0.05f, 0.4f);
            l.life    = l.maxLife;
            sf::Color cc[] = {
                sf::Color(0,255,180,220), sf::Color(255,0,120,220),
                sf::Color(0,180,255,220), sf::Color(255,255,0,200)
            };
            l.color = cc[RandI(0,3)];
        }
    }

    // Introspection rings
    for (auto& r : mRings)
    {
        r.life -= dt;
        float t = 1.f - (r.life / r.maxLife);
        r.radius = t * r.maxRadius;
        if (r.life <= 0.f)
        {
            r.x         = RandF((float)W*0.1f, (float)W*0.9f);
            r.y         = RandF((float)H*0.1f, (float)H*0.9f);
            r.maxRadius = RandF(80.f, 400.f);
            r.maxLife   = r.maxRadius / RandF(80.f, 200.f);
            r.life      = r.maxLife;
            r.radius    = 0.f;
            sf::Color rc[] = {
                sf::Color(0,255,180), sf::Color(0,180,255),
                sf::Color(180,0,255), sf::Color(255,200,0)
            };
            r.color = rc[RandI(0,3)];
        }
    }

    // Textes glitch
    const char* kTexts[] = {
        "SYSTEM FAILURE", "KERNEL PANIC",   "0xDEADBEEF",
        "ACCESS DENIED",  "BUFFER OVERFLOW","SEGFAULT",
        "NULL POINTER",   "STACK CORRUPT",  "MEMORY LEAK",
        "FATAL ERROR",    "CONNECTION LOST","PROCESS KILLED",
        "0xFF000000",     "ERR_CODE: 0x4C", "FATAL: 0xC000021A"
    };
    for (auto& t : mGlitchTexts)
    {
        t.life -= dt;
        if (t.life <= 0.f)
        {
            t.x    = RandF(0.f, (float)W * 0.75f);
            t.y    = RandF(0.f, (float)H);
            t.life = RandF(0.15f, 0.8f);
            t.text = kTexts[RandI(0, 14)];
            sf::Color tc[] = {
                sf::Color(0,255,180), sf::Color(255,50,50),
                sf::Color(0,200,255), sf::Color(255,255,0)
            };
            t.color = tc[RandI(0,3)];
        }
    }

    // Data streams
    for (auto& s : mDataStreams)
    {
        s.y += s.speed * dt;
        if (s.y > (float)H + s.chars.size() * s.spacing)
        {
            s.y = RandF(-(float)s.chars.size() * s.spacing, 0.f);
            s.x = RandF(0.f, (float)W);
            // Muter quelques caractères
            const char kChars[] = "0123456789ABCDEF><|=-+*#@!?";
            for (auto& ch : s.chars)
                if (RandI(0,4) == 0)
                    ch = kChars[RandI(0,(int)sizeof(kChars)-2)];
        }
    }

    // Flash
    mFlashTimer -= dt;
    if (mFlashTimer <= 0.f)
    {
        mFlashAlpha = RandF(80.f, 200.f);
        mFlashTimer = RandF(0.4f + (1.f - globalP), 2.f);
    }
    mFlashAlpha = std::max(0.f, mFlashAlpha - dt * 400.f);

    // Shake
    mShakeTimer -= dt;
    if (mShakeTimer <= 0.f)
    {
        float shakeAmp = globalP * 18.f;
        mShakeX     = RandF(-shakeAmp, shakeAmp);
        mShakeY     = RandF(-shakeAmp * 0.5f, shakeAmp * 0.5f);
        mShakeTimer = RandF(0.05f, 0.15f);
    }

    // Inversion
    mInvertTimer -= dt;
    if (mInvertTimer <= 0.f)
    {
        mInvertActive = !mInvertActive;
        mInvertTimer  = mInvertActive
            ? RandF(0.05f, 0.2f)
            : RandF(0.5f + (1.f - globalP), 2.f);
    }

    // Zoom pulse
    mZoomPulse = 1.f + std::sin(mTotal * 3.f) * globalP * 0.015f;

    // Grid offset
    mGridOffset += dt * 40.f;

    mVhsNoiseTimer += dt;
}

// ── DrawExtraEffects ──────────────────────────────────────────

void ScreenMelter::DrawExtraEffects(unsigned int W, unsigned int H)
{
    float globalP = std::min(mTotal / (kGlitchDuration + kScanlineDuration + kMeltDuration), 1.f);

    // 1. Grille holographique cyber
    {
        float gridSize = 60.f;
        sf::Color gridColor(0, 220, 180, (sf::Uint8)(30.f + globalP * 50.f));
        sf::RectangleShape hline({ (float)W, 1.f });
        hline.setFillColor(gridColor);
        float offset = std::fmod(mGridOffset, gridSize);
        for (float y = offset; y < (float)H; y += gridSize)
        {
            hline.setPosition(0.f, y);
            mOverlay.draw(hline);
        }
        sf::RectangleShape vline({ 1.f, (float)H });
        vline.setFillColor(gridColor);
        for (float x = 0.f; x < (float)W; x += gridSize)
        {
            vline.setPosition(x, 0.f);
            mOverlay.draw(vline);
        }
    }

    // 2. Glow pulses (cercles concentriques qui s'expandent)
    for (const auto& g : mGlowPulses)
    {
        if (g.radius < 1.f) continue;
        float t = g.radius / g.maxRadius;
        sf::Uint8 alpha = (sf::Uint8)(g.alpha * (1.f - t));

        // 3 anneaux concentriques par pulse pour le deep glow
        for (int ring = 0; ring < 3; ring++)
        {
            float r = g.radius - ring * 12.f;
            if (r < 1.f) continue;

            int pts = std::max(32, (int)(r * 0.5f));
            sf::VertexArray circle(sf::LinesStrip, pts + 1);
            for (int i = 0; i <= pts; i++)
            {
                float angle = (float)i / pts * 6.2832f;
                circle[i].position = {
                    g.x + std::cos(angle) * r,
                    g.y + std::sin(angle) * r
                };
                sf::Uint8 a = (sf::Uint8)(alpha * (1.f - ring * 0.3f));
                circle[i].color = sf::Color(g.color.r, g.color.g, g.color.b, a);
            }
            mOverlay.draw(circle, sf::BlendAdd);
        }
    }

    // 3. Lignes cyber
    for (const auto& l : mCyberLines)
    {
        float t = l.life / l.maxLife;
        sf::Uint8 alpha = (sf::Uint8)(l.color.a * t);
        sf::Color c(l.color.r, l.color.g, l.color.b, alpha);

        sf::Vertex line[2];
        line[0].position = { l.x1, l.y1 };
        line[0].color    = c;
        line[1].position = { l.x2, l.y2 };
        line[1].color    = sf::Color(c.r, c.g, c.b, (sf::Uint8)(alpha/3));
        mOverlay.draw(line, 2, sf::Lines, sf::BlendAdd);

        // Point brillant aux extrémités
        sf::CircleShape dot(3.f);
        dot.setOrigin(3.f, 3.f);
        dot.setFillColor(sf::Color(255, 255, 255, alpha));
        dot.setPosition(l.x1, l.y1);
        mOverlay.draw(dot, sf::BlendAdd);
    }

    // 4. Introspection rings
    for (const auto& r : mRings)
    {
        if (r.radius < 1.f) continue;
        float t = 1.f - (r.life / r.maxLife);
        sf::Uint8 alpha = (sf::Uint8)(180.f * (1.f - t));

        int pts = std::max(32, (int)(r.radius * 0.4f));
        sf::VertexArray circle(sf::LinesStrip, pts + 1);
        for (int i = 0; i <= pts; i++)
        {
            float angle = (float)i / pts * 6.2832f;
            circle[i].position = {
                r.x + std::cos(angle) * r.radius,
                r.y + std::sin(angle) * r.radius
            };
            circle[i].color = sf::Color(r.color.r, r.color.g, r.color.b, alpha);
        }
        mOverlay.draw(circle, sf::BlendAdd);

        // Tick marks sur le ring (style radar)
        int ticks = 8;
        for (int i = 0; i < ticks; i++)
        {
            float angle = (float)i / ticks * 6.2832f;
            sf::Vertex tick[2];
            tick[0].position = {
                r.x + std::cos(angle) * (r.radius - 8.f),
                r.y + std::sin(angle) * (r.radius - 8.f)
            };
            tick[1].position = {
                r.x + std::cos(angle) * (r.radius + 8.f),
                r.y + std::sin(angle) * (r.radius + 8.f)
            };
            tick[0].color = tick[1].color = sf::Color(r.color.r, r.color.g, r.color.b, alpha);
            mOverlay.draw(tick, 2, sf::Lines, sf::BlendAdd);
        }
    }

    // 5. Cubes 4D (tesseract)
    for (const auto& cube : mCubes4D)
    {
        auto verts = ProjectHypercube(cube, cube.x, cube.y);

        for (const auto& [i, j] : kEdges)
        {
            sf::Vertex line[2];
            line[0].position = verts[i];
            line[1].position = verts[j];

            // Couleur selon profondeur W
            float depthI = (float)i / 15.f;
            sf::Color c(
                (sf::Uint8)(0   + depthI * 100),
                (sf::Uint8)(180 + depthI * 75),
                (sf::Uint8)(255 - depthI * 100),
                (sf::Uint8)cube.alpha
            );
            line[0].color = c;
            line[1].color = sf::Color(c.r/2, c.g/2, c.b, (sf::Uint8)(cube.alpha*0.4f));

            mOverlay.draw(line, 2, sf::Lines, sf::BlendAdd);
        }

        // Points aux sommets
        for (const auto& v : verts)
        {
            sf::CircleShape dot(2.f);
            dot.setOrigin(2.f, 2.f);
            dot.setPosition(v);
            dot.setFillColor(sf::Color(100, 255, 255, (sf::Uint8)cube.alpha));
            mOverlay.draw(dot, sf::BlendAdd);
        }
    }

    // 6. Data streams (pluie de code)
    if (mFontLoaded)
    {
        for (const auto& s : mDataStreams)
        {
            for (int i = 0; i < (int)s.chars.size(); i++)
            {
                float cy = s.y + i * s.spacing;
                if (cy < 0.f || cy > (float)H) continue;

                float fade = 1.f - (float)i / s.chars.size();
                sf::Text txt;
                txt.setFont(mFont);
                txt.setCharacterSize(12);
                txt.setString(s.chars[i]);
                txt.setPosition(s.x, cy);

                // Tête = blanc brillant, queue = vert sombre
                if (i == 0)
                    txt.setFillColor(sf::Color(200, 255, 200, 240));
                else
                    txt.setFillColor(sf::Color(0, (sf::Uint8)(180 * fade), (sf::Uint8)(80 * fade), (sf::Uint8)(200 * fade)));

                mOverlay.draw(txt, sf::BlendAdd);
            }
        }
    }

    // 7. Textes glitch
    if (mFontLoaded)
    {
        for (const auto& t : mGlitchTexts)
        {
            sf::Text txt;
            txt.setFont(mFont);
            txt.setCharacterSize(RandI(10, 18));
            txt.setString(t.text);
            txt.setPosition(t.x + RandF(-2.f, 2.f), t.y);
            txt.setFillColor(t.color);
            mOverlay.draw(txt, sf::BlendAdd);
        }
    }

    // 8. Flash
    if (mFlashAlpha > 1.f)
    {
        sf::RectangleShape flash({ (float)W, (float)H });
        flash.setFillColor(sf::Color(255, 200, 200, (sf::Uint8)mFlashAlpha));
        mOverlay.draw(flash, sf::BlendAdd);
    }

    // 9. Vignette rouge permanente qui s'intensifie
    {
        float vigAlpha = globalP * 80.f;
        sf::RectangleShape vig({ (float)W, (float)H });
        vig.setFillColor(sf::Color(120, 0, 0, (sf::Uint8)vigAlpha));
        mOverlay.draw(vig, sf::BlendAdd);
    }
}