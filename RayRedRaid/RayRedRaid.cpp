#define RAYGUI_IMPLEMENTATION

#include <sw/redis++/redis++.h>
#include "raylib.h"
#include "raymath.h"
#include "raygui.h"
#include "redball_icon.h"

#include "sounds/shoot_embed.h"
#include "sounds/explosion_embed.h"
#include "sounds/horn_embed.h"
#include "sounds/thompson_hipodrom_horn_plus_embed.h"
#include "images/background_embed.h"

#include <vector>
#include <algorithm>
#include <string>
#include <utility>
#include <thread>
#include <iostream>

using namespace std;

// Redis leaderboard utilities ------------------------------------------------

// add/update a player's score in the sorted set
static void update_leaderboard(sw::redis::Redis &redis, const string &username, int score)
{
    try {
        // ZADD defaults to update score if member exists
        redis.zadd("leaderboard", username, score);
    }
    catch (const sw::redis::Error &e) {
        cerr << "Redis error (update leaderboard): " << e.what() << endl;
    }
}

// fetch top N players (name, score) from sorted set
static vector<pair<string,int>> get_top_players(sw::redis::Redis &redis, int n)
{
    vector<pair<string,int>> out;
    try {
        vector<string> names;
        redis.zrevrange("leaderboard", 0, n - 1, back_inserter(names));
        for (auto &name : names) {
            auto score_opt = redis.zscore("leaderboard", name);
            if (score_opt) {
                out.emplace_back(name, (int)*score_opt);
            } else {
                out.emplace_back(name, 0);
            }
        }
    }
    catch (const sw::redis::Error &e) {
        cerr << "Redis error (get top players): " << e.what() << endl;
    }
    return out;
}

// ---------------------------------------------------------------------------

void listen_to_redis(string host, int port, string password) {
	try {
		sw::redis::ConnectionOptions opts;
		opts.host = host;
		opts.port = port;
		opts.password = password;

		auto redis_sub = sw::redis::Redis(opts);
		auto sub = redis_sub.subscriber();

		// Što napraviti kad stigne signal/poruka
		sub.on_message([](string channel, string msg) {
			//cout << "\n[REDIS SIGNAL]  channel " << channel << ": " << msg << endl;
			cout << "\n" << msg << " [" << "]"  << endl;
			cout << "Nastavite s radom (izbor): \n" << flush;
			});

		sub.subscribe("ps_osvjezavanje"); // Slušamo kanal za signale

		cout << "[REDIS] listening on chanell 'ps_osvjezavanje'..." << endl;

		while (true) {
			sub.consume(); // Čeka poruku bez blokiranja glavnog threada
		}
	}
	catch (const sw::redis::Error& e) {
		cerr << "Subscriber greška: " << e.what() << endl;
	}
}

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
    	sw::redis::ConnectionOptions opts;
	opts.host = "redis-17353.c16.us-east-1-3.ec2.cloud.redislabs.com";
	opts.port = 17353;
	opts.user = "default";
	opts.password = "5vKxk0eOcekOOWz5Q58S0BVMOGutHo3j";
	opts.connect_timeout = std::chrono::milliseconds(5000);
	
	// Kreiramo Redis objekt
	auto redis = sw::redis::Redis(opts);

	// Pokrećemo nit za slušanje Redis signala
	thread t1(listen_to_redis, opts.host, opts.port, opts.password);
	t1.detach(); // Odvajamo nit da radi neovisno o main-u

	// ikona od exe priprema png za  redball.rc  https://www.icoconverter.com/ + dodati redball.ico u cmakelist

    const int screenWidth = 1600;
    const int screenHeight = 1000;

    for (int i = 0; i < 20; i++)
    {
        stars[i].x = GetRandomValue(0, screenWidth);
        stars[i].y = GetRandomValue(0, screenHeight / 2);
        stars[i].size = GetRandomValue(1, 3);  // 1-3 piksela
        stars[i].color = Fade(WHITE, GetRandomValue(128, 255) / 255.0f);  // poluprozirne bijele
    }

    InitWindow(screenWidth, screenHeight, "Ray Red Raid - Shoot'em all 2!");

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

    bool scoreSubmitted = false;          // ensure we only push one score per round
    vector<pair<string,int>> topPlayers;  // cache of top players for start screen

    // START SCREEN
    bool gameStarted = false;
    char username[64] = { 0 };
    bool usernameEditMode = true;  // odmah aktivan za tipkanje

    // CHAT
    char chatMessage[256] = { 0 };
    bool chatEditMode = false;

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

        if (!gameOver && gameStarted)
        {
            gameTime += dt;

            if (gameTime >= GAME_DURATION)
            {
                gameOver = true;
                if (!scoreSubmitted && strlen(username) > 0)
                {
                    update_leaderboard(redis, string(username), score);
                    scoreSubmitted = true;
                }
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

        if (!gameStarted)
        {
            // ── START SCREEN ─────────────────────────────────────────
            int cx = screenWidth / 2;
            int cy = screenHeight / 2;

            // refresh leaderboard list (top 3)
            topPlayers = get_top_players(redis, 3);

            // Semi-transparent overlay
            int ow = 800, oh = 520;  // a little taller to fit leaderboard
            int ox = cx - ow / 2;
            int oy = cy - oh / 2 - 10;
            DrawRectangle(ox, oy, ow, oh, Color{ 0, 0, 0, 190 });
            DrawRectangleLinesEx({ (float)ox, (float)oy, (float)ow, (float)oh }, 2, Fade(RED, 0.7f));

            // Title
            const char* title = "RAY RED RAID";
            int titleSize = 68;
            DrawText(title, cx - MeasureText(title, titleSize) / 2, oy + 30, titleSize, RED);

            // draw leaderboard under title
            if (!topPlayers.empty())
            {
                const char* lbTitle = "Top Players:";
                int lbSize = 24;
                DrawText(lbTitle, cx - MeasureText(lbTitle, lbSize) / 2, oy + 110, lbSize, YELLOW);
                for (size_t i = 0; i < topPlayers.size(); ++i)
                {
                    string line = TextFormat("%zu. %s - %d", i + 1,
                                             topPlayers[i].first.c_str(),
                                             topPlayers[i].second);
                    DrawText(line.c_str(), cx - MeasureText(line.c_str(), 20) / 2,
                             oy + 140 + (int)i * 28, 20, LIGHTGRAY);
                }
            }

            // Subtitle
            const char* sub = "Enter your username to start:";
            DrawText(sub, cx - MeasureText(sub, 26) / 2, oy + 116, 26, LIGHTGRAY);

            // "Username:" label — centered, above textbox
            DrawText("Username:", cx - MeasureText("Username:", 26) / 2, oy + 168, 26, WHITE);

            // TextBox — centered below label
            GuiSetStyle(DEFAULT, TEXT_SIZE, 24);
            if (GuiTextBox({ (float)(cx - 200), (float)(oy + 204), 400, 52 }, username, 64, usernameEditMode))
                usernameEditMode = !usernameEditMode;

            // START GAME button — centered
            if (GuiButton({ (float)(cx - 130), (float)(oy + 278), 260, 56 }, "START GAME"))
                if (strlen(username) > 0) gameStarted = true;

            GuiSetStyle(DEFAULT, TEXT_SIZE, 10);

            // Hint
            const char* hint = "Click the textbox, type your name, then click START GAME";
            DrawText(hint, cx - MeasureText(hint, 18) / 2, oy + 360, 18, GRAY);
        }
        else
        {
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
            DrawText(TextFormat("Nukes: %d (N)", nukeCount), 20, 140, 28, nukeCount > 0 ? DARKGREEN : GRAY);
            DrawText(TextFormat("Player: %s", username), 20, 180, 24, YELLOW);

            if (gameOver)
            {
                //DrawRectangle(0, 0, screenWidth, screenHeight, Color { 0, 0, 0, 180 });

                const char* msg = TextFormat("GAME OVER - YOU WIN !!! \n\n%s - Score: %d\n\nPress Enter to play again", username, score);
                int fontSize = 32;
                Vector2 size = MeasureTextEx(GetFontDefault(), msg, fontSize, 4);

                DrawText(msg,
                    (screenWidth - size.x) / 2,
                    (screenHeight - size.y) / 4,
                    fontSize, BLUE);
            }

            // ── CHAT OVERLAY (bottom) ────────────────────────────────
            {
                int chatH = 62;
                int chatY = screenHeight - chatH;

                DrawRectangle(0, chatY, screenWidth, chatH, Color{ 0, 0, 0, 175 });
                DrawLine(0, chatY, screenWidth, chatY, Fade(DARKGRAY, 0.9f));

                GuiSetStyle(DEFAULT, TEXT_SIZE, 22);

                DrawText("Chat:", 14, chatY + (chatH - 22) / 2, 22, LIGHTGRAY);

                int tbX = 84;
                int tbW = screenWidth - 244;
                if (GuiTextBox({ (float)tbX, (float)(chatY + 10), (float)tbW, 42 }, chatMessage, 256, chatEditMode))
                    chatEditMode = !chatEditMode;

                if (GuiButton({ (float)(screenWidth - 155), (float)(chatY + 10), 140, 42 }, "SEND"))
                { /* TODO: send logic */ }

                GuiSetStyle(DEFAULT, TEXT_SIZE, 10);
            }
        }

        DrawFPS(screenWidth - 90, 10);

        EndDrawing();

        // ── RESTART ON CLICK ───────────────────────────────────────
        if (gameOver && gameStarted && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER)))
        {

            //PlaySound(hornSound);
			PlaySound(mptSound);

            gameTime = 0.0f;
            gameOver = false;
            score = 0;
            bulletsFired = 0;
            nukeCount = 3;
            nukeUsedThisFrame = false;
            scoreSubmitted = false;

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
