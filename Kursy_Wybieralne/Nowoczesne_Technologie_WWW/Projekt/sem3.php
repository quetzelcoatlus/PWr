<?php
    require_once(__DIR__."/php/myPHP.php");
    if(get_browser_name($_SERVER['HTTP_USER_AGENT']) == "Internet Explorer")
        header('Location: ALERT.html');
    $P =  new myPage("Semestr III");
    $P->addCSS("sem.css");
    $P->setDescription("Kursy semestru trzeciego studiów.");
    echo $P->Begin();
    echo $P->Mathjax();
    echo $P->Back("index.php");
    $dir = basename(__FILE__);
    echo $P->Dropdown("./data/Studia.txt",$dir);
    echo $P->Dropdown("./data/Hobby.txt",$dir);
    echo $P->Box("Semestr 3");
    echo $P->Subject("Technologie programowania",["Dziedziczenia","XML"],["Wzorców projektowych","Wyrażeń regularnych"],["Pozytywnie","Przeciętnie"]);
    echo $P->Subject("Bazy danych i systemy operacyjne",["SQL","JDBC"],["Algebry relacji","Transakcji"],["Wymagający","Ciekawy"]);
    echo $P->Subject("Architektura komputerów i systemy operacyjne",["Linux","Język C"],["Asemblera","Teorii"],["Treściwy","Ciekawy"]);
    echo $P->Subject("Metody probabilistyczne i statystyka",["Probabilistyki"],["Rozkładów"],["Źle prowadzony"]);
    echo $P->End();
?>