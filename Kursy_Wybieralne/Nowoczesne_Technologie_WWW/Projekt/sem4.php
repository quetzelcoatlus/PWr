<?php
    require_once(__DIR__."/php/myPHP.php");
    if(get_browser_name($_SERVER['HTTP_USER_AGENT']) == "Internet Explorer")
        header('Location: ALERT.html');
    $P =  new myPage("Semestr IV");
    $P->addCSS("sem.css");
    $P->setDescription("Kursy semestru czwartego studiów.");
    echo $P->Begin();
    echo $P->Mathjax();
    echo $P->Back("index.php");
    $dir = basename(__FILE__);
    echo $P->Dropdown("./data/Studia.txt",$dir);
    echo $P->Dropdown("./data/Hobby.txt",$dir);
    echo $P->Box("Semestr 4");
    echo $P->Subject("Technologie sieciowe",["Ping","Traceroute"],["Sieci","Grafów"],["Fajny wykładowca","Pozytywnie"]);
    echo $P->Subject("Algorytmy i struktury danych",["Sortowania","Drzew"],["Więcej sortowań","Złożoności obliczeniowej"],["Wymagający","Ciekawy"]);
    echo $P->Subject("Nowoczesne Technologie WWW",["Podstaw HTML","Podstaw CSS"],["PHP","Wybierania ładnych kolorów"],["Przyjemny","Ciekawy"]);
    echo $P->Subject("Programowanie w logice",["Rekurencja ogonowa"],["Wątków"],["Interesujący"]);
    echo $P->Subject("Wprowadzenie do teorii grafów",["Grafów","Spójności"],["Składowych","Grafów eulerowskich"],["Neutralnie", "Dużo pisania"],"$$ |S| = |E| + |V | + 2 $$");
    echo $P->End();
?>