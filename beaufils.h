#ifndef REINFORCEMENT_IPD_BEAUFILS_H
#define REINFORCEMENT_IPD_BEAUFILS_H

// part of this file is adapted from http://www.lifl.fr/IPD/ipd.html.en#softwares
// PRISON, Copyright (C) 1992-1998 by LIFL (SMAC team)


#include "strukture.h"

enum B_akcija { COOPERATE, DEFECT };
typedef B_akcija card;

int turn = -1;

struct _emul_history
{
  vector<interakcija> *izvor;
  bool ja_sam_glavni = true;

  card operator[](int n)
  {
    auto i = izvor->at(n - 1);
    if (ja_sam_glavni)
      return i.first  == akcija::n ? DEFECT : COOPERATE;
    else
      return i.second == akcija::n ? DEFECT : COOPERATE;
  }
} RH, MH;

int nbC(_emul_history& emh) { return count_if(emh.izvor->begin(), emh.izvor->end(), [&](const interakcija& i)
  {
    if (emh.ja_sam_glavni)
      return i.first == akcija::s;
    else
      return i.second == akcija::s;
  });
}

int nbD(_emul_history& emh) { return count_if(emh.izvor->begin(), emh.izvor->end(), [&](const interakcija& i)
  {
    if (emh.ja_sam_glavni)
      return i.first == akcija::n;
    else
      return i.second == akcija::n;
  });
}

B_akcija _pokreni_klasicnu_strategiju(function<card(int)> st_f, stanje s)
{
  auto &povijest = s.povijest[s.s_kime_trebam_igrati->id];
  turn = povijest.size() + 1;
  MH.ja_sam_glavni = false; // MH je nasa strategija, a mi nismo klasicna
  RH.ja_sam_glavni = true;
  MH.izvor = RH.izvor = &povijest;

  return st_f(s.s_kime_trebam_igrati->id /*irelevantan parametar */ );
}

struct klasicna_strategija : strategija
{
  function<card(int)> ime;
  klasicna_strategija(function<card(int)>  _ime) : ime(_ime) {}

  virtual akcija operator()(const stanje& st)
  {
    return _pokreni_klasicnu_strategiju(ime, st) == COOPERATE ? akcija::s : akcija::n;
  }

};

populacija klasicna_populacija(const initializer_list<function<card(int)>>& strategije, int kopija)
{
  populacija p;
  int k = 1;
  for (auto &s : strategije)
    for (int i = 0; i < kopija; ++i, ++k)
      p.obicni_igraci.push_back(new igrac{new klasicna_strategija(s), k});
  return p;
}









namespace bfs
{

  card
  all_c(int X)
  {
    /* Always cooperates. [c]* */

    return COOPERATE;
  }

  card
  all_d(int X)
  {
    /* Always defects. [d]* */
    return DEFECT;
  }

  card
  tit_for_tat(int X)
  {
    /* The tit_for_tat strategy was introduced by Anatole Rapoport. It begins to
       cooperate, and then play what its opponent played in the last move.  */

    if (turn == 1)
      return COOPERATE;
    else
      return RH[turn - 1];
  }

  card
  mistrust(int X)
  {
    /* Defects, then plays opponent's move. */

    if (turn == 1)
      return DEFECT;
    else
      return RH[turn - 1];
  }

  card
  spiteful(int X)
  {
    /* It cooperates until the opponent has defected, after that move it always
       defects.  */

    if (turn == 1)
      return COOPERATE;
    else if (RH[turn - 1] == DEFECT || MH[turn - 1] == DEFECT)
      return DEFECT;
    else
      return COOPERATE;
  }

  card
  per_ddc(int X)
  {
    /* Plays periodically : [d,d,c]*  */

    if (turn % 3 == 0)
      return COOPERATE;
    else
      return DEFECT;
  }

  card
  per_ccd(int X)
  {
    /* Plays periodically : [c,c,d]*  */

    if (turn % 3 == 0)
      return DEFECT;
    else
      return COOPERATE;
  }

  card
  soft_majo(int X)
  {
    /* Cooperates, then plays opponent's majority move, if equal then
       cooperates.  */

    if (nbC(RH) >= nbD(RH))
      return COOPERATE;
    else
      return DEFECT;
  }

  card
  hard_majo(int X)
  {
    /* Cooperates, then plays opponent's majority move, if equal then
       defects.  */

    if (nbC(RH) > nbD(RH))
      return COOPERATE;
    else
      return DEFECT;
  }

  card
  prober(int X)
  {
    /* Plays [d,c,c], then it defects in all other move if opponent has
       cooperated in move 2 and 3, and plays as tit_for_tat in other cases.  */

    if (turn == 1) return DEFECT;
    else if (turn == 2) return COOPERATE;
    else if (turn == 3) return COOPERATE;
    else if (RH[2] == COOPERATE && RH[3] == COOPERATE)
      return DEFECT;
    else
      return RH[turn - 1]; /* tit for tat */
  }

  card
  hard_tft(int X)
  {
    /* Cooperates except if opponent has defected at least one time in the two
       previous move. */

    if (turn == 1)
      return COOPERATE;
      // luka: del: else if (defect_lim(2,RH) >= 1)
    else if (RH[turn - 1] == DEFECT || (turn >= 3) && (RH[turn - 2] == DEFECT))
      return DEFECT;
    else
      return COOPERATE;
  }


  card
  gradual_killer(int X)
  {
    /* This strategy has been built on a late friday evening in order to be sure
       that tit_for_tat can beat gradual in round-robin tournament as well as in
       ecological competition. To achieve this goal it begins by defecting in
       the first fifth moves, then cooperates two times. Then it defects all the
       time if the opponent has defected in move 6 and 7, and else cooperates
       all the times. It works with an environment of 3 strategies :
       tit_for_tat, gradual and gradual_killer */

    if (turn <= 5)
      return DEFECT;
    else if (turn <= 7)
      return COOPERATE;
    else if (RH[6] == DEFECT && RH[7] == DEFECT)
      return DEFECT;
    else
      return COOPERATE;
  }

  card
  hard_tf2t(int X)
  {
    /* Cooperates except if opponent has defected two consecutive times in the
       last 3 moves. */

    if (turn <= 2)
      return COOPERATE;
    else if (RH[turn - 1] == DEFECT && RH[turn - 2] == DEFECT)
      return DEFECT;
    else if (turn > 3 && RH[turn - 2] == DEFECT && RH[turn - 3] == DEFECT)
      return DEFECT;
    else
      return COOPERATE;
  }

  card
  soft_tf2t(int X)
  {
    /* Cooperates except if opponent has defected two consecutive times. */

    if (turn <= 2)
      return COOPERATE;
    else if (RH[turn - 1] == DEFECT && RH[turn - 2] == DEFECT)
      return DEFECT;
    else
      return COOPERATE;
  }

#ifdef neimpl
  card
  c_then_per_dc (int X)
  {
    /* Cooperates until first opponent's defection, then plays [d,c]*. */

    static counter n;

    if (turn==1)
    {
      n[X]=0;
      return COOPERATE;
    }
    else if (n[X] == 1)
    {
      if (MH[turn - 1]==DEFECT)
        return COOPERATE;
      else
        return DEFECT;
    }
    else if (RH[turn - 1]==DEFECT)
    {
      n[X]=1;
      return DEFECT;
    }
    else
      return COOPERATE;
  }

  card
  per_cd (int X)
  {
    /* Plays periodically [c,d].   */

    if ((turn%2) != 0)
      return COOPERATE;
    else
      return DEFECT;
  }

  card
  per_cccdcd (int X)
  {
    /* Plays periodically : [c,c,c,d,c,d]*.  */

    if      (turn % 6 == 1) return COOPERATE;
    else if (turn % 6 == 2) return COOPERATE;
    else if (turn % 6 == 3) return COOPERATE;
    else if (turn % 6 == 4) return DEFECT;
    else if (turn % 6 == 5) return COOPERATE;
    else                    return DEFECT;
  }

  card
  per_ccccd (int X)
  {
    /* Plays periodically : [c,c,c,c,d]*. */

    if ((turn%5) == 0)
      return DEFECT;
    else
      return COOPERATE;
  }

  card
  prober2 (int X)
  {
    /* Plays [d,c,c], then if opponent plays [d,c] in move 2 and 3, always
       cooperate, else plays as tit for tat. */

    if      (turn==1) return DEFECT;
    else if (turn==2) return COOPERATE;
    else if (turn==3) return COOPERATE;
    else if (RH[2]==DEFECT && RH[3]==COOPERATE)
      return COOPERATE;
    else
      return RH[turn-1]; /* tit for tat */
  }

  card
  prober3 (int X)
  {
    /* Plays [d,c] then if opponent cooperates in move 2 always defect, else
       plays as tit for tat. */

    if      (turn==1) return DEFECT;
    else if (turn==2) return COOPERATE;
    else if (RH[2]==COOPERATE)
      return DEFECT;
    else
      return RH[turn-1]; /* tit for tat */
  }

  card
  prober4 (int X)
  {
    /* Plays [c,c,d,c,d,d,d,c,c,d,c,d,c,c,d,c,d,d,c,d] then ... */

    static counter nb_d_d, nb_c_d;
    static char what[22]=" ccdcdddccdcdccdcddcd";

    if (turn==1)
    {
      nb_c_d[X]=0;
      nb_d_d[X]=0;
      return COOPERATE;
    }
    else if (turn==2)
      return COOPERATE;
    else
    {
      if (turn <= 20)
      {
        if (what[turn-2]=='c' && RH[turn-1]==DEFECT) nb_c_d[X]++;
        if (what[turn-2]=='d' && RH[turn-1]==DEFECT) nb_d_d[X]++;
        if (what[turn]=='c')
          return COOPERATE;
        else
          return DEFECT;
      }
      else
      if (abs(nb_c_d[X] - nb_d_d[X]) <= 2)
        return DEFECT;
      else
      if (turn<=25)
        return COOPERATE;
      else
        return RH[turn-1];
    }
  }

  card
  hard_prober (int X)
  {
    /* Plays [d,d,c,c], then if opponent cooperated in move 2 and 3 always
       defects, else plays as tit for tat. */

    if      (turn==1 || turn==2) return DEFECT;
    else if (turn==3 || turn==4) return COOPERATE;
    else if ((RH[2]==COOPERATE) && (RH[3]==COOPERATE))
      return DEFECT;
    else
      return RH[turn-1]; /* tit for tat */
  }

  card
  doubler (int X)
  {
    /* Cooperates except when opponent defected, and opponent's cooperations is
       less than 2 times opponent's defections. */

    static counter nb_d, nb_c;
    if (turn==1)
    {
      nb_c[X]=0;
      nb_d[X]=0;
      return COOPERATE;
    }
    else
    {
      if (RH[turn - 1] == DEFECT)
        nb_d[X]++;
      else
        nb_c[X]++;
      if (RH[turn - 1]==DEFECT && nb_c[X]<=2*nb_d[X])
        return DEFECT;
      else
        return COOPERATE;
    }
  }

  card
  soft_spiteful (int X)
  {
    /* Cooperates except when opponent defected, in such a case opponent is
       punished with [d,d,d,d,c,c]. */

    static counter n;

    if (turn==1)
    {
      n[X]=0;
      return COOPERATE;
    }
    else
    {
      if (n[X]==1 || n[X]==2)
      {
        n[X]--;
        return COOPERATE;
      }
      else if (n[X] > 2)
      {
        n[X]--;
        return DEFECT;
      }
      else if (RH[turn - 1]==DEFECT)
      {
        n[X] = 4+2-1;
        return DEFECT;
      }
      else
        return COOPERATE;
    }
  }

  card
  pavlov (int X)
  {
    /* The win-stay/lose-shift strategy is studied in nowak-sigmund:strategy.
       It cooperates if and only if both players opted for the same choice in
       the previous move.  */

    if (turn == 1)
      return COOPERATE;
    else if (MH[turn-1] == RH[turn-1])
      return COOPERATE;
    else
      return DEFECT;
  }

  card
  slow_tft (int X)
  {
    /* Plays [c,c], then if opponent plays two consecutive time the same move
       plays my opposite move .*/

    if (turn<=2)
      return COOPERATE;
    if (RH[turn-1] == RH[turn-2])
      return RH[turn-1];
    else
      return MH[turn-1];
  }

  card
  easy_go (int X)
  {
    /* It defects until the opponent has defected, after that move it always
       cooperates.  */

    if (turn == 1)
      return DEFECT;
    else
    if (RH[turn - 1] == DEFECT || MH[turn - 1] == COOPERATE)
      return COOPERATE;
    else
      return DEFECT;
  }


  /*****************************************************************************
   * Strategies with random intervention.
   *****************************************************************************/
  card
  ipd_random (int X)
  {
    /* Cooperates with probability 1/2. */

    if ((rand()%2) != 0)
      return COOPERATE;
    else
      return DEFECT;
  }

  card
  hard_joss (int X)
  {
    /* Plays as tit for tat, but cooperates with a probability of only 0.9. */

    if (turn==1)
      return COOPERATE;
    else if (RH[turn - 1]==DEFECT)
      return DEFECT;
    else if ((rand()%10) != 0)
      return COOPERATE;
    else
      return DEFECT;
  }

  card
  soft_joss (int X)
  {
    /* Plays as tit for tat, but defects with a probability of only 0.9. */

    if (turn == 1)
      return COOPERATE;
    else if (RH[turn - 1]==COOPERATE)
      return COOPERATE;
    else if ((rand()%10) != 0)
      return DEFECT;
    else
      return COOPERATE;
  }

  card
  prob_c_4_on_5 (int X)
  {
    /* Cooperates with probability  p(c)=4/5.  */

    if ((rand()%5) != 0)
      return COOPERATE;
    else
      return DEFECT;
  }

  card
  calculator (int X)
  {
    /* Plays as hard_joss in the first 20 moves. Then if opponent played
       periodically, always defects, else plays as tit for tat. */

    static counter period, find;
    int j, end;

    if (turn<=20)
      return hard_joss(X); /* no static var in hard_joss */
    if (turn==21)
    {
      /* looking for a period. */
      find[X]=0;
      for (period[X]=1; !find[X] && period[X]<=(turn/2); period[X]++)
      {
        end=0;
        for (j=1; j<=(turn - 1 - period[X]) && !end; j++)
        {
          if (RH[j] != RH[j+period[X]])
            end=1;
        }
        if (!end)
          find[X]=1;
      }
    }
    if (find[X] == 1)
      return DEFECT;
    else
      return RH[turn-1]; /* tit for tat */
  }

  card
  better_and_better (int X)
  {
    /* Defects with probability (1000-turn)/1000, so less and less. */

    if ((rand()%1000) < (1000-turn))
      return DEFECT;
    else
      return COOPERATE;
  }

  card
  worse_and_worse (int X)
  {
    /* Defects with probability  turn/1000, so more and more. */

    if ((rand()%1000) < turn)
      return DEFECT;
    else
      return COOPERATE;
  }

  card
  worse_and_worse2 (int X)
  {
    /* Plays as tit for tat during the first 20 moves, then defects with probability
       (turn-20) / turn. */

    if (turn==1)  return COOPERATE;
    if (turn<=20) return RH[turn-1];
    else if (RH[turn-1]==DEFECT)
      return DEFECT;
    else if ((rand()%turn) < (turn-20))
      return DEFECT;
    else
      return COOPERATE;
  }

  card
  worse_and_worse3 (int X)
  {
    /* Cooperates, then the more the opponent defect, the more it defects,
       i.e. in move turn+1 defects with probability nb(d)/turn. */

    static counter n;

    if (turn==1)
    {
      n[X]=0;
      return COOPERATE;
    }
    else if (RH[turn - 1]==DEFECT)
      n[X]++;

    if ((rand()%turn) < n[X])
      return DEFECT;
    else
      return COOPERATE;
  }

#endif // neimplem.

}

#endif //REINFORCEMENT_IPD_BEAUFILS_H
