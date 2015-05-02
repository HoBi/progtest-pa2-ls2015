    map<string, CStudent*> cards;
    map<unsigned int, CStudent*> students;
    map<string, set<unsigned int>> registered;
    map<string, vector<CResult*>> resolved;
    map<string, vector<CResult*>> missing;

( 1 ) LOAD                      [|||---]

 - vytvoøím studenta, nasypu jeho karty do cards a nalinkuju

( 2 ) REGISTER                  [||----]

 - najdu si studenta podle karty
 - vložím ho do registeredStudents podle testu
 - vložím do missing prázdný result podle testu

( 3 ) ASSESS                    [||----]

 - najdu test v missing ( nesmí failnout )
 - pøiøadím result, vyhodim z missing a pøidám do resolved na konec

( 4 ) LISTTEST                  [||----]

 - mrknu do pole resolved testù podle testu a registered seznamu
 - pokud to chce sortit, naklonuju a sesortim, else vyhodim obsah tak jak je

( 5 ) LISTMISSING               [|||---]

 - mrknu do pole missing testù podle testu a registered seznamu
 - vyplivu všechny, kteøí tam maj missing test


 # jak zrychlit
  - cresult vytváøet až v assess

  # co može byè blbje
  - naèítání vstupu ( no jak jinak )
  - cancelling loadu když se najde prázdná karta nebo je count vìtší jak nula ( tam se možná mùže stát, že je nìkdy prázdný )