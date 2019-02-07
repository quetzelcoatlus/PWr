Żeby działał SQLite trzeba wejść do XAMPPA i:
1) Przycisk Config przy Apache -> PHP (php.ini)
2) Szukamy wszystkie występienia sqlite i:
  a) odkomentowujemy "extension=sqlite3" (usuwamy średnik)
  b) odkomentowujemy "sqlite3.extension_dir =" i przypisujemy mu ścieżkę absolutną do "php_sqlite3.dll". Tę bibliotekę umieściłem w folderze db. 
3) Powinno działać
