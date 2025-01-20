#include "engine.c" 

int main(){
    CreateWindow(SIZE(800,480),TITLE("FLOPPY BIRD"));
    int ball_id = AddBall(CORD(100,100),20,GREEN);
    AddGravity(ball_id);
    AddControlls(ball_id,CNTLR_JUMPER);
    AddWalls();
    AddCollision(ball_id);
    GenerateBoxes(3,100,200);

    while(WindowNotClosed()){
        StartDrawing();
        ClearBackground(WHITE);
        EndDrawing();
        if (CheckCollisions()){
            game_over=1;
            PutText("GAME OVER",CENTER,RED);
        }
        PutText(get_score(),TOP_LEFT,YELLOW);
    }
    return 0;
}