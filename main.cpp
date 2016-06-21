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
  srand(314);

  populacija pop = klasicna_populacija({
    bfs::all_c, bfs::all_d,
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
  }, 1);

  const int koliko_pokusaja = 500;
  struct okruzenje
  {
    parametrizirana_strategija* moja_strategija;
    igrac* ja;
    stanje st;
    parametrizirana_strategija::mutacija prethodna_mut; //, pretprethodna_mut;

    okruzenje() : moja_strategija(new parametrizirana_strategija()),
                  ja(new igrac{ moja_strategija }),
                  prethodna_mut(*moja_strategija)
                  // , pretprethodna_mut(*moja_strategija)
    {

    }

  };

  array<okruzenje, koliko_pokusaja> okruzenja;

  stanje test_tft, test_rnd, test_grad;
  int test_grad_cnt = 0;


  for (int iteracija = 0; iteracija < 1000; ++iteracija)
  {

    bool mutiranje         = iteracija % 2 == 0;
    bool primjena_mutacije = mutiranje && (iteracija > 0);
    //mutiranje = primjena_mutacije = false;


    /*
     * 0. korak: potez(st), st = mut(st)
     * 1. korak: potez(st) - ignoriramo ishod jer mutacija nije stigla djelovat
     * 2. korak: potez(st); reakcija_dobra ? primjeni mutaciju : nemoj; st = mut(st);
     * 3. korak: potez(st) - ignoriramo ishod jer mutacija nije stigla djelovat
     *   ...
     */

    igrac *i = pop.random_izvuci_igraca();

    test_rnd.s_kime_trebam_igrati = test_tft.s_kime_trebam_igrati = test_grad.s_kime_trebam_igrati = i;
    if (test_tft.povijest_za_trenutnog().size() == 0)
      test_tft.osvjezi(akcija::s, i->potez(test_tft));
    else
      test_tft.osvjezi(nti_korak(test_tft, -1), i->potez(test_tft));
    test_rnd.osvjezi(rand() % 2 ? akcija::s : akcija::n, i->potez(test_rnd));

    akcija akc(akcija::s); // za gradual
    if (test_grad.povijest_za_trenutnog().size() == 0)
    {
      test_grad_cnt = 0;
    }
    else if ((test_grad_cnt == 1) || (test_grad_cnt == 2))
    {
      test_grad_cnt--;
      akc = akcija::s;
    }
    else if (test_grad_cnt > 2)
    {
      test_grad_cnt--;
      akc = akcija::n;
    }
    else if (nti_korak(test_grad, -1) == akcija::n)
    {
      test_grad_cnt = max(5, izbroji_akcije(test_grad, akcija::n) + 1);
      akc = akcija::n;
    }
    else
      akc = akcija::s;
    test_grad.osvjezi(akc, i->potez(test_grad));


    for (auto &okr : okruzenja)
    {
      parametrizirana_strategija *moja_strategija = okr.moja_strategija;
      igrac *ja = okr.ja;
      stanje &st = okr.st;
      parametrizirana_strategija::mutacija &prethodna_mut = okr.prethodna_mut;


      st.s_kime_trebam_igrati = i;

      auto ishod = st.osvjezi(ja->potez(st), i->potez(st));
     // cout << "[ja: " << ishod.first << ", suigrac:" << ishod.second << "]\n";


      if (primjena_mutacije)
      {
        if (ishod.second == akcija::s)
        {
          prethodna_mut.primjena();
          prethodna_mut.primjena();

          if (ishod.first == akcija::n)
          {
            prethodna_mut.primjena();
            prethodna_mut.primjena();
          }
        }
        else
        {
          prethodna_mut.anti_primjena();

        }
      }

      /*
      parametrizirana_strategija::mutacija mut(*moja_strategija);
      mut.primjena();
      prethodna_mut = mut.kljucne_mutacije();*/

      /*for (auto& val : moja_strategija->vektor_tezina)
      {
        cout << val << " ";
      }*/
    /*  cout << "inic_sluc: " << *moja_strategija->inicijalna_slucajnost() << " "
      << "pos0: " << *moja_strategija->reakcija_za_posljednje_korake() << " "
      << "pos1: " << *(moja_strategija->reakcija_za_posljednje_korake() + 1) << " "
      << "pos2: " << *(moja_strategija->reakcija_za_posljednje_korake() + 2) << " "
      << "prvi: " << *moja_strategija->reakcija_na_prvi() << " "
      << "osvet: " << *moja_strategija->osvetoljubivost() << " "
      << "aosvet: " << *moja_strategija->anti_osvetoljubivost() << " "
      << "r2: " << *moja_strategija->ritmicnost2() << " "
      << "r2': " << *(moja_strategija->ritmicnost2() + 1) << " "
      << "r3: " << *moja_strategija->inicijalna_slucajnost() << " "
      << "r3': " << *(moja_strategija->inicijalna_slucajnost() + 1) << " "
      << "r3'': " << *(moja_strategija->inicijalna_slucajnost() + 2) << " "
      << "vec: " << *moja_strategija->inicijalna_slucajnost() << " "
      << "trajosv: " << *moja_strategija->inicijalna_slucajnost() << " "
      << endl; */
    }

    if (mutiranje)
    {
      sort(okruzenja.begin(), okruzenja.end(), [&](const okruzenje& a, const okruzenje& b)
      {
        return a.st.uspjesnost() > b.st.uspjesnost();
      });

      // pretp: 500 pokusaja
      okruzenje cuvanje[] = {okruzenja[0], okruzenja[1], okruzenja[2], okruzenja[3], okruzenja[4]};
      for (int i = 0; i < koliko_pokusaja; ++i)
      {
        int pretinac = i / 100;

        okruzenje &okr = okruzenja[i];
        //if (iteracija % 4 == 0)
        okr.moja_strategija->vektor_tezina = cuvanje[pretinac].moja_strategija->vektor_tezina;

        parametrizirana_strategija::mutacija mut(* okr.moja_strategija);
        mut.primjena();
        okr.prethodna_mut = mut.kljucne_mutacije();
      }
    }



    cout << "iteracija " << iteracija + 1 << "\t: " << okruzenja[0].st.uspjesnost()
         << "\ttft: " << test_tft.uspjesnost()
         << "\trandom: " << test_rnd.uspjesnost()
         << "\tgradual: " << test_grad.uspjesnost() << endl;

  }

  for (auto &okruzenje : okruzenja)
  {
    cout << okruzenje.st.uspjesnost() << endl;
  }

  sort(okruzenja.begin(), okruzenja.end(), [&](const okruzenje& a, const okruzenje& b)
  {
    return a.st.uspjesnost() > b.st.uspjesnost();
  });

  auto *moja_strategija = okruzenja[0].moja_strategija;
  cout << "inic_sluc: " << *moja_strategija->inicijalna_slucajnost() << " "
    << "pos0: " << *moja_strategija->reakcija_za_posljednje_korake() << " "
    << "pos1: " << *(moja_strategija->reakcija_za_posljednje_korake() + 1) << " "
    << "pos2: " << *(moja_strategija->reakcija_za_posljednje_korake() + 2) << " "
    << "prvi: " << *moja_strategija->reakcija_na_prvi() << " "
    << "osvet: " << *moja_strategija->osvetoljubivost() << " "
    << "aosvet: " << *moja_strategija->anti_osvetoljubivost() << " "
    << "r2: " << *moja_strategija->ritmicnost2() << " "
    << "r2': " << *(moja_strategija->ritmicnost2() + 1) << " "
    << "r3: " << *moja_strategija->ritmicnost3() << " "
    << "r3': " << *(moja_strategija->ritmicnost3() + 1) << " "
    << "r3'': " << *(moja_strategija->ritmicnost3() + 2) << " "
    << "vec: " << *moja_strategija->utjecaj_vecine() << " "
    << "trajosv: " << *moja_strategija->trajanje_osvete() << " "
    << endl;



  return 0;
}