#include "strukture.h"
#include "beaufils.h"
#include <fstream>

#define PRINTER(name) printer(#name)

string printer(string name) {
	return name;
}

void tit_for_tat_s_kompom()
{
	igrac protivnik{ new klasicna_strategija(bfs::tit_for_tat), 77 };

	stanje st{ &protivnik };

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

void nagradi(parametrizirana_strategija::mutacija& mut, int n) {
	for (int i = 0; i < n; ++i)
	{
		mut.primjena();
	}
};

Int_Generator *gen = new Int_Generator(0, 1);

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

const int koliko_pokusaja = 1;
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

int jas = 0, jan = 0, onis = 0, onin = 0;

void kolo(int iteracija, ostream &file)
{
	igrac *i = pop.random_izvuci_igraca();

	test_rnd.s_kime_trebam_igrati = test_tft.s_kime_trebam_igrati = test_grad.s_kime_trebam_igrati = i;
	if (test_tft.povijest_za_trenutnog().size() == 0)
		test_tft.osvjezi(akcija::s, i->potez(test_tft));
	else
		test_tft.osvjezi(nti_korak(test_tft, -1), i->potez(test_tft));
	test_rnd.osvjezi(gen->Produce() ? akcija::s : akcija::n, i->potez(test_rnd));

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
		test_grad_cnt = izbroji_akcije(test_grad, akcija::n) + 1;
		akc = akcija::n;
	}
	else
		akc = akcija::s;
	test_grad.osvjezi(akc, i->potez(test_grad));


	auto &okr = okruzenja[0];
	{
		parametrizirana_strategija *moja_strategija = okr.moja_strategija;
		igrac *ja = okr.ja;
		stanje &st = okr.st;
		parametrizirana_strategija::mutacija &prethodna_mut = okr.prethodna_mut;


		st.s_kime_trebam_igrati = i;

		auto ishod = st.osvjezi(ja->potez(st), i->potez(st));
		if (ishod.first == akcija::s) ++jas; else ++jan;
		if (ishod.second == akcija::s) ++onis; else ++onin;

	}

	file << iteracija + 1 << ";" << okruzenja[0].st.uspjesnost()
		<< ";" << test_tft.uspjesnost()
		<< ";" << test_rnd.uspjesnost()
		<< ";" << test_grad.uspjesnost()
		<< ";" << jas << ";" << jan << ";" << onis << ";" << onin << ";\n";
}
int main()
{

	ofstream file;
	file.open("uspjesnost-kroz-treniranje.csv");
	file << "iteracija;" << "mi;" << "tft;" << "random;" << "gradual;" << "mi-s;" << "mi-n;" << "oni-s;" << "oni-n;\n";

	for (int iteracija = 0; iteracija < 1000; ++iteracija)
	{

		bool mutiranje = iteracija % 2 == 0;
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
		test_rnd.osvjezi(gen->Produce() ? akcija::s : akcija::n, i->potez(test_rnd));

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
			test_grad_cnt = izbroji_akcije(test_grad, akcija::n) + 1;
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
      if (ishod.first == akcija::s) ++jas; else ++jan;
      if (ishod.second == akcija::s) ++onis; else ++onin;
			// cout << "[ja: " << ishod.first << ", suigrac:" << ishod.second << "]\n";

			if (primjena_mutacije && ishod.first == akcija::n && ishod.second == akcija::s)
			{
				nagradi(prethodna_mut, 5);
			}
			else if (primjena_mutacije && ishod.first == akcija::s && ishod.second == akcija::s)
			{
				nagradi(prethodna_mut, 2);
			}
			else if (primjena_mutacije && ishod.first == akcija::n && ishod.second == akcija::n)
			{
				//nagradi(prethodna_mut, 1);
				prethodna_mut.anti_primjena();
			}
			else if (primjena_mutacije && ishod.first == akcija::s && ishod.second == akcija::n)
			{
				//nagradi(prethodna_mut, 0);
				prethodna_mut.anti_primjena();
			}
			//if (primjena_mutacije)
			//{
			//  if (ishod.second == akcija::s)
			//  {
			//    prethodna_mut.primjena();
			//    prethodna_mut.primjena();

			//    if (ishod.first == akcija::n)
			//    {
			//      prethodna_mut.primjena();
			//      prethodna_mut.primjena();
			//    }
			//  }
			//  else
			//  {
			//    prethodna_mut.anti_primjena();
			//  }
			//}


		}

		if (mutiranje)
		{
			//sort(okruzenja.begin(), okruzenja.end(), [&](const okruzenje& a, const okruzenje& b)
			//{
			//  return a.st.uspjesnost() > b.st.uspjesnost();
			//});

			// //pretp: 500 pokusaja
			//okruzenje cuvanje[] = {okruzenja[0], okruzenja[1], okruzenja[2], okruzenja[3], okruzenja[4]};
			//for (int i = 0; i < koliko_pokusaja; ++i)
			//{
			//  int pretinac = i / 10;

			//  okruzenje &okr = okruzenja[i];
			//  //if (iteracija % 10 == 0)
			//    okr.moja_strategija->vektor_tezina = cuvanje[pretinac].moja_strategija->vektor_tezina;

			//  parametrizirana_strategija::mutacija mut(* okr.moja_strategija);
			//  mut.primjena();
			//  okr.prethodna_mut = mut.kljucne_mutacije();
			//}
			parametrizirana_strategija::mutacija mut(*okruzenja[0].moja_strategija);
			mut.primjena();
			okruzenja[0].prethodna_mut = mut.kljucne_mutacije();
		}


		
		file << iteracija + 1 << ";" << okruzenja[0].st.uspjesnost()
			<< ";" << test_tft.uspjesnost()
			<< ";" << test_rnd.uspjesnost()
			<< ";" << test_grad.uspjesnost()
			<< ";" << jas << ";" << jan << ";" << onis << ";" << onin << ";\n";

	}

	file.close();

	for (auto &okruzenje : okruzenja)
	{
		cout << okruzenje.st.uspjesnost() << endl;
	}

	/*  sort(okruzenja.begin(), okruzenja.end(), [&](const okruzenje& a, const okruzenje& b)
	  {
		return a.st.uspjesnost() > b.st.uspjesnost();
	  })*/;

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




	// --- evaluacija (testiranje) ---

	test_tft = stanje(), test_rnd = stanje(), test_grad = stanje();
	test_grad_cnt = 0;
	jas = jan = onis = onin = 0;

	file.open("uspjesnost-kroz-testiranje.csv");
	file << "iteracija;" << "mi;" << "tft;" << "random;" << "gradual;" << "mi-s;" << "mi-n;" << "oni-s;" << "oni-n;\n";

	okruzenja[0].st.povijest = map<int, vector<interakcija>>();
	for (int iteracija = 0; iteracija < 1000; ++iteracija)
	{
		kolo(iteracija, file);
	}

	file.close();

	// evaluacija za svakog od igraca

	int j = 1;
	for each (igrac *i in pop.obicni_igraci)
	{
		test_tft = stanje(), test_rnd = stanje(), test_grad = stanje();
		okruzenja[0].st.povijest = map<int, vector<interakcija>>();

		test_grad_cnt = 0;
		jas = jan = onis = onin = 0;

		stringstream name;
		name << "igrac_" << j++ << ".csv";

		file.open(name.str());
		file << "iteracija;" << "mi;" << "tft;" << "random;" << "gradual;" << "mi-s;" << "mi-n;" << "oni-s;" << "oni-n;\n";

		for (int iteracija = 0; iteracija < 1000; ++iteracija)
		{
			kolo(iteracija, file);
		}
		file.close();
	}


	return 0;
}