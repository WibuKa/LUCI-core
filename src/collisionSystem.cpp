#include "collisionSystem.h"
#include <iostream>
#include <vector>

CollisionSystem collisionSystem;


struct OverlapIDs
{
    int a;
    int b;
};

std::vector<OverlapIDs> overlaps;
std::vector<Shape> shapes2D;

void CollisionSystem::addCollision2D(int ID,int shapeType,float x,float y ,int w,int h){
    shapes2D.push_back({ID,shapeType,x,y,w,h});
}

void CollisionSystem::_update_(){
    overlaps.clear();
    for (const Shape& shape_a:shapes2D){
        for(Shape shape_b:shapes2D){
            if (shape_a.ID == shape_b.ID){continue;}
            if (shape_a.x + shape_a.w < shape_b.x - shape_b.w||
                shape_b.x - shape_a.w > shape_b.x + shape_b.w||
                shape_a.y + shape_a.h < shape_b.y - shape_b.h||
                shape_b.y - shape_a.h > shape_b.y + shape_b.h)
            {
                continue;
            }else
            {
                overlaps.push_back({shape_a.ID,shape_b.ID});
            }
        }
    }
}

