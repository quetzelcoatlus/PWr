¯eby dzia³a³ SQLite trzeba wejœæ do XAMPPA i:
1) Przycisk Config przy Apache -> PHP (php.ini)
2) szukamy wszystkie wyst¹pienia sqlite i:
  a) odkomentowujemy "extension=sqlite3" (usuwamy œrednik)
  b) odkomentowujemy "sqlite3.extension_dir =" i przypisujemy mu œcie¿kê absolutn¹ do "php_sqlite3.dll". Tê bibliotekê umieœci³em w folderze db. Ja osobiœcie wrzuci³em sobie j¹ do "C:/xampp/htdocs", wiêc moja linijka w php.ini wygl¹da tak: "sqlite3.extension_dir = "C:/xampp/htdocs""
3) i w sumie powinno dzia³aæ