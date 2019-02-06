<?php
    require_once(__DIR__."/php/myPHP.php");
    if(get_browser_name($_SERVER['HTTP_USER_AGENT']) == "Internet Explorer")
        header('Location: ALERT.html');
    $P =  new myPage("Semestr II");
    $P->addCSS("sem.css");
    $P->setDescription("Kursy semestru drugiego studiów.");
    echo $P->Begin();
    echo $P->Mathjax();
    echo $P->Back("index.php");
    $dir = basename(__FILE__);
    echo $P->Dropdown("./data/Studia.txt",$dir);
    echo $P->Dropdown("./data/Hobby.txt",$dir);
    echo $P->Box("Semestr 2");
    echo $P->Subject("Algebra abstrakcyjna i kodowanie", ["Macierzy","Kodowania"], ["Kodowania", "Liczb zespolonych"], ["Wspaniały wykładowca","Najwyższa ocena"],"$$ e^{\pi i} + 1 = 0 $$");
    echo $P->Subject("Analiza matematyczna 2",["Pochodnych","Funkcji","Granic"],["Podciągiów","Szeregów","Całek"],["Spokojny","Monotonny","Nieciekawy"],"$$ \int\limits_{-\infty}^{+\infty} e^{-x^2} dx = \sqrt{\pi} $$");
    echo $P->Subject("Matematyka Dyskretna",["Symbolu Newtona","Liczb Stirlinga"],["Funkcji tworzącej","Fajnych triczków"],["Przyjemny","Zabawny"],"$$ F _{n} = \\frac{1}{\\sqrt{5}} \Bigg[\bigg( \\frac{1 + \\sqrt{5}}{2} \bigg)^{n} - \bigg(\\frac{1 - \\sqrt{5}}{2} \bigg)^{n} \Bigg]  $$");
    echo $P->Subject("Fizyka",["Pisania eseji","Rzutu ukośnego"],["Teorii względności","Elektryczności"],["Słabo","Nudno"],"$$ E=mc^2 $$");
    echo $P->Subject("Kurs programowania",["Wątków","GUI"],["OOP","Fajnych bibliotek"],["Pozytywnie","Przeciętnie"]);
    echo $P->Subject("Problemy prawne informatyki",["Pisać umowy","Pisać licencje"],["Kruczków prawnych","Niebezpieczeństw"],["Pozytywnie","Inspirująco"]);
    echo $P->End();
?>