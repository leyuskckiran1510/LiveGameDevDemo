#include "engine.c"
#include <raylib.h>

int main() {
  CreateWindow(SIZE(800, 400), "Flappy Bird");
  int ball_id = AddBall(CORD(100, 140), 10, RED);
  AddGravity(ball_id);
  AddWalls();
  AddCollision(ball_id);
  GenerateBoxes(3, 120, 180);
  AddControlls(ball_id, CNTLR_JUMPER);
  while (WindowNotClosed()) {
    StartDrawing();
    ClearBackground(WHITE);
    if (CheckCollisions()) {
      GAME_OVER();
      PutText("GAME OVER", CENTER, RED);
    }

    PutText(get_score(CURRENT_SCORE), TOP_RIGHT, BLACK);
    PutText(get_score(HIGHEST_SCORE), TOP_LEFT, BLACK);
    EndDrawing();
  }
  return 0;
}