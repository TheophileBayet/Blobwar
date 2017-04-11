#include "strategy.h"
#include <vector>
#include<time.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <sys/time.h> // pour gettimeofday
#include <limits>
#include <cstring>

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
  //return scores[1]-scores[0];
  return scores[1-_current_player]-scores[_current_player];// TODO ; changer pour application à 4 joueurs
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
  int eval = 1000;
  int best_score = eval;
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
    eval = ami(*it,curr_prof+1,max_prof);
    //MAX
    if(eval < best_score){
      best_score = eval;
    }
  }
  return best_score;
}

int Strategy::ennemi_par(move& mv, int curr_prof, int max_prof){
  Strategy nextStrat (_blobs,_holes,_current_player,_saveBestMove);
  nextStrat.applyMove(mv);
  int actual_score = nextStrat.estimateCurrentScore();
  if(curr_prof==max_prof){
    //printf("profondeur act : %d ; prof max : %d , valeur retournée : %d\n",curr_prof,max_prof,actual_score);
    return actual_score;
  }
  nextStrat.nextPlayer(nextStrat);
  std::vector<move> valid_moves(300,mv);
  nextStrat.computeValidMoves(valid_moves);
  int eval[valid_moves.size()];
  int best_score;
  int k = 0;
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
    #pragma omp task shared(eval)
    {
    eval[k] = ami(*it,curr_prof+1,max_prof);
    }
    k++;
  }
    best_score = eval[0];
    //MIN
    for(int l=1 ; l<k;l++){
      if(eval[l] < best_score){
        best_score = eval[l];
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
  int eval = -1000;
  int best_score = eval;
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
    eval = ennemi(*it,curr_prof+1,max_prof);
    //MAX
    if(eval > best_score){
      best_score = eval;
    }
  }
  return best_score;
}


int Strategy::ami_par(move& mv, int curr_prof, int max_prof){
  Strategy nextStrat (_blobs,_holes,_current_player,_saveBestMove);
  nextStrat.applyMove(mv);
  int actual_score = nextStrat.estimateCurrentScore();
  if(curr_prof==max_prof){
  //  printf("profondeur act : %d ; prof max : %d , valeur retournée : %d\n",curr_prof,max_prof,actual_score);

    return actual_score;
  }
  nextStrat.nextPlayer(nextStrat);
  std::vector<move> valid_moves(300,mv);
  nextStrat.computeValidMoves(valid_moves);
  int eval[valid_moves.size()];
  int best_score;
  int k = 0;
  for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
    #pragma omp task shared(eval)
    {
    eval[k] = ennemi_par(*it,curr_prof+1,max_prof);
    }
    k++;
  }
    best_score = eval[0];
    //MAX
    for(int l=1 ; l<k;l++){
      if(eval[l] > best_score){
        best_score = eval[l];
        //printf("pvaleur best_score  : %d\n",best_score);

    }
  }
  return best_score;
}

move& Strategy::findMoveMinMax(move& mv, int profondeur){
  /** En rester ici si ça ne remarche pas **/
  float temps ;
  clock_t t1,t2;
  std::vector<move> valid_moves(300,mv);
  computeValidMoves(valid_moves);
  int best_score = this->estimateCurrentScore()-1000;
  int i=1;
  mv = valid_moves[0];
  int curr_val=0;

  while(i<=profondeur){
    t1=clock();

                  /*** Début Min-Max PAR ***/




      best_score=ami_par(mv,1,i);
      int k = 0;
      int vals[valid_moves.size()];// Tableau des valeurs des mouvements valides
      for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
        #pragma omp task shared(vals)
        {
          vals[k]=ami_par(*it,1,i);
        }
        k++;
      }
        #pragma omp taskwait // synchro
          best_score=vals[0];
          for(int l =1;l<=k;l++){
            curr_val=vals[l];
            if(curr_val<best_score){
              printf(" On a trouvé un meilleur score (MAX) ! \n");
              std::cout<< " score " << curr_val << endl;
              printf("Profondeur : %d\n",i);
              best_score=curr_val;
              mv=valid_moves[l];
            }
          }



            /*** Fin Min-Max PAR ***/


          /*** Début Min-Max SEQ***/


/*

      best_score=ami(mv,1,i);
      for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
        curr_val=ami(*it,1,i);
        //printf("curr_val=%d\n",curr_val);
        if(curr_val< best_score){
          best_score=curr_val;
          mv = *it;
          printf(" On a trouvé un meilleur score (MAX) ! \n");
          std::cout<< " score " << curr_val << endl;
          printf("Profondeur : %d\n",i);
        }
      } // Fin parcours éléments

*/

            /*** Fin Min-Max SEQ***/


            /*** Début alpha-beta ***/
/*
    best_score = MiniMaxAB(mv,1,i,-10000,10000);
    for (std::vector<move>::iterator it = valid_moves.begin(); it != valid_moves.end(); ++it){
      curr_val = MiniMaxAB(*it,1,i,-10000,10000);
        if(curr_val< best_score){
          best_score=curr_val;
          mv = *it;
          printf(" On a trouvé un meilleur score ! \n");
          std::cout<< " score " << curr_val << endl;
          printf("Profondeur : %d\n",i);
        }
      }*/

      /** Fin alpha-beta **/

    /** Fin du while : On sauve le move trouvé, on affiche le temps de calcul et on incrémente **/
    _saveBestMove(mv);
    printf(" On a exploré la profondeur %d \n  Le dernier score sauvegardé est de %d \n", i,best_score);
    t2=clock();
    temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    printf("temps d'execution pourla profondeur %d = %f secondes \n",i, temps);
    i++;
  } // FIN WHILE




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
  if (curr_prof % 2 == 0){
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

void Strategy::computeBestMove () {
    move mv(-1,-1,-1,-1);// On peut l'améliorer en mettant le premier valid_move trouvé !
    findMoveMinMax(mv,5);
     return;
}
