#ifndef REINFORCEMENT_IPD_DEFINITIONS_H
#define REINFORCEMENT_IPD_DEFINITIONS_H


#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <map>
#include <array>
#include <set>
#include <stdexcept>
#include <cstdlib>
#include "reinforcement_ipd/reinforcement_ipd/Generator.h"
using namespace std;


Double_Generator *rand_epsilon = new Double_Generator(-0.2, 0.2);
Int_Generator *rand_mutation = new Int_Generator(0, 13);

enum class akcija
{
  s, n
};
std::ostream& operator<<(std::ostream& out, const akcija& val)
{
  if (val == akcija::s)
    out << "s";
  else
    out << "n";
  return out;
}
int aint(akcija a)
{
  return a == akcija::n ? 0 : 1;
}

typedef pair<akcija, akcija> interakcija;
const vector<interakcija> prazni_vec_int;


struct strategija;
struct stanje;
struct igrac
{
  strategija *s;
  int id;

  akcija potez(stanje st);
};

struct stanje
{
  igrac* s_kime_trebam_igrati;

  map<
    int, // igrac I
    vector<interakcija> // popis susreta s igracem I
  > povijest;

  interakcija osvjezi(akcija moja, akcija tuda)
  {
    interakcija i = make_pair(moja, tuda);
    povijest[s_kime_trebam_igrati->id].push_back(i);
    return i;
  }

  const vector<interakcija>& povijest_za_trenutnog() const
  {
    if (povijest.find(s_kime_trebam_igrati->id) != povijest.end())
      return povijest.at(s_kime_trebam_igrati->id);
    else
      return prazni_vec_int;
  }

  double uspjesnost() const
  {
    double s = 0, uk = 0;
    for (auto &igrac_vek : povijest)
      for (auto &interakcija : igrac_vek.second)
      {
        uk += 10;
        if (interakcija.first == akcija::s)
        {
          if (interakcija.second == akcija::s)
          {
            s += 8;
          }
          else
            s += 1;
        }
        else
        {
          if (interakcija.second == akcija::s)
          {
            s += 10;
          }
          else
            s += 4;
        }
      }

    return s / uk;

  }

};


typedef function<akcija(const stanje&)> izvrsiva_strategija;

struct strategija
{
  virtual akcija operator()(const stanje& st) = 0;
};

akcija igrac::potez(stanje st)
{
  return (*s)(st);
}


bool barem_jedan(const stanje& st, akcija a)
{
  for (auto interakcija : st.povijest_za_trenutnog())
    if (interakcija.second == a)
      return true;

  return false;
}

akcija nti_korak(const stanje& st, int n)
{
  if (n < 0)
  {
    n += st.povijest_za_trenutnog().size();
  }
  return st.povijest_za_trenutnog().at(n).second;
}

akcija nti_korak_moj(const stanje& st, int n)
{
  if (n < 0)
  {
    n += st.povijest_za_trenutnog().size();
  }
  return st.povijest_za_trenutnog().at(n).first;
}

akcija vecinska_akcija(const stanje& st)
{
  int ss = 0, nn = 0;
  for (auto interakcija : st.povijest_za_trenutnog())
    if (interakcija.second == akcija::s)
      ++ss;
    else
      ++nn;
  return (ss >= nn) ? akcija::s : akcija::n;
}

int izbroji_akcije(const stanje& st, akcija a)
{
  int ss = 0;
  for (auto interakcija : st.povijest_za_trenutnog())
    if (interakcija.second == a)
      ++ss;
  return ss;
}

int dobrih_zadnjih_koraka(const stanje& st)
{
  int ss = 0;
  for (auto interakcija : st.povijest_za_trenutnog())
    if (interakcija.second == akcija::s)
      ++ ss;
    else
      ss = 0;
  return ss;
}


struct klasicna_strategija; // u beaufils.h

struct parametrizirana_strategija : strategija
{
  typedef double tezina_t;

#define novi_parametar(ime, pomak)  vector<tezina_t>::iterator ime() { return vektor_tezina.begin() + pomak; }

  // interpretacija tezine: niska vr = n ili suprotno, srednja vr = neovisno, visoka vr = s ili jednako
  vector< tezina_t > vektor_tezina;

  // output vise ovisi o daljnjim karakteristikama (impulzivnost?)
  novi_parametar(inicijalna_slucajnost, 0)

  // ovisnost o prethodna tri koraka
  novi_parametar(reakcija_za_posljednje_korake, 1)

  // ovisnost o prvom tudem koraku
  novi_parametar(reakcija_na_prvi, 4)

  // reakcija o postojanju s/n bilo gdje do sada
  novi_parametar(osvetoljubivost, 5)
  novi_parametar(anti_osvetoljubivost, 6)

  // utjecaj djeljivosti rednog broja na reakciju (xn, xn + 1 (, xn + 2))
  novi_parametar(ritmicnost2, 7)
  novi_parametar(ritmicnost3, 9)

  // ovisnost o vecinskoj reakciji
  novi_parametar(utjecaj_vecine, 12)

  // trajanje "osvete" za svaku nesuradnju
  novi_parametar(trajanje_osvete, 13)


  parametrizirana_strategija()
  {
    vektor_tezina.resize(14);

    for (auto& tezina : vektor_tezina)
      tezina = 0.5;
  }

  enum class utjecaj { n_ili_ne, nista, s_ili_da };

  struct utjecaj_s_tezinom : public pair<utjecaj, tezina_t> {
    tezina_t tez;
    utjecaj  ut;

    utjecaj_s_tezinom(const pair<utjecaj, tezina_t>& par) : tez(par.second), ut(par.first) {}
  };

  utjecaj_s_tezinom procesuiraj_tezinu(vector<tezina_t>::iterator tezina)
  {
    tezina_t val = *tezina;
    return make_pair( val < 1/3 ? utjecaj::n_ili_ne : (val < 2/3 ? utjecaj::nista : utjecaj::s_ili_da),
                      abs(val < 1/3 ? 3 * val  :  (val < 2/3 ? 0 : 3 * (val - 2/3))));
  };

  struct mutacija :
    public map
    <
      int,   // koordinata
      pair
      <
        tezina_t, // stara vr.
        tezina_t  // nova vr.
      >
    >
  {
    // utjece li ova tezina na rezultat
    bool aktivna_tezina(tezina_t tez) const { return 1/3 <= tez && tez <= 2/3; }

    // one koje su promijenile kategoriju (ušle ili izašle u [1/3, 2/3])
    mutacija kljucne_mutacije() const;

    // on koje su bile i ostale izvan [1/3, 2/3], dakle promijenile intenzitet ali ne i nacelno djelovanje
    mutacija manje_kljucne_mutacije() const;

    // one koje su bile i ostale u [1/3, 2/3], te one koje se nisu mijenjale
    mutacija irelevantne_mutacije() const;

    void primjena()
    {
      for (auto& par : *this)
       clamp(s.vektor_tezina[par.first] += par.second.second - par.second.first);
    }

    void anti_primjena()
    {
      for (auto& par : *this)
        clamp(s.vektor_tezina[par.first] = par.second.first);
    }


    // TODO copy asgn, primjena

    parametrizirana_strategija& s;
    mutacija(parametrizirana_strategija& _s);

    mutacija(const mutacija& mut) : map(mut), s(mut.s) {}

    mutacija& operator=(const mutacija& mut)
    {
      map::operator=(mut);
      return *this;
    }

  private:
    tezina_t epsilon() {
		return rand_epsilon->Produce(); /* -0.2 do +0.2 */ }
    void clamp(tezina_t& t) { if (t < 0) t = 0; if (t > 1) t = 1; }
	

  };

;

  virtual akcija operator()(const stanje& st)
  {
    int tr_korak = st.povijest_za_trenutnog().size();
    tezina_t acm = 0;

    auto y = procesuiraj_tezinu(inicijalna_slucajnost());
    if (y.ut == utjecaj::s_ili_da)
    {
      acm = 0.5 + 0.5 * (y.tez);
    }
    else if (y.ut == utjecaj::n_ili_ne)
    {
      acm = 0.5 - 0.5 * (y.tez);
    }
    else
    {
      acm = 0.5;
    }

    // barem jedna suradnja uzrokuje... (preferirati s?)
    y = procesuiraj_tezinu(this->anti_osvetoljubivost());
    if (y.ut == utjecaj::s_ili_da && barem_jedan(st, akcija::s))
    {
      acm += 0.3 * (y.tez);
    }
    else if (y.ut == utjecaj::n_ili_ne && barem_jedan(st, akcija::s))
    {
      acm -= 0.3 * (y.tez);
    }


    // barem jedna nesuradnja uzrokuje... (preferirati n?)
    y = procesuiraj_tezinu(this->osvetoljubivost());
    if (y.ut == utjecaj::s_ili_da && barem_jedan(st, akcija::n))
    {
      acm += 0.3 * (y.tez);
    }
    else if (y.ut == utjecaj::n_ili_ne && barem_jedan(st, akcija::n))
    {
      acm -= 0.3 * (y.tez);
    }


    // velika tezina -> ponasati se isto kao protivnik u prvom; mala -> suprotno
    y = procesuiraj_tezinu(this->reakcija_na_prvi());
    if (tr_korak > 0)
      if (y.ut == utjecaj::s_ili_da)
      {
        if (nti_korak(st, 0) == akcija::s)
          acm += 0.3 * (y.tez);
        else
          acm -= 0.3 * (y.tez);
      }
      else if (y.ut == utjecaj::n_ili_ne)
      {
        if (nti_korak(st, 0) == akcija::s)
          acm -= 0.3 * (y.tez);
        else
          acm += 0.3 * (y.tez);
      }


    // posljednja 3 koraka
    for (int i = 1; i <= 3; ++i)
    {
      if (tr_korak < i)
        continue;

      y = procesuiraj_tezinu(this->reakcija_za_posljednje_korake() + (i - 1));
      if (y.ut == utjecaj::s_ili_da)
      {
        if (nti_korak(st, -i) == akcija::s)
          acm += 0.3 * (y.tez);
        else
          acm -= 0.3 * (y.tez);
      }
      else if (y.ut == utjecaj::n_ili_ne)
      {
        if (nti_korak(st, -i) == akcija::s)
          acm -= 0.3 * (y.tez);
        else
          acm += 0.3 * (y.tez);
      }
    }

    // tendencija za 2k + 1 korake
    y = procesuiraj_tezinu(this->ritmicnost2() + 1);
      if (st.povijest_za_trenutnog().size() % 2 == 0)
      if (y.ut == utjecaj::s_ili_da)
      {
        acm += 0.1 * (y.tez);
      }
      else if (y.ut == utjecaj::n_ili_ne)
      {
        acm -= 0.1 * (y.tez);
      }
    // tendencija za 2k korake
    y = procesuiraj_tezinu(this->ritmicnost2());
    if (st.povijest_za_trenutnog().size() % 2 == 1)
      if (y.ut == utjecaj::s_ili_da)
      {
        acm += 0.1 * (y.tez);
      }
      else if (y.ut == utjecaj::n_ili_ne)
      {
        acm -= 0.1 * (y.tez);
      }


    // tendencija za 3k + 1 korake
    y = procesuiraj_tezinu(this->ritmicnost3() + 1);
    if (st.povijest_za_trenutnog().size() % 3 == 0)
      if (y.ut == utjecaj::s_ili_da)
      {
        acm += 0.1 * (y.tez);
      }
      else if (y.ut == utjecaj::n_ili_ne)
      {
        acm -= 0.1 * (y.tez);
      }
    // tendencija za 3k + 2 korake
    y = procesuiraj_tezinu(this->ritmicnost3() + 2);
    if (st.povijest_za_trenutnog().size() % 3 == 1)
      if (y.ut == utjecaj::s_ili_da)
      {
        acm += 0.1 * (y.tez);
      }
      else if (y.ut == utjecaj::n_ili_ne)
      {
        acm -= 0.1 * (y.tez);
      }
    // tendencija za 3k korake
    y = procesuiraj_tezinu(this->ritmicnost3());
    if (st.povijest_za_trenutnog().size() % 3 == 2)
      if (y.ut == utjecaj::s_ili_da)
      {
        acm += 0.1 * (y.tez);
      }
      else if (y.ut == utjecaj::n_ili_ne)
      {
        acm -= 0.1 * (y.tez);
      }

    y = procesuiraj_tezinu(this->utjecaj_vecine());
    if (y.ut == utjecaj::s_ili_da)
    {
      akcija v = vecinska_akcija(st);
      if (v == akcija::s)
        acm += 0.3 * (y.tez);
      else
        acm -= 0.3 * (y.tez);
    }
    else if (y.ut == utjecaj::n_ili_ne)
    {
      akcija v = vecinska_akcija(st);
      if (v == akcija::s)
        acm -= 0.3 * (y.tez);
      else
        acm += 0.3 * (y.tez);
    }

    y = procesuiraj_tezinu(this->trajanje_osvete());
    if (y.ut == utjecaj::s_ili_da)
    {
      int preostalo_kazne = 0;
      if (y.tez > 0.5) // vise gradual
      {
        int ocekivano_trajanje = (y.tez - 0.5) * 2 * izbroji_akcije (st, akcija::n);
        int izdrzana_kazna = dobrih_zadnjih_koraka(st);
        preostalo_kazne = ocekivano_trajanje - izdrzana_kazna;
      }
      else if (tr_korak > 1) // vise TFT
      {
        preostalo_kazne = nti_korak(st, -1) == akcija::s ? 0 : 1;
      }
      else
      {
        preostalo_kazne = 0;
      }

      if ((preostalo_kazne == 0) && (y.tez >= 0.3))
        acm += 0.6; // prekid osvete

      if (preostalo_kazne > 0)
        acm -= 0.5;
    }



    if (tr_korak >= 20)
    {
      int prozori[8] = {0}; // za x, y, z iz {n = 0, s = 1}, na prozori[4 * x + 2 * y + z]
                            // je broj takvih trojki iz povijesti, gdje su x i y moje akcije,
                            // a z protivnikova reakcija
      for (int prozor_pocetak = 0; prozor_pocetak <= tr_korak - 3; ++prozor_pocetak)
      {
        int x = aint(nti_korak_moj(st, prozor_pocetak)),
          y = aint(nti_korak_moj(st, prozor_pocetak + 1)),
          z = aint(nti_korak(st, prozor_pocetak + 2));

        ++prozori[4 * x + 2 * y + z];
      }

      int X = aint(nti_korak_moj(st, tr_korak - 2)),
        Y = aint(nti_korak_moj(st, tr_korak - 1));

      // pokusamo predvidjeti z

      int adresa = 4 * X + 2 * Y;
      int Z_n = prozori[adresa],
        Z_s = prozori[adresa + 1];
      if (Z_n > 2 * Z_s)
        acm -= 5;
      //if (Z_s > 2 * Z_n)
      //  acm += 5;

      // bez prozora: 73
      // > 2 *, +-1 daje 73
      // > 2 *, +-10 daje 74
      // > 1 *, +-10 daje 74
      // > 1.1 *, +- 10 daje
    }



    return (acm >= 0.5) ? akcija::s : akcija::n;
  }

};

parametrizirana_strategija::mutacija parametrizirana_strategija::mutacija::kljucne_mutacije() const
{
  mutacija filt(s);
  for (const auto &par : *this)
    if (aktivna_tezina(par.second.first) != aktivna_tezina(par.second.second))
      filt[par.first] = par.second;
  return filt;
}

parametrizirana_strategija::mutacija parametrizirana_strategija::mutacija::manje_kljucne_mutacije() const
{
  mutacija filt(s);
  for (auto &par : *this)
    if (aktivna_tezina(par.second.first) && aktivna_tezina(par.second.second))
      filt[par.first] = par.second;
  return filt;
}

parametrizirana_strategija::mutacija parametrizirana_strategija::mutacija::irelevantne_mutacije() const
{
  mutacija filt(s);
  for (auto &par : *this)
    if (!aktivna_tezina(par.second.first) && !aktivna_tezina(par.second.second))
      filt[par.first] = par.second;
  return filt;
}

parametrizirana_strategija::mutacija::mutacija(parametrizirana_strategija &_s) : s(_s)
{
  for (int i = 0; i < 4; ++i)
  {
	int j = rand_mutation->Produce();
    (*this)[j].first = (*this)[j].second = s.vektor_tezina[j];
    clamp((*this)[j].second += epsilon());
  }
}


struct populacija
{
  vector<igrac*> obicni_igraci, mutanti;
  igrac* random_izvuci_igraca() { 
	  if (rand == NULL)
	  {
		  rand = new Int_Generator(0, obicni_igraci.size() - 1);
	  }
	  return obicni_igraci[rand->Produce()]; /* zasad samo obicni */ }

private:
	Int_Generator *rand = NULL;
};


#endif //REINFORCEMENT_IPD_DEFINITIONS_H
