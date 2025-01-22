// #include "../raylib/include/raylib.h"
#include "raylib.h"
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define RADIUS(r) r
#define EXIT_LOOP break;
#define CORD(cx, cy)                                                           \
  (Vector2) { .x = (cx), .y = (cy) }
#define SIZE(x, y) (x), (y)
#define TITLE(x) x
#define SetGameFPS(x) SetTargetFPS((x));
#define WindowNotClosed() !WindowShouldClose()
#define BG_COLOR (Color){0x18, 0x18, 0x18}
#define MAX_OBJECTS 100
char *get_score(bool);

typedef enum ObjectType {
  BALL = 0,
  BOX = 1,
  LINE = 2,
} ObjectType;

typedef enum ScoreType{
  CURRENT_SCORE = 0,
  HIGHEST_SCORE
}ScoreType;

typedef enum Controller {
  CNTLR_NONE = 0,
  CNTLR_JUMPER = 1,
  CNTLR_2AXIS = 2,
} Controller;

typedef struct Circle {
  Vector2 pos;
  int radius;
} Circle;

typedef struct _Rectangle {
  Vector2 pos;
  int width;
  int height;
} _Rectangle, Box;

typedef struct _Line {
  Vector2 pos;
  Vector2 pos2;
} _Line;

typedef enum MarqueeDirection {
  MRQ_NONE = 0,

  LEFT_TO_RIGHT,
  LEFT_TO_RIGHT_WRAP,

  RIGHT_TO_LEFT,
  RIGHT_TO_LEFT_WRAP,

  TOP_TO_BOTTOM,
  TOP_TO_BOTTOM_WRAP,

  BOTTOM_TO_TOP,
  BOTTOM_TO_TOP_WRAP,

} MarqueeDirection;

typedef struct Object {
  ObjectType type;
  union {
    Circle shape;
    _Rectangle rec;
    _Line line;
  };
  Color color;
  Vector2 velocity;
  Controller controller;
  MarqueeDirection marquee;
  int marquee_speed;
  int follows_gravity;
  int is_opaque;
} Object;

typedef struct Objects {
  Object items[MAX_OBJECTS];
  int count;
} Objects;

typedef struct Physics {
  int walls;
  float wall_damping;
  float jump_strength;
  float move;

} Physics;

typedef enum {
  POINT = 1,
  WING,
  DIE,
  SWOOSH,
  MUSIC_COUNT,
} MUSICS;

static Objects objects = {.count = 0};
static Physics physics_config = {0, .wall_damping = 0.5714278,
                                 .jump_strength = 6.9, .move = 1.5};
static int game_over = 0;
static int score = 0;
static char score_str[20];
static int is_audio = 0;
#define AddSounds() is_audio = 1;
Sound sounds[MUSIC_COUNT] = {};
static int game_over_audio_played = 0;

void CreateWindow(int width, int height, char *title) {
  InitWindow(width, height, title);
  InitAudioDevice();
  sounds[POINT] = LoadSound("assets/point.wav");
  sounds[WING] = LoadSound("assets/wing.wav");
  sounds[DIE] = LoadSound("assets/hit.wav");
  sounds[SWOOSH] = LoadSound("assets/swoosh.wav");
  SetTargetFPS(60);
}
int AddBall(Vector2 pos, int radius, Color color) {
  int index = objects.count++;
  objects.items[index].type = BALL;
  objects.items[index].shape.pos = pos;
  objects.items[index].shape.radius = radius;
  objects.items[index].color = color;
  objects.items[index].velocity = (Vector2){.x = 0, .y = 0};
  return index;
}

void RenderObjects() {
  for (int i = 0; i < objects.count; i++) {
    Object obj = objects.items[i];
    int x = obj.shape.pos.x;
    int y = obj.shape.pos.y;
    Color color = obj.color;
    switch (obj.type) {
    case BALL: {
      DrawCircle(x, y, obj.shape.radius, color);
      break;
    }
    case BOX: {
      DrawRectangle(x, y, obj.rec.width, obj.rec.height, color);
      break;
    }
    case LINE: {
      DrawLine(x, y, obj.line.pos2.x, obj.line.pos2.y, color);
      break;
    }
    }
  }
}
#define GRAVITY 0.4
#define ObjX(index) objects.items[(index)].shape.pos.x
#define ObjY(index) objects.items[(index)].shape.pos.y
#define ObjCol(index) objects.items[(index)].color
#define ObjRadius(index) objects.items[(index)].shape.radius

void play_sound(MUSICS sound) {
  if (is_audio) {
    PlaySound(sounds[sound]);
  }
}

void handel_controllers(Object *object) {
  switch (object->controller) {
  case CNTLR_NONE:
    break;
  case CNTLR_JUMPER: {
    if (IsKeyPressed(KEY_SPACE)) {
      object->velocity.y = -physics_config.jump_strength;
      play_sound(WING);
    }
    break;
  }
  case CNTLR_2AXIS: {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
      object->velocity.y -= physics_config.move;
      play_sound(SWOOSH);
    }

    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
      object->velocity.y += physics_config.move;
      play_sound(SWOOSH);
    }

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
      object->velocity.x -= physics_config.move;
      play_sound(SWOOSH);
    }

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
      object->velocity.x += physics_config.move;
      play_sound(SWOOSH);
    }

    if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_LEFT) &&
        !IsKeyDown(KEY_RIGHT)) {
      object->velocity.x *= 0.9f;
      play_sound(SWOOSH);
    }
    if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_UP) &&
        !IsKeyDown(KEY_DOWN)) {
      object->velocity.y *= 0.9f;
      play_sound(SWOOSH);
    }
    break;
  }
  }
}

int get_object_width(Object object) {
  switch (object.type) {
  case BALL: {
    return object.shape.radius;
  }
  case BOX:
    return object.rec.width;
  case LINE: {
    int width = fabs(object.line.pos.x - object.line.pos2.x) +
                fabs(object.line.pos.y - object.line.pos2.y);
    return width;
  }
  }
}

int get_object_height(Object object) {
  switch (object.type) {
  case BALL: {
    return object.shape.radius;
  }
  case BOX:
    return object.rec.height;
  case LINE: {
    return 1;
  }
  }
}

void handel_marquee(Object *object) {

  int width = get_object_width(*object);
  int height = get_object_height(*object);
  int marquee_speed = object->marquee_speed + (int)(score / 10);
  switch (object->marquee) {
  case MRQ_NONE:
    return;
  case LEFT_TO_RIGHT:
    object->shape.pos.x += physics_config.move;
    if (object->shape.pos.x > GetScreenWidth() - width) {
      object->shape.pos.x = GetScreenWidth() - width; // Stop at the right edge
    }
    break;

  case LEFT_TO_RIGHT_WRAP:
    object->shape.pos.x += marquee_speed;
    if (object->shape.pos.x > GetScreenWidth()) {
      object->shape.pos.x = -width; // Wrap to the left
    }
    break;

  case RIGHT_TO_LEFT:
    object->shape.pos.x -= marquee_speed;
    if (object->shape.pos.x < 0) {
      object->shape.pos.x = 0;
    }
    break;

  case RIGHT_TO_LEFT_WRAP:
    object->shape.pos.x -= marquee_speed;
    if (object->shape.pos.x < -width) {
      object->shape.pos.x = GetScreenWidth(); // Wrap to the right
      score++;
      play_sound(POINT);
      object->color = GREEN;
    }
    break;

  case TOP_TO_BOTTOM:
    object->shape.pos.y += marquee_speed;
    if (object->shape.pos.y > GetScreenHeight() - height) {
      object->shape.pos.y = GetScreenHeight() - height; // Stop at the bottom
    }
    break;

  case TOP_TO_BOTTOM_WRAP:
    object->shape.pos.y += marquee_speed;
    if (object->shape.pos.y > GetScreenHeight()) {
      object->shape.pos.y = -height; // Wrap to the top
    }
    break;

  case BOTTOM_TO_TOP:
    object->shape.pos.y -= marquee_speed;
    if (object->shape.pos.y < 0) {
      object->shape.pos.y = 0; // Stop at the top edge
    }
    break;

  case BOTTOM_TO_TOP_WRAP:
    object->shape.pos.y -= marquee_speed;
    if (object->shape.pos.y < -height) {
      object->shape.pos.y = GetScreenHeight(); // Wrap to the bottom
    }
    break;

  default:
    break;
  }
}

void ApplyPhysics() {
  if (game_over) {
    if (!game_over_audio_played) {
      game_over_audio_played = 1;
      play_sound(DIE);
    }
    return;
  }
  for (int i = 0; i < objects.count; i++) {
    ObjX(i) += objects.items[i].velocity.x;
    ObjY(i) += objects.items[i].velocity.y;

    if (objects.items[i].follows_gravity) {
      objects.items[i].velocity.y += GRAVITY;
    }
    if (physics_config.walls) {
      int radius = get_object_width(objects.items[i]);
      if (ObjX(i) <= radius || ObjX(i) >= GetScreenWidth() - radius) {
        objects.items[i].velocity.x *= -1;
        objects.items[i].velocity.x *= (physics_config.wall_damping);
      }
      if (ObjY(i) <= radius || ObjY(i) >= GetScreenHeight() - radius) {
        objects.items[i].velocity.y *= -1;
        objects.items[i].velocity.y *= (physics_config.wall_damping);
      }
    }
    handel_controllers(&objects.items[i]);
    handel_marquee(&objects.items[i]);
  }
}

#define StartDrawing()                                                         \
  BeginDrawing();                                                              \
  ApplyPhysics();                                                              \
  RenderObjects();
#define AddGravity(object_id) objects.items[object_id].follows_gravity = 1;
#define AddCollision(object_id) objects.items[object_id].is_opaque = 1;
#define AddWalls() physics_config.walls = 1;
#define AddControlls(object_id, control_type)                                  \
  objects.items[object_id].controller = control_type;

int AddMovingBox(Box rec, MarqueeDirection dircetion, Color color) {
  int index = objects.count++;
  objects.items[index].type = BOX;
  objects.items[index].rec = rec;
  objects.items[index].velocity = (Vector2){.x = 0, .y = 0};
  objects.items[index].color = color;
  objects.items[index].marquee = dircetion;
  objects.items[index].marquee_speed = 5;
  return index;
}

int CheckCollisions() {
  for (int i = 0; i < objects.count; i++) {
    if (!objects.items[i].is_opaque)
      continue;
    for (int j = 0; j < objects.count; j++) {
      if (objects.items[i].type == BALL && objects.items[j].type == BOX) {
        if (CheckCollisionCircleRec(objects.items[i].shape.pos,
                                    objects.items[i].shape.radius,
                                    (Rectangle){
                                        .x = objects.items[j].rec.pos.x,
                                        .y = objects.items[j].rec.pos.y,
                                        .height = objects.items[j].rec.height,
                                        .width = objects.items[j].rec.width,
                                    })) {
          objects.items[j].color = BLUE;
          return 1;
        }
      }
    }
  }
  return 0;
}

typedef enum {
  CENTER = 0,

  TOP_LEFT,
  TOP_RIGHT,

  BOTTOM_LEFT,
  BOTTOM_RIGHT,

} TEXT_POS;

void PutText(char *text, TEXT_POS pos, Color color) {
  int x, y;
  switch (pos) {

  case CENTER: {
    x = GetScreenWidth() / 2;
    y = GetScreenHeight() / 2;
    x -= MeasureText(text, 24) / 2;
    break;
  }
  case TOP_LEFT: {
    x = 0;
    y = 10;
    x += MeasureText(text, 24) * 0.1;
    break;
  }
  case TOP_RIGHT: {
    x = GetScreenWidth() - 10;
    y = 0;
    x -= MeasureText(text, 24) * 1.2;
    break;
  }
  case BOTTOM_LEFT: {
    x = 0;
    y = GetScreenHeight() - 24;
    x += MeasureText(text, 24) * 0.2;
    break;
  }
  case BOTTOM_RIGHT: {
    x = GetScreenWidth() - 10;
    y = GetScreenHeight() - 24;
    x -= MeasureText(text, 24) * 1.2;
    break;
  } break;
  }
  // void *fnt = LoadFont("");
  // DrawTextPro(fnt,text, , Vector2 origin, float rotation, float fontSize,
  // float spacing, Color tint)
  DrawText(text, x, y, 24, color);
}

void GenerateBoxes(int count, float minGap, float maxGap) {
  srand(time(NULL));
  float boxWidth = 100;
  float minHeight = 50;
  float maxHeight = GetScreenHeight() - minGap - minHeight;
  float xOffset = GetScreenWidth();
  float gap = 0;
  for (int i = 0; i < count; i++) {
    gap = minGap + rand() % (int)(maxGap - minGap + 1);
    float topHeight = minHeight + rand() % (int)(maxHeight - minHeight + 1);
    float bottomHeight = GetScreenHeight() - topHeight - gap;
    AddMovingBox(
        (Box){
            .pos = {.x = xOffset, .y = 0},
            .height = topHeight,
            .width = boxWidth,
        },
        RIGHT_TO_LEFT_WRAP, GREEN);

    AddMovingBox(
        (Box){
            .pos = {.x = xOffset, .y = GetScreenHeight() - bottomHeight},
            .height = bottomHeight,
            .width = boxWidth,
        },
        RIGHT_TO_LEFT_WRAP, GREEN);
    xOffset += boxWidth + 200;
  }
}

int get_highest_score() {
  FILE *read_file = fopen("highest_score.txt", "r");
  int highest_score = 0;
  if (read_file != NULL) {
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), read_file)) {
      sscanf(buffer, "Current Score : %d", &highest_score);
    }
    fclose(read_file);
  }
  if (score > highest_score) {
    FILE *write_file = fopen("highest_score.txt", "w");
    highest_score = score;
    if (write_file != NULL) {
      fprintf(write_file, "%s", get_score(false));
      fclose(write_file);
    }
  }
  return highest_score;
}

char *get_score(bool highest) {
  if (highest) {
    sprintf(score_str, "Highest Score : %d", get_highest_score());
  } else {
    sprintf(score_str, "Current Score : %d", score);
  }
  return score_str;
}
