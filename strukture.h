#ifndef REINFORCEMENT_IPD_DEFINITIONS_H
#define REINFORCEMENT_IPD_DEFINITIONS_H


#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>
#include <cstdlib>
using namespace std;

enum class akcija
{
  s, n
};
typedef pair<akcija, akcija> interakcija;


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
       s.vektor_tezina[par.first] = par.second.second;
    }


    // TODO copy asgn, primjena

    parametrizirana_strategija& s;
    mutacija(parametrizirana_strategija& _s);

    mutacija(const mutacija& mut) : map(mut), s(mut.s) {}

    mutacija& operator=(const mutacija& mut)
    {
      map::operator=(mut);
    }

  private:
    tezina_t epsilon() { return (rand() % 20 - 10) * 0.02; }
    void clamp(tezina_t& t) { if (t < 0) t = 0; if (t > 1) t = 1; }
  };


  virtual akcija operator()(const stanje& st)
  {

    /*
     *
     * ovdje ide kod koji na osnovu tezina odreduje ponasanje
     * osnovna ideja: imati w (na pocetku ~= 0.5) kojeg ostale tezine modificiraju
     *
     */

    return akcija::s;
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
  for (int i = 0; i < s.vektor_tezina.size(); ++i)
  {
    (*this)[i].first = (*this)[i].second = s.vektor_tezina[i];
    if (rand() % 10 == 0)
      clamp((*this)[i].second += epsilon());
  }
}


struct populacija
{
  vector<igrac*> obicni_igraci, mutanti;

  igrac* random_izvuci_igraca() { return obicni_igraci[rand() % obicni_igraci.size()]; /* zasad samo obicni */ }
};


#endif //REINFORCEMENT_IPD_DEFINITIONS_H