#include "game.c"

int main(){
    CreateWindow(SIZE(800,480),TITLE("Floopy : GAME"));
    SetGameFPS(60);
    int object_id = AddBall((Vector2){.x=200,.y=240},10,RED);  
    AddControlls(object_id,CNTLR_JUMPER);
    AddGravity();
    AddWalls();
    while(WindowNotClosed()){
        StartDrawing();
        ClearBackground(BG_COLOR);
        EndDrawing();
    }
    CloseWindow();
}
