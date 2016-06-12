#include "strukture.h"
#include "beaufils.h"

void tit_for_tat_s_kompom()
{
  igrac protivnik{new klasicna_strategija(bfs::tit_for_tat), 77};

  stanje st{&protivnik};

  do
  {
    cout << "Potez (s/n)? ";
    string odgs; cin >> odgs;
    auto odg = odgs == "s" ? akcija::s : akcija::n;
    auto prot_odg = protivnik.potez(st);

    cout << "vi: "
    << (odg == akcija::s ? "suradnja" : "nesuradnja")
    << endl << "protivnik: "
    << (prot_odg == akcija::s ? "suradnja" : "nesuradnja")
    << endl;

    st.povijest[protivnik.id].push_back(make_pair(odg, prot_odg));

  } while (true);
};

int main()
{
  populacija pop = klasicna_populacija({
    bfs::all_c, bfs::all_d,
    bfs::gradual,
    bfs::gradual_killer,
    bfs::hard_majo,
    bfs::hard_tf2t,
    bfs::hard_tft,
    bfs::mistrust,
    bfs::per_ccd,
    bfs::per_ddc,
    bfs::prober,
    bfs::soft_majo,
    bfs::soft_tf2t,
    bfs::spiteful,
    bfs::tit_for_tat
  }, 2);

  auto moja_strategija = new parametrizirana_strategija();
  igrac* ja = new igrac{ moja_strategija };

  stanje st;

  parametrizirana_strategija::mutacija prethodna_mut(*moja_strategija);
  for (int iteracija = 0; iteracija < 1000; ++iteracija)
  {

    igrac* i = pop.random_izvuci_igraca();
    st.s_kime_trebam_igrati = i;

    auto ishod = st.osvjezi(ja->potez(st), i->potez(st));
    if (ishod.second == akcija::s)
    {
      prethodna_mut.primjena();
    }

    parametrizirana_strategija::mutacija mut(*moja_strategija);
    mut.primjena();
    prethodna_mut = mut.kljucne_mutacije();

    for (auto& val : moja_strategija->vektor_tezina)
    {
      cout << val << " ";
    }
    cout << endl << "(" << iteracija << ")" << endl;

    /*
     *
     * ovdje treba pozvati izmjenu strategije ovisno o ishodu
     *
     */
  }



  cout << "Hello, World!" << endl;
  return 0;
}