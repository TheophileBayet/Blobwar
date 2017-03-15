#include "strategy.h"
#include <vector>



void Strategy::applyMove (const move& mv) {
        if(!((mv.ox-mv.nx==1 || mv.ox-mv.nx==-1)&&(mv.oy-mv.ny==1 || mv.oy-mv.ny==-1))){
          // Déplacement avec déplacement
          _blobs.set(mv.ox,mv.oy,-1);
        }
        _blobs.set(mv.nx,mv.ny,(int) _current_player);
        for(int x = std::max(0,mv.nx-1) ; x <= std::min(7,mv.nx+1) ; x++) {
              for(int y = std::max(0,mv.ny-1) ; y <= std::min(7,mv.ny+1) ; y++) {
                  if (_blobs.get(x,y)!=-1){
                    _blobs.set(x,y,(int) _current_player);
                  }
              }

        }
}

Sint32 Strategy::estimateCurrentScore () const {
  	Uint32 scores[4];
	scores[0] = 0;
	scores[1] = 0;
	scores[2] = 0;
	scores[3] = 0;
	for(Uint8 i = 0 ; i < 8 ; i++)
		for(Uint8 j = 0 ; j < 8 ; j++)
			if(_blobs.get(i, j) != -1) scores[_blobs.get(i, j)]++;
  //std::cout<<"player " << _current_player << " score " << scores[_current_player] << endl;
  return scores[_current_player];
}

vector<move>& Strategy::computeValidMoves (vector<move>& valid_moves) const {
    int indice = 0;
    int curr = (int) _current_player;
    // On parcours l'ensemble de l'échiquier et on trouve tous les blobs du joueur courant
    for(int i = 0;i<8;i++){
      for(int j = 0 ; j<8;j++){
        if(_blobs.get(i,j)==curr){
          //std::cout << " blob trouvé en (" << i << "," << j << ") " << std::endl;
          // une fois trouvés,les blobs, on ajoute tous ses moves valides.
          for(int k = std::max(0,i-2) ; k <= std::min(7,i+2) ; k++) {
              for(int l = std::max(0,j-2) ; l <= std::min(7,j+2) ; l++) {
                  if (_holes.get(k,l)) continue;
                  if(k==i && j==l) continue ;
                  if (_blobs.get(k,l) == -1) {
                    move mv(i,j,k,l);
                    valid_moves[indice]=mv;
                    //std::cout << " newwwwwwwwww valid move : " << k << " ; " << l << std::endl;
                    // aucun ( 0,0,0,0,) stocké
                    indice ++;
                  }
            }
          }
        }
      }
    }
    return valid_moves;
}

void Strategy::nextPlayer(Strategy strat){
  strat._current_player = 1-strat._current_player;
}

move& searchBestMoves(Strategy current_strat,vector<move>& valid_moves) {

  return valid_moves[0];
}

int Strategy::valMinMax (const move& mv, int curr_prof, int max_prof){
  Strategy nextStrat (_blobs,_holes,_current_player,_saveBestMove);
  nextStrat.applyMove(mv);
  if(curr_prof==max_prof){
    //std::cout <<" player :" << _current_player << " ; profondeur " << curr_prof <<
    //" score : " << nextStrat.estimateCurrentScore() << std::endl;
    return nextStrat.estimateCurrentScore();
  }
  // On va calculer l'ensemble des coups possibles pour l'adversaire.
  nextStrat.nextPlayer(nextStrat);
  std::vector<move> valid_moves(300,mv);
  nextStrat.computeValidMoves(valid_moves);
  // on cherche parmi tous les moves valides le meilleur.
  int max_score = -100000;
  int curr_score ;
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
    if ( it->ox == 0 && it->oy == 0 && it->nx == 0 && it->ny == 0 ){ goto end_func ; }
    curr_score = valMinMax(*it,curr_prof+1,max_prof);
    if(curr_score> max_score){
      max_score=curr_score;
    }
  }
  goto end_func ;

  end_func :
    return max_score;
}


move& Strategy::findMoveMinMax(move& mv, int profondeur){
  std::vector<move> valid_moves(300,mv);
  computeValidMoves(valid_moves);
  int best_score = this->estimateCurrentScore() - 5;
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
    int curr_val = valMinMax(*it,1,profondeur);
    //std::cout << " coucou";
    if(curr_val> best_score){
      best_score=curr_val;
      mv = *it;
      std::cout<< " score " << curr_val << endl;
      _saveBestMove(mv);
    }
  }
  return mv;
}

/*

void Strategy::computeBestMove () {
    move mv(0,0,0,0);

    std::cout<<"player " << _current_player << " with score " << estimateCurrentScore() << endl;
    findMoveMinMax(mv,1);
     return;
}

*/
// SECOND VERSION ( GLOUTONNE )

void Strategy::computeBestMove () {
    move mv(0,0,1,0);
    std::vector<move> valid_moves(300,mv);
    computeValidMoves(valid_moves);
    // Estimation du meilleur coup :
    int max = this->estimateCurrentScore() -1000;
    int curr_score = max;
    for(std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
      Strategy nextStrat (_blobs,_holes,_current_player,_saveBestMove);
      nextStrat.applyMove(*it);
      //std::cout << "in parcours de valid_moves" << endl;
      curr_score = nextStrat.estimateCurrentScore();
      if (curr_score > max){
        max = curr_score;
        mv = *it;
        std::cout << " meilleur move ! "<< it->nx << endl;
      }

    }
     _saveBestMove(mv);
     return;
}




// FIRST VERSION ( BASIC)
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
