#include "definitions.h"
#include "beaufils.h"

int main()
{
  igrac protivnik{new klasicna_strategija(tit_for_tat), 77};

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



  cout << "Hello, World!" << endl;
  return 0;
}