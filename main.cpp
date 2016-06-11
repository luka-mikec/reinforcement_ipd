#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <stdexcept>
using namespace std;

// definiramo: akcija, interakcija, stanje, strategija, igrac, populacija

enum class akcija
{
  s, n
};
typedef pair<akcija, akcija> interakcija;

struct stanje
{
  igrac* s_kime_trebam_igrati;

  map<
    int, // igrac I
    vector<interakcija> // popis susreta s igracem I
  > povijest;
};

typedef function<akcija(stanje)> izvrsiva_strategija; // jer neke strategije
izvrsiva_strategija parametrizirana_kontrolna_funkcija; // naknadno definirana

struct strategija
{
  // neke strategije pokrecemo pomocu donjeg vektora, a neke su iz koda iz clanka
  izvrsiva_strategija evaluator = parametrizirana_kontrolna_funkcija;

  typedef double tezina_t;

  // interpretacija tezine: niska vr = n ili suprotno, srednja vr = neovisno, visoka vr = s ili jednako
  tezina_t
    // manja vrijednost = output vise ovisi o daljnjim karakteristikama (impulzivnost?)
    inicijalna_slucajnost,

    // ovisnost o prethodna tri koraka
    reakcija_za_posljednje_korake[3],

    // ovisnost o prvom tudem koraku
    reakcija_na_prvi,

    // reakcija o postojanju s/n bilo gdje do sada
    osvetoljubivost, anti_osvetoljubivost,

    // utjecaj djeljivosti rednog broja na reakciju (xn, xn + 1 (, xn + 2))
    ritmicnost2[2], ritmicnost3[2],

    // ovisnost o vecinskoj reakcijigradual_faktor
    utjecaj_vecine,

    // trajanje "osvete" za svaku nesuradnju
    trajanje_osvete;

  reference_wrapper<tezina_t> vektor_tezina[] =
  {
    inicijalna_slucajnost,
    reakcija_za_posljednje_korake[0], reakcija_za_posljednje_korake[1], reakcija_za_posljednje_korake[2],
    reakcija_na_prvi,
    osvetoljubivost, anti_osvetoljubivost,
    ritmicnost2[0], ritmicnost2[1], ritmicnost3[0], ritmicnost3[1], ritmicnost3[2],
    utjecaj_vecine,
    trajanje_osvete
  };

};

struct igrac
{
  strategija s;
  int id;
};

struct populacija
{
  vector<igrac*> obicni_igraci, mutanti;

  igrac* random_izvuci_igraca() { return obicni_igraci[rand() % obicni_igraci.size()]; // zasad samo obicni }
};


int main()
{
  cout << "Hello, World!" << endl;
  return 0;
}