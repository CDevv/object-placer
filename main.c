#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

struct WorldObject {
    int shape; //0 - square, 1 - circle
    float size;
    Color color;
    Vector2 position;
};

typedef struct WorldObject WorldObject;

struct Node {
    struct Node *next;
    WorldObject obj;
};

typedef struct Node Node;

Node *head = NULL;

void addObject(WorldObject obj) {
    if (head == NULL) {
        head = (Node*)malloc(sizeof(Node));
        head->obj = obj;
        head->next = NULL;
        return;
    }

    Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    current->next = (Node*)malloc(sizeof(Node));
    current->next->obj = obj;
    current->next->next = NULL;
}

void drawObject(WorldObject obj) {
    switch (obj.shape) {
        case 0:
            DrawRectangleV(obj.position, (Vector2){ obj.size, obj.size }, obj.color);
            break;
        case 1:
            DrawCircleV(obj.position, (obj.size / 2), obj.color);
            break;
    }
}

void drawObjects() {
    Node *current = head;

    if (current != NULL) {
        drawObject(current->obj);

        while (current != NULL) {
            drawObject(current->obj);
            current = current->next;
        }
    }
}

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

    int lastShape = 0;
    float lastSize = chosenSize;
    Color lastColor = chosenColor;
    Vector2 lastPos = (Vector2){ 0 };

    WorldObject lastObject = (WorldObject){ 0 };

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

        //mouse click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            WorldObject newObject;

            newObject.shape = chosenShape;
            newObject.size = chosenSize;
            newObject.color = chosenColor;
            newObject.position = resultVector;

            addObject(newObject);
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

        //draw the objects
        drawObjects();

        //draw a shape on mouse pos
        Color mouseShapeColor = (Color){ chosenColor.r, chosenColor.g, chosenColor.b, 256 / 2 };
        switch (chosenShape) {
            case 0:
                DrawRectangleV(resultVector, (Vector2){ chosenSize, chosenSize }, mouseShapeColor);
                break;
            case 1:
                DrawCircleV(resultVector, (float)chosenSize / 2, mouseShapeColor);
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
