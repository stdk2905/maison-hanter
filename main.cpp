
#include "raylib.h"
#include "raymath.h"
#include <cmath>

int main() {
    InitWindow(1280, 720, "LA MAISON HANTEE");
    SetTargetFPS(60);

    enum Screen { MENU, OPTIONS, GAME };
    Screen screen = MENU;

    int selected = 0;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float sensitivity = 0.12f;
    bool flashlight = true;

    Camera3D camera = {};
    camera.position = {0, 1.7f, 17};
    camera.target = {0, 1.7f, 16};
    camera.up = {0, 1, 0};
    camera.fovy = 70;
    camera.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (screen == MENU) {
            if (IsKeyPressed(KEY_UP)) selected = (selected + 2) % 3;
            if (IsKeyPressed(KEY_DOWN)) selected = (selected + 1) % 3;

            if (IsKeyPressed(KEY_ENTER)) {
                if (selected == 0) {
                    screen = GAME;
                    DisableCursor();
                }
                if (selected == 1) screen = OPTIONS;
                if (selected == 2) break;
            }
        }

        if (screen == OPTIONS) {
            if (IsKeyPressed(KEY_LEFT))
                sensitivity = fmaxf(0.04f, sensitivity - 0.02f);
            if (IsKeyPressed(KEY_RIGHT))
                sensitivity = fminf(0.30f, sensitivity + 0.02f);
            if (IsKeyPressed(KEY_ESCAPE)) screen = MENU;
        }

        if (screen == GAME) {
            Vector2 mouse = GetMouseDelta();
            yaw += mouse.x * sensitivity;
            pitch = Clamp(pitch - mouse.y * sensitivity, -70, 70);

            Vector3 forward = {
                cosf(yaw * DEG2RAD), 0,
                sinf(yaw * DEG2RAD)
            };

            Vector3 right = {-forward.z, 0, forward.x};
            Vector3 movement = {0, 0, 0};

            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_Z))
                movement = Vector3Add(movement, forward);
            if (IsKeyDown(KEY_S))
                movement = Vector3Subtract(movement, forward);
            if (IsKeyDown(KEY_D))
                movement = Vector3Add(movement, right);
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_Q))
                movement = Vector3Subtract(movement, right);

            if (Vector3Length(movement) > 0.01f) {
                movement = Vector3Scale(
                    Vector3Normalize(movement),
                    (IsKeyDown(KEY_LEFT_SHIFT) ? 5.0f : 3.0f) * dt
                );
            }

            Vector3 next = Vector3Add(camera.position, movement);

            // Barrières et passage central
            next.x = Clamp(next.x, -14, 14);
            next.z = Clamp(next.z, -19, 21);

            if (next.z < 8 && fabsf(next.x) > 2)
                next.z = 8;

            // Mur de la maison
            if (next.z < 3 && fabsf(next.x) < 5.5f)
                next.z = 3;

            camera.position = {next.x, 1.7f, next.z};

            Vector3 look = {
                cosf(yaw * DEG2RAD) * cosf(pitch * DEG2RAD),
                sinf(pitch * DEG2RAD),
                sinf(yaw * DEG2RAD) * cosf(pitch * DEG2RAD)
            };

            camera.target = Vector3Add(camera.position, look);

            if (IsKeyPressed(KEY_F)) flashlight = !flashlight;

            if (IsKeyPressed(KEY_ESCAPE)) {
                screen = MENU;
                selected = 0;
                EnableCursor();
            }
        }

        BeginDrawing();
        ClearBackground({5, 7, 13, 255});

        if (screen == MENU || screen == OPTIONS) {
            Camera3D decor = camera;
            decor.position = {0, 5, 23};
            decor.target = {0, 3, 0};

            BeginMode3D(decor);

            DrawPlane({0, 0, 0}, {70, 70}, DARKGREEN);

            // Maison
            DrawCube({0, 3, 0}, 12, 6, 8, GRAY);
            DrawCube({0, 6.3f, 0}, 13, 1, 9, DARKGRAY);
            DrawCube({0, 1.5f, 4.1f}, 1.5f, 3, 0.2f, BLACK);

            // Fenêtres
            for (float x : {-4.0f, 4.0f})
                DrawCube({x, 3.5f, 4.1f}, 1.5f, 1.7f, 0.15f, BLUE);

            // Arbres
            for (int i = 0; i < 10; i++) {
                float x = (i % 2 ? 1 : -1) * (10 + i % 3);
                float z = -13 + i * 2.5f;
                DrawCylinder({x, 2, z}, 0.3f, 0.3f, 4, 8, BROWN);
                DrawSphere({x, 5, z}, 2, DARKGREEN);
            }

            EndMode3D();

            DrawRectangle(0, 0, 1280, 720, {0, 0, 0, 160});

            DrawText("LA MAISON", 75, 85, 65, RAYWHITE);
            DrawText("HANTEE", 80, 150, 45, RED);

            if (screen == MENU) {
                const char* items[] = {
                    "JOUER", "OPTIONS", "QUITTER"
                };

                for (int i = 0; i < 3; i++) {
                    Color c = selected == i ? RED : LIGHTGRAY;
                    DrawText(items[i], 100, 350 + i * 60, 32, c);
                    if (selected == i)
                        DrawText(">", 65, 350 + i * 60, 32, RED);
                }
            } else {
                DrawText("OPTIONS", 90, 350, 38, WHITE);
                DrawText(
                    TextFormat("Sensibilite : %.2f", sensitivity),
                    90, 420, 25, WHITE
                );
                DrawText("Fleches gauche/droite : modifier",
                         90, 465, 20, GRAY);
                DrawText("ECHAP : retour", 90, 530, 22, RED);
            }

        } else {
            BeginMode3D(camera);

            // Terrain
            DrawPlane({0, -0.1f, 0}, {80, 80},
                      {36, 48, 30, 255});

            // Chemin
            DrawCube({0, 0, 12}, 3.5f, 0.03f, 35,
                     {65, 53, 42, 255});

            // Maison hantée
            DrawCube({0, 3, 0}, 12, 6, 8,
                     {66, 57, 50, 255});
            DrawCube({0, 6.5f, 0}, 13, 1, 9, DARKGRAY);

            // Porte
            DrawCube({0, 1.5f, 4.1f}, 1.5f, 3, 0.2f,
                     {25, 16, 14, 255});

            // Fenêtres
            for (float x : {-4.0f, 4.0f}) {
                DrawCube({x, 3.5f, 4.1f}, 1.5f, 1.7f,
                         {12, 19, 28, 255});
            }

            // Arbres et feuillage
            for (int i = 0; i < 12; i++) {
                float x = (i % 2 ? 1 : -1) * (10 + i % 3);
                float z = -15 + i * 2.5f;

                DrawCylinder({x, 2, z}, 0.3f, 0.3f, 4,
                             8, BROWN);
                DrawSphere({x, 5, z}, 2, DARKGREEN);
            }

            // Herbe décorative
            for (int i = 0; i < 100; i++) {
                float x = (i * 17 % 55) - 27;
                float z = (i * 23 % 55) - 27;

                DrawLine3D(
                    {x, 0, z},
                    {x + 0.1f, 0.4f, z},
                    GREEN
                );
            }

            // Barrières
            for (int x = -14; x <= 14; x += 2) {
                DrawCube({(float)x, 1, 21},
                         0.15f, 2, 0.15f, BROWN);
                DrawCube({(float)x, 1, -19},
                         0.15f, 2, 0.15f, BROWN);

                if (x < -2 || x > 2)
                    DrawCube({(float)x, 1, 8},
                             0.15f, 2, 0.15f, BROWN);
            }

            for (int z = -19; z <= 21; z += 2) {
                DrawCube({-14, 1, (float)z},
                         0.15f, 2, 0.15f, BROWN);
                DrawCube({14, 1, (float)z},
                         0.15f, 2, 0.15f, BROWN);
            }

            // Lampe torche
            if (flashlight) {
                Vector3 direction = Vector3Normalize(
                    Vector3Subtract(camera.target, camera.position)
                );

                Vector3 end = Vector3Add(
                    camera.position,
                    Vector3Scale(direction, 9)
                );

                DrawCylinderEx(
                    camera.position, end,
                    0.1f, 1.4f, 16,
                    {255, 245, 190, 35}
                );
            }

            EndMode3D();

            // Nuit
            DrawRectangle(0, 0, 1280, 720,
                          {0, 0, 15, 105});

            DrawText("LA MAISON HANTEE", 20, 20, 22, WHITE);
            DrawText(flashlight ? "LAMPE : ON (F)" :
                     "LAMPE : OFF (F)", 20, 55, 18, YELLOW);

            DrawText("ZQSD : bouger | SHIFT : courir",
                     20, 650, 18, WHITE);
            DrawText("Souris : regarder | ESC : menu",
                     20, 675, 18, WHITE);

            DrawCircle(640, 360, 2, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
