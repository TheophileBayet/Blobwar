#include "strategy.h"



void Strategy::applyMove (const move& mv) {
        if(!((mv.ox-mv.nx==1 || mv.ox-mv.nx==-1)&&(mv.oy-mv.ny==1 || mv.oy-mv.ny==-1))){
          // Déplacement avec déplacement
          _blobs(mv.ox,mv.oy)=-1;
        }
        _blobs(mv.nx,mv.ny)=(int) _current_player;
}

Sint32 Strategy::estimateCurrentScore () const {
    Sint32 score = 0
    for(Uint8 i = 0 ; i < 8 ; i++) {
		    for(Uint8 j = 0 ; j < 8 ; j++) {
			       if (! _holes.get(i, j)) { // verifier que holes vaut bien true s'il y a un trou
              if (_blobs.get(i,j)==0) score++;
              if (_blobs.get(i,j)==1) score --;
			}
		}
	}
    return score;
}

vector<move>& Strategy::computeValidMoves (vector<move>& valid_moves) const {
        // To be completed...
    return valid_moves;
}

void Strategy::computeBestMove () {
    // To be improved...

    //The following code find a valid move.
    move mv(0,0,0,0);
    //iterate on starting position
    for(mv.ox = 0 ; mv.ox < 8 ; mv.ox++) {
        for(mv.oy = 0 ; mv.oy < 8 ; mv.oy++) {
            if (_blobs.get(mv.ox, mv.oy) == (int) _current_player) {
                //iterate on possible destinations
                for(mv.nx = std::max(0,mv.ox-2) ; mv.nx <= std::min(7,mv.ox+2) ; mv.nx++) {
                    for(mv.ny = std::max(0,mv.oy-2) ; mv.ny <= std::min(7,mv.oy+2) ; mv.ny++) {
                        if (_holes.get(mv.nx, mv.ny)) continue;
                        if (_blobs.get(mv.nx, mv.ny) == -1) goto end_choice;
                    }
                }
            }
        }
    }

end_choice:
     _saveBestMove(mv);
     return;
}
