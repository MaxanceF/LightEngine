#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class ScreenMelter
{
public:
    void Trigger();
    void Update(float dt);
    void Draw();
    bool IsActive() const { return mPhase != Phase::Idle && mPhase != Phase::Done; }

    // Cubes 4D
    struct Cube4D
    {
        float angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW;
        float speedXY, speedXZ, speedXW, speedYZ, speedYW, speedZW;
        float x, y, scale;
        float alpha;
    };
    std::vector<Cube4D> mCubes4D;

    // Deep glow pulses
    struct GlowPulse { float x, y, radius, maxRadius, speed, alpha; sf::Color color; };
    std::vector<GlowPulse> mGlowPulses;

    // Lignes cyber
    struct CyberLine { float x1,y1,x2,y2; float life, maxLife; sf::Color color; };
    std::vector<CyberLine> mCyberLines;

    // Grille holographique
    float mGridOffset = 0.f;

    // Cercles d'introspection
    struct IntrospectionRing
    {
        float x, y, radius, maxRadius, thickness;
        float life, maxLife;
        sf::Color color;
    };
    std::vector<IntrospectionRing> mRings;

    // Texte glitch
    struct GlitchText { float x, y, life; std::string text; sf::Color color; };
    std::vector<GlitchText>  mGlitchTexts;
    sf::Font                 mFont;
    bool                     mFontLoaded = false;

    // Data stream (pluie de caractères)
    struct DataStream { float x, y, speed, spacing; std::vector<char> chars; };
    std::vector<DataStream> mDataStreams;

    float mCyberTimer = 0.f;

private:
    enum class Phase { Idle, Glitch, Scanlines, Melt, Done };
    Phase mPhase = Phase::Idle;

    float mTimer = 0.f;
    float mTotal = 0.f;

    sf::RenderWindow  mOverlay;
    sf::RenderTexture mCapture;
    sf::RenderTexture mWorkBuffer;

    struct Slice       { float offsetX = 0.f; };
    struct Drip        { float x, y, speed, width, len; sf::Color color; };
    struct GlitchBlock { sf::FloatRect rect; sf::Color color; float life = 0.f; };

    // Effets glitch supplémentaires
    struct ChromaShift { float offsetR, offsetG, offsetB; float life; };
    struct Scanband    { float y, height, alpha, speed; };
    struct Pixelblock  { sf::FloatRect rect; int blockSize; float life; };

    std::vector<Slice>       mSlices;
    std::vector<Drip>        mDrips;
    std::vector<GlitchBlock> mGlitchBlocks;
    std::vector<ChromaShift> mChromaShifts;
    std::vector<Scanband>    mScanbands;
    std::vector<Pixelblock>  mPixelblocks;

    float mVhsNoiseTimer  = 0.f;
    float mFlashTimer     = 0.f;
    float mFlashAlpha     = 1.f;
    float mShakeX         = 1;
    float mShakeY         = 1.f;
    float mShakeTimer     = 0.f;
    float mInvertTimer    = 0.f;
    bool  mInvertActive   = false;
    float mZoomPulse      = 5.f;

    sf::Image   mMeltImage;
    sf::Texture mMeltTexture;
    sf::Sprite  mMeltSprite;

    sf::RenderTexture mChromaBuffer;

    std::vector<float> mColFallSpeed;

    static constexpr float kGlitchDuration   = 2.0f;
    static constexpr float kScanlineDuration  = 2.5f;
    static constexpr float kMeltDuration      = 30.0f;

    float RandF(float min, float max) const;
    int   RandI(int   min, int   max) const;

    void CaptureDesktop();
    void InitOverlayWindow();

    void InitSlices(unsigned int H);
    void InitDrips(unsigned int W, unsigned int H);
    void InitGlitchBlocks(unsigned int W, unsigned int H);
    void InitExtraEffects(unsigned int W, unsigned int H);

    void UpdateGlitch(float dt, unsigned int W, unsigned int H);
    void UpdateScanlines(float dt, unsigned int H);
    void UpdateMelt(float dt, unsigned int W, unsigned int H);
    void UpdateExtraEffects(float dt, unsigned int W, unsigned int H);

    void DrawGlitchScanlines(unsigned int W, unsigned int H);
    void DrawMelt();
    void DrawExtraEffects(unsigned int W, unsigned int H);
};