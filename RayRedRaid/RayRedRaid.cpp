#define RAYGUI_IMPLEMENTATION

#include "raylib.h"
#include "raymath.h"
//#include "raygui.h"
#include "redball_icon.h"

#include "sounds/shoot_embed.h"
#include "sounds/explosion_embed.h"
#include "sounds/horn_embed.h"
#include "sounds/thompson_hipodrom_horn_plus_embed.h"
#include "images/background_embed.h"

#include <vector>
#include <algorithm>
#include <string>

struct Bullet
{
    Vector2 pos;
    Vector2 vel;
};

struct Target
{
    Vector2 pos;
    Vector2 vel;
    float radius;
    bool alive;
};

struct Explosion
{
    Vector2 pos;
    float radius;
    float life;
};

struct Star {
    int x, y;
    int size;
    Color color;
};

Star stars[20];

int main()
{
	// ikona od exe priprema png za  redball.rc  https://www.icoconverter.com/ + dodati redball.ico u cmakelist

    const int screenWidth = 1800;
    const int screenHeight = 900;

    for (int i = 0; i < 20; i++)
    {
        stars[i].x = GetRandomValue(0, screenWidth);
        stars[i].y = GetRandomValue(0, screenHeight / 2);
        stars[i].size = GetRandomValue(1, 3);  // 1-3 piksela
        stars[i].color = Fade(WHITE, GetRandomValue(128, 255) / 255.0f);  // poluprozirne bijele
    }

    InitWindow(screenWidth, screenHeight, "Ray Red Raid - Shoot right !!!");

    InitAudioDevice();

    // Kako generirati header iz PNG-a (samo jednom generirati pa zakomentirati) :  
    // Image img = LoadImage("redball.png");
    // ExportImageAsCode(img, "redball_icon.h");  // generira header
    // UnloadImage(img);

	// kada se ima redball_icon.h onda ju ovako prikazati kao ikonu prozora: 
    Image icon = { 0 };  // ili Image icon = {0};
    icon.data = REDBALL_ICON_DATA;                     // pointer na tvoj niz
    icon.width = REDBALL_ICON_WIDTH;                    // 32
    icon.height = REDBALL_ICON_HEIGHT;                   // 32
    icon.mipmaps = 1;                                     // standardno za ikonu
    icon.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;     // RGBA 32-bit (isto što i tvoj #define 7)
    SetWindowIcon(icon);

    SetTargetFPS(60);
    Texture2D backgroundTexture;
    bool backgroundLoaded = false;

    Image bgImage = LoadImageFromMemory(".png", background_png, (int)background_png_len);
    Texture2D bgTexture = LoadTextureFromImage(bgImage);
    UnloadImage(bgImage);  // oslobodi Image, zadrži Texture
    backgroundTexture = bgTexture;
    backgroundLoaded = true;

 /*   Sound shootSound;
    Sound explosionSound;
    shootSound = LoadSound("shoot.wav");
    explosionSound = LoadSound("explosion.wav");*/
    Wave shootWave = LoadWaveFromMemory(".wav", shoot_wav, (int)shoot_wav_len);
    Sound shootSound = LoadSoundFromWave(shootWave);
    Wave explosionWave = LoadWaveFromMemory(".wav", explosion_wav, (int)explosion_wav_len);
    Sound explosionSound = LoadSoundFromWave(explosionWave);
    Wave hornWave = LoadWaveFromMemory(".mp3", horn_mp3, (int)horn_mp3_len);
    Sound hornSound = LoadSoundFromWave(hornWave);
    Wave mptWave = LoadWaveFromMemory(".mp3", thompson_hipodrom_horn_plus_mp3, (int)thompson_hipodrom_horn_plus_mp3_len);
    Sound mptSound = LoadSoundFromWave(mptWave);

  
    SetSoundVolume(shootSound, 0.2f);
    SetSoundVolume(explosionSound, 0.7f);
    SetSoundVolume(mptSound, 0.7f);
    SetSoundVolume(hornSound, 0.9f);
    PlaySound(mptSound);
    // PlaySound(hornSound);

    // PLAYER
    Vector2 player = { 150, screenHeight / 2.0f };
    float playerSpeed = 600.0f;

  

    // GAME STATE
    std::vector<Bullet> bullets;
    std::vector<Target> targets;
    std::vector<Explosion> explosions;

    float bulletSpeed = 1200.0f;
    int bulletsFired = 0;
    int score = 0;
    int nukeCount = 3;          
    bool nukeUsedThisFrame = false;

    float gameTime = 0.0f;
    const float GAME_DURATION = 30.0f;
    bool gameOver = false;

    // Spawn initial targets
    for (int i = 0; i < 5; i++)
    {
        targets.push_back({
            { (float)screenWidth + 100, (float)GetRandomValue(50, screenHeight - 50) },
            { -200.0f - (float)GetRandomValue(0, 50), 0 },
            (float)GetRandomValue(25, 60),
            true
            });
    }

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // ──────────────────────────────────────────────────────────────
        // UPDATE LOGIC
        // ──────────────────────────────────────────────────────────────

        if (!gameOver)
        {
            gameTime += dt;

            if (gameTime >= GAME_DURATION)
            {
                gameOver = true;
            }

            // ── PLAYER MOVEMENT ────────────────────────────────────────
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))   player.x -= playerSpeed * dt;
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))  player.x += playerSpeed * dt;
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))     player.y -= playerSpeed * dt;
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))   player.y += playerSpeed * dt;

            player.x = Clamp(player.x, 30, screenWidth - 40);
            player.y = Clamp(player.y, 30, screenHeight - 40);

            // ── NUKE ALL TARGETS (N ključ) ────────────────────────────
            if (IsKeyPressed(KEY_N) && nukeCount > 0 && !nukeUsedThisFrame)
            {
                nukeUsedThisFrame = true;  // sprječava spam u istom frame-u

                // UBII SVE METE!
                for (auto& t : targets)
                {
                    if (t.alive)
                    {
                        t.alive = false;
                        score++;  // bonus po meti!
                        PlaySound(explosionSound);
                        explosions.push_back({ t.pos, 25, 0.8f });  // veća eksplozija
                    }
                }

                nukeCount--;  // potroši jedan nuke
            }
            else if (IsKeyReleased(KEY_N))
            {
                nukeUsedThisFrame = false;  // reset za sljedeći frame
            }

            // ── SHOOT ──────────────────────────────────────────────────
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE))
            {
                bullets.push_back({
                    player,
                    { bulletSpeed, 0 }
                    });
                bulletsFired++;
                PlaySound(shootSound);
            }

            // ── UPDATE BULLETS ─────────────────────────────────────────
            for (auto& b : bullets)
            {
                b.pos.x += b.vel.x * dt;
            }

            // ── UPDATE TARGETS ─────────────────────────────────────────
            for (auto& t : targets)
            {
                if (t.alive)
                {
                    t.pos.x += t.vel.x * dt;
                    t.pos.y += t.vel.y * dt;
                }
            }

            // ── COLLISION ──────────────────────────────────────────────
            for (auto& b : bullets)
            {
                for (auto& t : targets)
                {
                    if (!t.alive) continue;

                    if (CheckCollisionCircles(b.pos, 8, t.pos, t.radius))
                    {
                        t.alive = false;
                        score++;
                        PlaySound(explosionSound);
                        explosions.push_back({ t.pos, 15, 0.5f });

                        b.pos.x = screenWidth + 200;
                        break;
                    }
                }
            }

            // ── RESPAWN TARGETS (pogođeni ILI izašli lijevo) ─────────────────
            for (auto& t : targets)
            {
                if (!t.alive || t.pos.x < -100)  // ← OVO JE NOVA LINIJA!
                {
                    // Respawn bilo kog mrtvog ili izašlog targeta
                    t.pos = { (float)screenWidth + 100, (float)GetRandomValue(50, screenHeight - 50) };
                    t.vel = { -250.0f - (float)GetRandomValue(0, 620), 0 };
                    t.radius = (float)GetRandomValue(25, 90);
                    t.alive = true;
                }
            }

            // Ukloni samo one koji su totalno nestali (rijetko se događa)
            targets.erase(
                std::remove_if(targets.begin(), targets.end(),
                    [](const Target& t) { return t.pos.x < -300; }),  // ← veći threshold
                targets.end()
            );

            // ── CLEAN UP ───────────────────────────────────────────────
            bullets.erase(
                std::remove_if(bullets.begin(), bullets.end(),
                    [&](const Bullet& b) { return b.pos.x > screenWidth + 50; }),
                bullets.end());

            targets.erase(
                std::remove_if(targets.begin(), targets.end(),
                    [](const Target& t) { return t.pos.x < -100; }),
                targets.end());

            explosions.erase(
                std::remove_if(explosions.begin(), explosions.end(),
                    [](const Explosion& e) { return e.life <= 0; }),
                explosions.end());

            for (auto& e : explosions)
            {
                e.radius += 300 * dt;
                e.life -= dt;
            }

		

        }

        // ──────────────────────────────────────────────────────────────
        // DRAW
        // ──────────────────────────────────────────────────────────────

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(
            backgroundTexture,           // Texture
            Rectangle { 0, 0, (float)backgroundTexture.width, (float)backgroundTexture.height },   // source rect (cijela slika)
            Rectangle { 0, 0, (float)screenWidth, (float)screenHeight }, // destination rect (cijeli ekran)
            Vector2 { 0, 0 },                 // origin (0,0 za pun ekran)
            0.0f,                            // rotation
            WHITE                            // tint (WHITE = bez promjene boje)
        );

        // ── STARFIELD BACKGROUND ─────────────────────────────────────
        for (int i = 0; i < 20; i++)
        {
            // Treperenje (opciono)
            float blink = sin(GetTime() * 2 + i) * 0.5f + 0.5f;
            Color starColor = Fade(stars[i].color, blink);

            for (int s = 0; s < stars[i].size; s++)
            {
                DrawPixel(stars[i].x + s, stars[i].y, starColor);
            }
        }

        if (!gameOver)
        {
            // Player
            DrawCircleV(player, 25, BLUE);

            // Bullets
            for (const auto& b : bullets)
                DrawCircleV(b.pos, 6, ORANGE);

            // Targets
            for (const auto& t : targets)
                if (t.alive)
                    DrawCircleV(t.pos, t.radius, RED);

            // Explosions
            for (const auto& e : explosions)
                DrawCircleLinesV(e.pos, e.radius, ORANGE);
        }

        // UI - tijekom igre
        DrawText(TextFormat("Score: %d", score), 20, 20, 28, DARKGREEN);
        DrawText(TextFormat("Time: %.1f", GAME_DURATION - gameTime), 20, 60, 28, DARKGREEN);
        DrawText(TextFormat("Bullets: %d", bulletsFired), 20, 100, 28, DARKGREEN);
        DrawText(TextFormat("Nukes: %d (N)", nukeCount), 20, 140, 28,nukeCount > 0 ? DARKGREEN : GRAY);

        if (gameOver)
        {
            //DrawRectangle(0, 0, screenWidth, screenHeight, Color { 0, 0, 0, 180 });

            const char* msg = TextFormat("GAME OVER - YOU WIN !!! \n\nYour score: %d\n\nPress Enter to play again", score);
            int fontSize = 32;
            Vector2 size = MeasureTextEx(GetFontDefault(), msg, fontSize, 4);

            DrawText(msg,
                (screenWidth - size.x) / 2,
                (screenHeight - size.y) / 4,
                fontSize, BLUE);
        }

        DrawFPS(10, screenHeight - 30);

        EndDrawing();

        // ── RESTART ON CLICK ───────────────────────────────────────
        if (gameOver && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER)))
        {

            //PlaySound(hornSound);
			PlaySound(mptSound);

            gameTime = 0.0f;
            gameOver = false;
            score = 0;
            bulletsFired = 0;
            nukeCount = 3;          
            nukeUsedThisFrame = false;  

            bullets.clear();
            explosions.clear();
            targets.clear();

            // ponovno spawn početnih meta
            for (int i = 0; i < 5; i++)
            {
                targets.push_back({
                    { (float)screenWidth + 100, (float)GetRandomValue(50, screenHeight - 50) },
                    { -200.0f - (float)GetRandomValue(0, 50), 0 },
                    (float)GetRandomValue(25, 40),
                    true
                    });
            }

            
        }
    }
    UnloadTexture(backgroundTexture);
    UnloadSound(shootSound);
    UnloadSound(explosionSound);
    UnloadSound(hornSound);
	UnloadSound(mptSound);
    UnloadWave(hornWave);
    UnloadWave(shootWave);       
    UnloadWave(explosionWave);
	UnloadWave(mptWave);
    CloseWindow();
    return 0;
}
 