#include"engine.c"

int main(){
    CreateWindow(SIZE(800,400),"FlappyBird");
    int ball = AddBall(CORD(100,100),10,BLACK);
    AddGravity(ball);
    AddWalls();
    AddSounds();
    GenerateBoxes(3, 100,160);
    AddCollision(ball);
    AddControlls(ball,CNTLR_JUMPER);
    while(WindowNotClosed()){
        StartDrawing();
        ClearBackground(RED);
        if(CheckCollisions()){
            game_over=1;
            PutText("GAME OVER",CENTER , PINK);
        }
        PutText(get_score(),TOP_LEFT, WHITE);
        EndDrawing();


    }



    return 1;
}