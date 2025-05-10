#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

int main()
{
    const int width = 640;
    const int height = 480;
    const int objectSize = 20;

    InitWindow(width, height, "Object Placer");
    SetTargetFPS(60);

    int chosenShape = 0;
    int chosenSize = objectSize;
    Color chosenColor = RED;
    char *options = "Square;Circle";
    char *optionTitles[2] = { "Square", "Circle" };
    Vector2 mouse2DPos = { 0 };
    Vector2 mousePosDiff = { 0 };
    Vector2 wheelScale = { 0 };
    Rectangle exampleObject = { 320, 240, chosenSize, chosenSize };

    RenderTexture cameraTexture = LoadRenderTexture(496, 436);

    Camera2D camera = { 0 };
    camera.target = (Vector2){ 320, 240 };
    camera.offset = (Vector2){ 320, 240 };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Rectangle renderRect = { 0.0f, 0.0f, (float)cameraTexture.texture.width, (float)-cameraTexture.texture.height};

    while (!WindowShouldClose()) {
        //mouse
        mouse2DPos = GetMousePosition();
        mouse2DPos = GetWorldToScreen2D(mouse2DPos, camera);

        //mouse middle button
        Vector2 mouseDelta = GetMouseDelta();
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            mouseDelta = Vector2Scale(mouseDelta, -1.0f/camera.zoom);
            camera.target = Vector2Add(camera.target, mouseDelta);

            mouse2DPos = Vector2Add(mouse2DPos, mouseDelta);
            mousePosDiff = Vector2Add(mousePosDiff, mouseDelta);
        }

        //mouse wheel zoom
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            camera.offset = GetMousePosition();
            camera.target = mouseWorldPos;

            float scale = 0.2f * wheel;
            //camera.zoom = Clamp(expf(logf(camera.zoom)+scale), 0.125f, 64.0f);
            camera.zoom = Clamp(camera.zoom + scale, 0.125f, 64.0f);
        }

        mouse2DPos = GetMousePosition();
        Vector2 resultVector = Vector2Subtract(mouse2DPos, (Vector2){ 138, 32 });
        resultVector = Vector2Add(resultVector, mousePosDiff);
        resultVector = Vector2Subtract(resultVector, (Vector2){ (float)chosenSize / 2, (float)chosenSize / 2 });
        exampleObject.x = resultVector.x;
        exampleObject.y = resultVector.y;

        exampleObject.height = chosenSize;
        exampleObject.width = chosenSize;

        if (chosenShape == 1) {
            //circle
            resultVector = Vector2Add(resultVector, (Vector2){ (float)objectSize / 2, (float)objectSize / 2 });
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginTextureMode(cameraTexture);
        ClearBackground(RAYWHITE);
        BeginMode2D(camera);

        rlPushMatrix();
        rlTranslatef(0, 25*50, 0);
        rlRotatef(90, 1, 0, 0);
        DrawGrid(100, 50);
        rlPopMatrix();

        switch (chosenShape) {
            case 0:
                DrawRectangleRec(exampleObject, chosenColor);
                break;
            case 1:
                DrawCircleV(resultVector, (float)chosenSize / 2, chosenColor);
                break;
        }

        EndMode2D();
        DrawText(TextFormat("Placing %s", optionTitles[chosenShape]), 8, 8, 16, MAROON);
        EndTextureMode();

        //controls
        GuiComboBox((Rectangle){ 8, 8, 120, 24 }, options, &chosenShape);
        GuiPanel((Rectangle){ 136, 8, 500, 464 }, "World View");
        GuiSpinner((Rectangle){ 8, 40, 120, 24 }, NULL, &chosenSize, 10, 50, NULL);
        GuiColorPicker((Rectangle){ 8, 72, 96, 96 }, NULL, &chosenColor);
        DrawRectangleRec((Rectangle){ 8, 176, 120, 24 }, chosenColor);

        //texture for camera
        DrawTextureRec(cameraTexture.texture, renderRect, (Vector2){ 138, 32 }, WHITE);

        EndDrawing();
    }

    UnloadRenderTexture(cameraTexture);

    CloseWindow();
}
