<?php
    require_once(__DIR__."/php/myPHP.php");
    if(get_browser_name($_SERVER['HTTP_USER_AGENT']) == "Internet Explorer")
        header('Location: ALERT.html');
    $P =  new myPage("Semestr I");
    $P->addCSS("sem.css");
    $P->setDescription("Kursy semestru pierwszego studiów.");
    echo $P->Begin();
    echo $P->Mathjax();
    echo $P->Back("index.php");
    $dir = basename(__FILE__);
    echo $P->Dropdown("./data/Studia.txt",$dir);
    echo $P->Dropdown("./data/Hobby.txt",$dir);
    echo $P->Box("Semestr 1");
    echo $P->Subject("Algebra z geometrią analityczną", ["Macierzy","Pierscieni","Grup"], ["Homomorfizmu", "Liczb zespolonych", "Ciał"], ["Wspaniały wykładowca", "Dobrze spędzony czas", "Najwyższa ocena"],"$$ NWD(a, n) = 1 \\Rightarrow a^{\phi (n)} \\equiv _{n} 1 $$");
    echo $P->Subject("Analiza matematyczna 1",["Pochodnych","Funkcji","Granic"],["Podciągiów","Szeregów","Całek"],["Spokojny","Monotonny","Nieciekawy"],"$$  \\frac{d}{dx} \int\limits_{a}^{x} f(t) dt = f(x) $$");
    echo $P->Subject("Logika i struktury formalne",["Kwantyfikatorów","Zbiorów","Rachunku zdań"],["Dowodzenia","Indukcji","Tautologii"],["Super","Ekstra","Fajny"],"$$ \binom{n}{k} = \binom{n-1}{k-1} + \binom{n-1}{k} $$");
    echo $P->Subject("Wstęp do informatyki i programowania",["Podstaw języka C","Podstaw programowania"],["Złożoności obliczeniowej","Rekurencji"],["Interesujący","Łatwy"]);
    echo $P->End();
?>