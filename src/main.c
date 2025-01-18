#include "engine.c"

int main(){
    CreateWindow(SIZE(800,480),TITLE("Floopy : GAME"));
    SetGameFPS(60);
    int object_id = AddBall((Vector2){.x=200,.y=240},10,RED);  
    AddControlls(object_id,CNTLR_JUMPER);
    AddCollision(object_id);
    AddGravity(object_id);
    AddWalls();
    GenerateBoxes(3,100,200);

    while(WindowNotClosed()){
        StartDrawing();
        if(CheckCollisions()){
            PutText("Game Over",CENTER,RED);
            game_over=1;
        }
        if(game_over && IsKeyPressed(KEY_ENTER)){
            break;
        }
        PutText(get_score(),TOP_LEFT,YELLOW);
        ClearBackground(BG_COLOR);
        EndDrawing();
    }
    CloseWindow();
}
