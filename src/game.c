#include "../raylib/include/raylib.h"

#define CreateWindow(...) InitWindow(__VA_ARGS__)
#define SIZE(x,y)  (x),(y)
#define TITLE(x) x
#define SetGameFPS(x) SetTargetFPS((x));
#define WindowNotClosed() !WindowShouldClose()
#define BG_COLOR (Color){0x18,0x18,0x18}
#define  MAX_OBJECTS 100

typedef enum ObjectType{
    BALL=0,
    BOX=1,
    LINE=2,
}ObjectType;


typedef enum Controller{
    CNTLR_NONE=0,
    CNTLR_JUMPER=1,
    CNTLR_2AXIS=2,
} Controller;

typedef struct Circle{
    Vector2 pos;
    int radius;
} Circle;

typedef struct _Rectangle{
    Vector2 pos;
    int width;
    int height;
} _Rectangle;

typedef struct _Line{
    Vector2 pos;
    Vector2 pos2;
} _Line;

typedef struct Object{
    ObjectType type;
    union{
        Circle shape;
        _Rectangle rec;
        _Line line;
    };
    Color color;
    Vector2 velocity;
    Controller controller;
} Object;


typedef struct Objects{
    Object items[MAX_OBJECTS];
    int count;
} Objects;

typedef struct Physics{
    int gravity;
    int walls;
    float wall_damping;
    float jump_strength;
    float move;

} Physics;

static Objects objects={.count=0};
static Physics physics_config = {0,.wall_damping=0.5714278,.jump_strength=6.9,.move=1.5};

int AddBall(Vector2 pos,int radius,Color color){
    int index = objects.count++;
    objects.items[index].type = BALL;
    objects.items[index].shape.pos = pos;
    objects.items[index].shape.radius = radius;
    objects.items[index].color = color;
    objects.items[index].velocity = (Vector2){.x=0,.y=0};
    return index;
}

void RenderObjects(){
    for(int i=0;i<objects.count;i++){
        Object obj = objects.items[i];
        int x=obj.shape.pos.x;
        int y=obj.shape.pos.y;
        Color color = obj.color;
        switch (obj.type) {
        case BALL:{
            DrawCircle(x,y,obj.shape.radius,color);
            break;
        }
        case BOX:{
            DrawRectangle(x,y,obj.rec.width, obj.rec.height, color);
            break;
        }
        case LINE:{
            DrawLine(x,y,obj.line.pos2.x,obj.line.pos2.y,color);
            break;
        }
        }
    }
}
#define GRAVITY 0.4
#define ObjX(index)  objects.items[(index)].shape.pos.x
#define ObjY(index)  objects.items[(index)].shape.pos.y
#define ObjCol(index)  objects.items[(index)].color
#define ObjRadius(index)  objects.items[(index)].shape.radius

void ApplyPhysics(){
    for(int i=0;i<objects.count;i++){
        ObjX(i) += objects.items[i].velocity.x;     
        ObjY(i) += objects.items[i].velocity.y;  

        if(physics_config.gravity){
            objects.items[i].velocity.y += GRAVITY; 
        }
        if(physics_config.walls){
            int radius = ObjRadius(i);
            if(ObjX(i)<=radius || ObjX(i)>=GetScreenWidth()-radius){
                    objects.items[i].velocity.x *=-1;
                    objects.items[i].velocity.x *=(physics_config.wall_damping);
            }
            if(ObjY(i)<=radius || ObjY(i)>=GetScreenHeight()-radius){
                objects.items[i].velocity.y *=-1;
                objects.items[i].velocity.y *=(physics_config.wall_damping);
            }
        }
        switch (objects.items[i].controller) {
            case CNTLR_NONE:break;
            case CNTLR_JUMPER:{
                if (IsKeyPressed(KEY_SPACE)) {
                    objects.items[i].velocity.y = -physics_config.jump_strength;
                }
                break;
            }
            case CNTLR_2AXIS:{
                if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
                    objects.items[i].velocity.y -= physics_config.move;
                }
                
                if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
                    objects.items[i].velocity.y += physics_config.move;
                }
                
                if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
                    objects.items[i].velocity.x -= physics_config.move;
                }
                
                if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
                    objects.items[i].velocity.x += physics_config.move;
                }

                if (!IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) && 
                    !IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
                    objects.items[i].velocity.x *= 0.9f;
                }
                if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S) && 
                    !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
                    objects.items[i].velocity.y *= 0.9f;
                }
                break;
            }
        }
    }
}

#define StartDrawing() BeginDrawing();ApplyPhysics();RenderObjects();
#define AddGravity() physics_config.gravity=1;
#define AddWalls()   physics_config.walls=1;
#define AddControlls(object_id,control_type) objects.items[object_id].controller=control_type;
