#pragma once

#define BOX_SHAPE 0

struct Shape
{
    int ID;
    int type = BOX_SHAPE;
    float x,y;
    int w,h;
};


class CollisionSystem
{
public:
    void addCollision2D(int ID,int shapeType,float x,float y ,int w,int h);
    void _update_();
};

extern CollisionSystem collisionSystem;