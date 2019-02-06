<?php
    require_once(__DIR__."/php/myPHP.php");
    if(get_browser_name($_SERVER['HTTP_USER_AGENT']) == "Internet Explorer")
        header('Location: ALERT.html');
    $P =  new myPage("Muzyka");
    $P->addCSS("hob.css");
    $P->setDescription("Moja przygoda z muzyką");
    echo $P->Begin();
    echo $P->Back("index.php");
    $dir = basename(__FILE__);
    echo $P->Dropdown("./data/Studia.txt",$dir);
    echo $P->Dropdown("./data/Hobby.txt",$dir);
    echo $P->Box("Muzyka");
    echo $P->HobbyBox("Jak to się zaczęło?","Muzyka jest dla mnie istotna od dziecka. Przez te ponad 20 lat mój gust bardzo się zmieniał i rozszerzał, jednak zawsze przywiązywałem sporą wagę do tego, czego w danej chwili słuchałem.");
    echo $P->HobbyBox("Co lubię najbardziej?","Muzyka pozwala mi się zrelaksować. Umila mi podróż autem oraz wycieczki rowerowe. Przy muzyce lepiej mi się pracuje, dlatego słucham jej przez większość wolnego czasu.");
    echo $P->End();
?>