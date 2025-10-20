#ifndef NAMEDAYS_PL_H
#define NAMEDAYS_PL_H
// clang-format off

/*-- Tabela imienin -- Imiona dla każdego dnia roku rozdzielone przecinkami.
Jeśli w danym dniu występuje wiele imion, należy pozostawić między nimi spację, a
program będzie je zamieniał co 4 sekundy.
*/

const char* nameday_label = "Imieniny:";

  // Tablica imienin - imiona oddzielone przecinkami na każdy dzień roku (rotacja co 4 sekundy) - źródło: kalendarzswiat.pl
const char* namedays[] PROGMEM = {
  // Styczeń (31 dni)
  "Mieczysława,Mieszka", "Izydora,Makarego", "Danuty,Genowefy", "Anieli,Eugeniusza", "Edwarda,Szymona", "Kacpra,Melchiora,Baltazara", "Juliana,Lucjana", "Seweryna,Teofila", "Weroniki,Juliana", "Jana,Wilhelma",
  "Matyldy,Honoraty", "Benedykta,Arkadiusza", "Weroniki,Bogumiły", "Feliksa,Hilarego", "Pawła,Izydora", "Marcelego,Włodzimierza", "Antoniego,Rościsława", "Piotra,Małgorzaty", "Henryka,Mariusza", "Fabiana,Sebastiana",
  "Agnieszki,Jarosława", "Anastazego", "Ildefonsa,Rajmunda", "Felicji,Tymoteusza", "Pawła,Miłosza", "Seweryna,Pauliny", "Jana,Przybysława", "Walerego,Radomira", "Zdzisława,Franciszka", "Macieja,Martyny", "Jana,Marceliny",
  // Luty (29 dni - rok przestępny)
  "Brygidy,Ignacego", "Marii,Mirosława", "Błażeja,Hipolita", "Andrzeja,Weroniki", "Agaty,Adelajdy", "Doroty,Tytusa", "Ryszarda,Romualda", "Jana,Piotra", "Cyryla,Apolonii", "Jacka,Scholastyki",
  "Łazarza,Marii", "Eulalii,Modesta", "Grzegorza,Katarzyny", "Walentego,Metodego", "Faustyna,Józefa", "Danuty,Juliany", "Donata,Łukasza", "Symeona,Konstancji", "Konrada,Arnolda", "Leona,Ludomiła",
  "Eleonory,Feliksa", "Marty,Małgorzaty", "Romany,Damiana", "Macieja,Bogusza", "Wiktora,Cezarego", "Mirosława,Aleksandra", "Gabriela,Anastazji", "Teofila,Makarego", "Hilarego,Oswałda",
  // Marzec (31 dni)
  "Antoniny,Radosława", "Heleny,Pawła", "Tycjana,Kunegundy", "Kazimierza,Łucji", "Fryderyka,Wacława", "Róży,Wiktora", "Pawła,Tomasza", "Beaty", "Katarzyny,Franciszki", "Cypriana,Marcelego",
  "Konstantego,Benedykta", "Bernarda,Grzegorza", "Bożeny,Krystyny", "Leona,Matyldy", "Ludwiki,Klemensa", "Izabeli,Hilarego", "Zbigniewa,Patryka", "Cyryla,Edwarda", "Józefa,Bogdana", "Eufemii,Klaudii",
  "Benedykta,Lubomira", "Bogusława,Katarzyny", "Feliksa,Pelagii", "Marka,Gabriela", "Marii,Wieńczysława", "Teodora,Emanuela", "Lidii,Ernesta", "Anieli,Sykstusa", "Wiktora,Eustachego", "Amelii,Jana", "Balbiny,Gwidona",
  // Kwiecień (30 dni)
  "Zbigniewa,Grażyny", "Franciszka,Władysława", "Ryszarda,Pankracego", "Wacława,Izydora", "Ireny,Wincentego", "Celestyna,Wilhelma", "Donata,Rufina", "Dionizego,Januarego", "Marii,Marcelego", "Michała,Makarego",
  "Leona,Filipa", "Juliusza,Wiktora", "Przemysława,Hermenegildy", "Justyny,Waleriana", "Anastazji,Bazylego", "Julii,Benedykta", "Roberta,Patrycego", "Bogusławy,Bogumiły", "Adolfa,Tymona", "Czesława,Agnieszki",
  "Feliksa,Anzelma", "Leona,Łukasza", "Jerzego,Wojciecha", "Grzegorza,Aleksandra", "Marka,Jarosława", "Marii,Marcelego", "Zyty,Teofila", "Pawła,Walerii", "Piotra,Pawła", "Mariana,Katarzyny",
  // Maj (31 dni)
  "Józefa,Filipa", "Anatola,Zygmunta", "Marii,Aleksandra", "Moniki,Floriana", "Ireny,Waldemara", "Jana,Judyty", "Ludmiły,Gizeli", "Stanisława,Dezyderii", "Bożydara,Grzegorza", "Izydora,Antoniny",
  "Franciszka,Jakuba", "Dominika,Pankracego", "Roberta,Serwacego", "Bonifacego,Dobiesława", "Zofii,Jana", "Andrzeja,Wieńczysława", "Weroniki,Sławomira", "Feliksa,Aleksandry", "Piotra,Mikołaja", "Bernarda,Bazylego",
  "Wiktora,Tymoteusza", "Julii,Heleny", "Iwony,Dezyderego", "Joanny,Zuzanny", "Urbana,Grzegorza", "Filipa,Pauliny", "Jana,Juliusza", "Augustyna,Jaromira", "Teodozji,Magdaleny", "Feliksa,Ferdynanda", "Anieli,Petroneli",
  // Czerwiec (30 dni)
  "Jakuba,Konrada", "Erazma,Marianny", "Leszka,Klotyldy", "Karola,Franciszka", "Walerii,Bonifacego", "Pauliny,Laury", "Roberta,Wiesława", "Maksyma,Medarda", "Pelagii,Felicjana", "Bogumiła,Małgorzaty",
  "Barnaby,Feliksa", "Jana,Onufrego", "Lucjana,Antoniego", "Walerego,Bazylego", "Wita,Jolanty", "Aliny,Justyny", "Laury,Adolfa", "Marka,Elżbiety", "Gerwazego,Protazego", "Bogny,Florentyny",
  "Alicji,Alojzego", "Pauliny,Flawiusza", "Wandy,Zenona", "Jana,Danuty", "Łucji,Wilhelma", "Jana,Pawła", "Marii,Władysława", "Leona,Ireneusza", "Piotra,Pawła", "Emilii,Lucyny",
  // Lipiec (31 dni)
  "Haliny,Mariana", "Marii,Urbana", "Jacka,Anatola", "Teodora,Innocentego", "Karoliny,Antoniego", "Łucji,Dominika", "Cyryla,Metodego", "Elżbiety,Prokopa", "Zenona,Weroniki", "Filipa,Amelii",
  "Olgi,Pelagii", "Jana,Gwalberta", "Ernesta,Małgorzaty", "Marceliny,Bonawentury", "Henryka,Włodzimierza", "Marii,Benedykta", "Bogdana,Aleksego", "Kamila,Szymona", "Wincentego,Wodzisława", "Czesława,Hieronima",
  "Daniela,Andrzeja", "Magdaleny,Bolesława", "Bogny,Apolinarego", "Kingi,Krystyny", "Jakuba,Krzysztofa", "Anny,Mirosławy", "Julii,Natalii", "Wiktora,Innocentego", "Marty,Olafa", "Julity,Ludmiły", "Ignacego,Heleny",
  // Sierpień (31 dni)
  "Piotra,Justyny", "Gustawa,Alfonsa", "Lidii,Augusta", "Dominika,Protazego", "Marii,Stanisławy", "Sławy,Jakuba", "Doroty,Kajetana", "Emila,Cyryla", "Romana,Romualda", "Borysa,Wawrzyńca",
  "Zuzanny,Filomeny", "Klary,Hilarego", "Hipolita,Diany", "Alfreda,Euzebiusza", "Marii,Napoleona", "Rocha,Joachima", "Jacka,Mirona", "Heleny,Bronisławy", "Bolsława,Juliana", "Bernarda,Sobiesława",
  "Joanny,Franciszki", "Cezarego,Tymoteusza", "Filipa,Apolinarego", "Jerzego,Bartłomieja", "Ludwika,Luizy", "Marii,Zefiryny", "Józefa,Moniki", "Augustyna,Patrycji", "Sabiny,Jana", "Rózy,Szczęsnego", "Bogdana,Rajmunda",
  // Wrzesień (30 dni)
  "Bronisława,Idziego", "Stefana,Juliana", "Izabeli,Szymona", "Rozalii,Róży", "Doroty,Wawrzyńca", "Beaty,Eugeniusza", "Reginy,Melchiora", "Marii,Adrianny", "Piotra,Mikołaja", "Bernarda,Sobiesława",
  "Jacka,Piotra", "Marii,Gwidona", "Filipa,Eugenii", "Cypriana,Bernarda", "Albina,Nikodema", "Edyty,Kornela", "Justyna,Franciszki", "Ireny,Józefa", "Januarego,Konstancji", "Filipiny,Eustachego",
  "Hipolita,Mateusza", "Tomasza,Maurycego", "Tekli,Bogusława", "Gerarda,Teodora", "Aurelii,Ładysława", "Justyny,Cypriana", "Kosmy,Damiana", "Marka,Wacława", "Michała,Michaliny", "Zofii,Hieronima",
  // Październik (31 dni)
  "Danuty,Remigiusza", "Teofila,Dionizego", "Gerarda,Teresy", "Rozalii,Franciszka", "Apolinarego,Placyda", "Artura,Brunona", "Marii,Marka", "Pelagii,Brygidy", "Ludwika,Dionizego", "Pauliny,Franciszka",
  "Emila,Aldony", "Eustachego,Maksymiliana", "Edwarda,Teofila", "Bernarda,Fortunaty", "Teresy,Jadwigi", "Gawła,Ambrożego", "Wiktora,Małgorzaty", "Łukasza,Juliana", "Piotra,Ziemowita", "Ireny,Kleopatry",
  "Urszuli,Hilarego", "Filipa,Kordulii", "Teodora,Seweryna", "Rafała,Marcina", "Kryspina,Ingi", "Lucjana,Ewarysta", "Sabiny,Iwony", "Szymona,Tadeusza", "Euzebii,Narcyza", "Zenobii,Przemysława", "Urbana,Augusta",
  // Listopad (30 dni)
  "Seweryna,Wiktoryny", "Bohdana,Bożydara", "Sylwii,Huberta", "Karola,Olgierda", "Sławomira,Elżbiety", "Feliksa,Leonarda", "Antoniego,Ernesta", "Sewera,Gotfryda", "Ursyna,Teodora", "Andrzeja,Ludomira",
  "Bartłomieja,Marcina", "Renaty,Witolda", "Stanisława,Mikołaja", "Serafina,Rogera", "Alberta,Leopolda", "Gertrudy,Edmunda", "Grzegorza,Salomei", "Anieli,Romana", "Elżbiety,Seweryna", "Feliksa,Anatola",
  "Janusza,Konrada", "Marka,Cecylii", "Klemensa,Amelii", "Jana,Flory", "Erazma,Katarzyny", "Konrada,Sylwestra", "Waleriana,Maksymiliana", "Grzegorza,Zdzisława", "Błażeja,Saturnina", "Andrzeja,Konstantego",
  // Grudzień (31 dni)
  "Natalii,Eligiusza", "Pauliny,Balbiny", "Franciszka,Ksawerego", "Barbary,Piotra", "Kryspina,Saby", "Mikołaja,Emiliana", "Marcina,Ambrożego", "Marii,Wirgiliusza", "Wiesławy,Leokadii", "Julii,Daniela",
  "Damazego,Waldemara", "Adelajdy,Aleksandra", "Łucji,Otylii", "Alfreda,Izydora", "Celiny,Waleriana", "Euzebiusza,Zdzisławy", "Olimpii,Łazarza", "Gracjana,Bogusława", "Urbana,Dariusza", "Bogumiła,Dominika",
  "Tomasza,Tomisława", "Zenona,Honoraty", "Wiktorii,Sławomiry", "Adama,Ewy", "Eugenii,Anastazji", "Dionizego,Szczepana", "Kosmy,Damiana", "Cezarego,Teofila", "Dawida,Tomasza", "Eugeniusza,Sabiny", "Sylwestra,Sebastiana"
};


#endif // NAMEDAYS_PL_H
// clang-format on