#ifndef __MOVE_H
#define __MOVE_H
#include<SDL.h>
#include<iostream>
#include<sstream>

/** Move class
 */
struct move
{
    move(){}

    move(const move & mv)
            :ox(mv.ox), oy(mv.oy), nx(mv.nx), ny(mv.ny) {}
    move(const Uint8 oldx, const Uint8 oldy, const Uint8 newx, const Uint8 newy)
            : ox(oldx),oy(oldy),nx(newx),ny(newy){}

    move& operator=(const move& mv)
        {ox=mv.ox;oy=mv.oy;nx=mv.nx;ny=mv.ny; return *this;}

    void display(){
          std::cout << " move("<<(int) ox << ","<< (int) oy << "," << (int) nx << "," << (int) ny <<")" << std::endl;}

    Uint8 ox;
    Uint8 oy;
    Uint8 nx;
    Uint8 ny;
};

#endif
