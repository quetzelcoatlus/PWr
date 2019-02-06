<?php
    require_once(__DIR__."/php/myPHP.php");
    if(get_browser_name($_SERVER['HTTP_USER_AGENT']) == "Internet Explorer")
        header('Location: ALERT.html');
    $P =  new myPage("Programowanie");
    $P->addCSS("hob.css");
    $P->setDescription("Moja przygoda z programowaniem");
    echo $P->Begin();
    echo $P->Back("index.php");
    $dir = basename(__FILE__);
    echo $P->Dropdown("./data/Studia.txt",$dir);
    echo $P->Dropdown("./data/Hobby.txt",$dir);
    echo $P->Box("Programowanie");
    echo $P->HobbyBox("Jak to się zaczęło?","Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam volutpat dictum ipsum, non cursus tortor laoreet non. Donec sit amet risus maximus, maximus nisl ut, pellentesque ipsum. Nulla pellentesque mi erat, sit amet iaculis magna facilisis sed. Proin felis nisi, mollis in dictum vitae, cursus malesuada nibh. Sed id mi sed dui viverra faucibus et id lorem. Nam vitae placerat ipsum. Nullam molestie, urna ac nullam.");
    echo $P->HobbyBox("Co lubię najbardziej?","Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas semper felis at fringilla aliquam. Duis sodales leo at urna varius posuere. Nunc eget elit semper, convallis lectus vitae, faucibus elit. Maecenas imperdiet tincidunt semper. Curabitur aliquam est a ante vehicula hendrerit. Mauris mollis sapien et metus tristique gravida in eleifend nisl. Curabitur ultrices, eros ac rutrum varius, ante leo tristique leo, ac tincidunt orci orci eu ex. Praesent sed nibh accumsan orci aliquam.");
    echo $P->End();
?>