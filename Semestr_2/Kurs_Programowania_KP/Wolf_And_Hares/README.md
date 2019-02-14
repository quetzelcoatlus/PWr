Ze wszystkich programów tego kursu, ten zdecydowanie jest moim ulubionym. Jest to symulacja, w której podaje się rozmiar planszy, opóźnienie tury, liczbę wilków oraz liczbę zajęcy. Ideą programu jest wielowątkowość, więc każdy wilk i każdy zając są osobnymi wątkami (dla dużych danych komputer może się męczyć). Symulacja przebiega następująco: na planszy pojawiają się wilki i zające. Zające chcą przeżyć, więc uciekają od najbliższego wilka, a wilki chcą jeść, więc biegną do najbliższego zająca. Jeżeli wilk zje zająca, to bodajże przez parę tur musi odpocząć. Niestety trudno tu o happy end, ale wygląda to dość absorbująco. 

Żeby uruchomić wystarczy bodajże:  
$ javac *.java ; java Main
