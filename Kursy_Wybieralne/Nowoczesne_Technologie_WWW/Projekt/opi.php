<?php
    session_start();
    require_once(__DIR__."/php/myPHP.php");
    $P =  new myPage("Opinie");
    $P->addCSS("opi.css");
    $P->setDescription("Opinie na temat strony internetowej");
    echo $P->Begin();
    echo $P->Back("index.php");
    $dir = basename(__FILE__);
    echo $P->Dropdown("./data/Studia.txt",$dir);
    echo $P->Dropdown("./data/Hobby.txt",$dir);
    echo '<div class="box"><a href="#form" class="linkbut" style="margin:10px 0px;">Wprowadź opinię</a></div>';
    echo $P->Opinions();
    echo $P->Form();
    echo $P->End();
?>