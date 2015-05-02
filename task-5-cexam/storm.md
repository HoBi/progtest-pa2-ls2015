    map<string, CStudent*> cards;
    map<unsigned int, CStudent*> students;
    map<string, set<unsigned int>> registered;
    map<string, vector<CResult*>> resolved;
    map<string, vector<CResult*>> missing;

( 1 ) LOAD                      [|||---]

 - vytvo��m studenta, nasypu jeho karty do cards a nalinkuju

( 2 ) REGISTER                  [||----]

 - najdu si studenta podle karty
 - vlo��m ho do registeredStudents podle testu
 - vlo��m do missing pr�zdn� result podle testu

( 3 ) ASSESS                    [||----]

 - najdu test v missing ( nesm� failnout )
 - p�i�ad�m result, vyhodim z missing a p�id�m do resolved na konec

( 4 ) LISTTEST                  [||----]

 - mrknu do pole resolved test� podle testu a registered seznamu
 - pokud to chce sortit, naklonuju a sesortim, else vyhodim obsah tak jak je

( 5 ) LISTMISSING               [|||---]

 - mrknu do pole missing test� podle testu a registered seznamu
 - vyplivu v�echny, kte�� tam maj missing test


 # jak zrychlit
  - cresult vytv��et a� v assess

  # co mo�e by� blbje
  - na��t�n� vstupu ( no jak jinak )
  - cancelling loadu kdy� se najde pr�zdn� karta nebo je count v�t�� jak nula ( tam se mo�n� m��e st�t, �e je n�kdy pr�zdn� )