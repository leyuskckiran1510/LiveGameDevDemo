#include "engine.c"
#include <raylib.h>

int main() {
  CreateWindow(SIZE(1000, 600), "FlappyBird");
  int ball = AddBall(CORD(100, 100), 10, BLACK);
  AddGravity(ball);
  AddWalls();
  AddSounds();
  GenerateBoxes(3, 100, 160);
  AddCollision(ball);
  AddControlls(ball, CNTLR_JUMPER);
  while (WindowNotClosed()) {
    StartDrawing();
    ClearBackground(RED);
    if (CheckCollisions()) {
      game_over = 1;
      if (game_over) {
        PutText("GAME OVER", CENTER, PINK);
      }
    }
    PutText(get_score(CURRENT_SCORE), TOP_LEFT, WHITE);
    PutText(get_score(HIGHEST_SCORE), TOP_RIGHT, WHITE);
    EndDrawing();
  }
  return 1;
}