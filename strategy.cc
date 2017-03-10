#include "strategy.h"
#include <vector>



void Strategy::applyMove (const move& mv) {
        if(!((mv.ox-mv.nx==1 || mv.ox-mv.nx==-1)&&(mv.oy-mv.ny==1 || mv.oy-mv.ny==-1))){
          // Déplacement avec déplacement
          _blobs.set(mv.ox,mv.oy,-1);
        }
        _blobs.set(mv.nx,mv.ny,(int) _current_player);
}

Sint32 Strategy::estimateCurrentScore () const {
    Sint32 score = 0;
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
    int indice = 0;
    int curr = (int) _current_player;
    // On parcours l'ensemble de l'échiquier et on trouve tous les blobs du joueur courant
    for(int i = 0;i<8;i++){
      for(int j = 0 ; j<8;j++){
        if(_blobs.get(i,j)==curr){
          // une fois trouvés,les blobs, on ajoute tous ses moves valides.
          for(int k = std::max(0,i-2) ; k <= std::min(7,i+2) ; k++) {
              for(int l = std::max(0,j-2) ; l <= std::min(7,j+2) ; l++) {
                  if (_holes.get(k,l)) continue;
                  if(k==i && j==l) continue ;
                  if (_blobs.get(k,l) == -1) {
                    move mv(i,j,k,l);
                    valid_moves[indice]=mv;
                    indice ++;
                  }
            }
          }
        }
      }
    }
    return valid_moves;
}


void Strategy::computeBestMove () {
    move mv(0,0,0,0);
    std::vector<move> valid_moves(64,mv);
    computeValidMoves(valid_moves);
    // Estimation du meilleur coup :
    int max = this->estimateCurrentScore() -3;
    int curr_score = max;
    for(std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
      Strategy nextStrat (_blobs,_holes,_current_player,_saveBestMove);
      nextStrat.applyMove(*it);
      curr_score = nextStrat.estimateCurrentScore();
      if (curr_score > max){
        max = curr_score;
        mv = *it;
      }

    }
     _saveBestMove(mv);
     return;
}
/*/
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
}*/
