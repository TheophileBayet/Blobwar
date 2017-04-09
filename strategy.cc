#include "strategy.h"
#include <vector>



void Strategy::applyMove (const move& mv) {
        if(mv.ox-mv.nx==2||mv.ox-mv.nx==-2||mv.oy-mv.ny==2||mv.oy-mv.ny==-2){
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
  return scores[_current_player];// TODO ; changer pour application plus générale
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
    while(indice < (int) valid_moves.size()){
      valid_moves[indice]=valid_moves[0];
      indice ++;
    }
    return valid_moves;
}

void Strategy::nextPlayer(Strategy strat){
  strat._current_player = 1-strat._current_player;
}

/*
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
*/


int Strategy::ennemi(move& mv, int curr_prof, int max_prof){
  Strategy nextStrat (_blobs,_holes,_current_player,_saveBestMove);
  nextStrat.applyMove(mv);
  int actual_score = nextStrat.estimateCurrentScore();
  if(curr_prof==max_prof){
    return actual_score;
  }
  nextStrat.nextPlayer(nextStrat);
  std::vector<move> valid_moves(300,mv);
  nextStrat.computeValidMoves(valid_moves);
  int eval = +1000;
  int best_score = eval;
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
    eval = actual_score-ami(*it,curr_prof+1,max_prof);
    if(eval < best_score){
      best_score = eval;
    }
  }
  return best_score;
}

int Strategy::ami(move& mv, int curr_prof, int max_prof){
  Strategy nextStrat (_blobs,_holes,_current_player,_saveBestMove);
  nextStrat.applyMove(mv);
  int actual_score = nextStrat.estimateCurrentScore();
  if(curr_prof==max_prof){
    return actual_score;
  }
  nextStrat.nextPlayer(nextStrat);
  std::vector<move> valid_moves(300,mv);
  nextStrat.computeValidMoves(valid_moves);
  int eval = -1000; //TODO : - infini
  int best_score = eval;
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
    eval = actual_score-ennemi(*it,curr_prof+1,max_prof);
    if(eval > best_score){
      best_score = eval;
    }
  }
  return best_score;
}
move& Strategy::findMoveMinMax(move& mv, int profondeur){
  /** En rester ici si ça ne remarche pas **/
  std::vector<move> valid_moves(300,mv);
  computeValidMoves(valid_moves);
  int best_score = this->estimateCurrentScore()-1000;
  int i=1;
  int curr_val=0;
  while(i<=profondeur){
    best_score = MiniMaxAB(mv,1,i,-10000,10000);
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
      // if (current_player == 1){
      //curr_val = ennemi(*it,1,i);
      // } else {
      curr_val = MiniMaxAB(*it,1,i,-10000,10000);
      //}
        if(curr_val> best_score){
          best_score=curr_val;
          mv = *it;
          printf(" On a trouvé un meilleur score ! \n");
          std::cout<< " score " << curr_val << endl;
          printf("Profondeur : %d\n",i);
        }
    }
    _saveBestMove(mv);
    printf(" On a exploré la profondeur %d \n  Le dernier score sauvegardé est de %d \n", i,best_score);
    i++;
  }
  return mv;
}

int Strategy::MiniMaxAB(move& mv, int curr_prof, int max_prof, int a, int b)
{
  Strategy nextStrat (_blobs,_holes,_current_player,_saveBestMove);
  nextStrat.applyMove(mv);

  int actual_score = nextStrat.estimateCurrentScore();
    nextStrat.nextPlayer(nextStrat);
  if (curr_prof == max_prof){
    return actual_score;
  }
  int alpha = a;
  int beta = b;
  std::vector<move> valid_moves(300,mv);
  computeValidMoves(valid_moves);
  if (curr_prof % 2 == 1){
    for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
      int b_it = MiniMaxAB(*it,curr_prof+1,max_prof,alpha,beta);
      if (beta > b_it){beta = b_it;}
      if (alpha >= beta){return alpha;}
    }
    return beta;
  }
  else {
    for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
      int a_it = MiniMaxAB(*it,curr_prof+1,max_prof,alpha,beta);
      if (alpha < a_it){alpha = a_it;}
      if (alpha >= beta){ return beta;}
    }
    return alpha;
  }
}

// OLD findMoveMinMax
/*
move& Strategy::findMoveMinMax(move& mv, int profondeur){
  std::vector<move> valid_moves(300,mv);
  computeValidMoves(valid_moves);
  int best_score = this->estimateCurrentScore()-1000;
  if(profondeur%2==0) { best_score = best_score + 10000;}
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
    int curr_val = valMinMax(*it,1,profondeur);
    //std::cout << " coucou";
    if(profondeur%2==0)// PAIR
    {

      if(curr_val< best_score){
        best_score=curr_val;
        mv = *it;
        std::cout<< " score " << curr_val << endl;
        _saveBestMove(mv);
      }
    }else{// IMPAIR
      if(curr_val> best_score){
        best_score=curr_val;
        mv = *it;
        std::cout<< " score " << curr_val << endl;
        _saveBestMove(mv);
      }
    }
  }
  return mv;
}
*/


void Strategy::computeBestMove () {
    move mv(-1,-1,-1,-1);// On peut l'améliorer en mettant le premier valid_move trouvé !
    //std::cout<<"player " << _current_player << " with score " << estimateCurrentScore() << endl;
    findMoveMinMax(mv,2);
    //MiniMaxAB(mv,1,3,-10000,10000);//alpha = -infini, beta = +infini

     return;
}



/*
// SECOND VERSION ( GLOUTONNE )
void Strategy::computeBestMove () {
    move mv(-1,-1,-1,-1);
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
        std::cout << " meilleur move ! score : "<<max<<  endl;
        it->display();
      }

    }
     _saveBestMove(mv);
     return;
}
*/



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
