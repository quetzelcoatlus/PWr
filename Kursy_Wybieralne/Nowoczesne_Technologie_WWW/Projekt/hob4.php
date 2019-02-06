<?php
    require_once(__DIR__."/php/myPHP.php");
    if(get_browser_name($_SERVER['HTTP_USER_AGENT']) == "Internet Explorer")
        header('Location: ALERT.html');
    $P =  new myPage("Gry komputerowe");
    $P->addCSS("hob.css");
    $P->addJS("game.js");
    $P->setDescription("Moja przygoda z grami komputerowymi");
    echo $P->Begin();
    echo $P->Back("index.php");
    $dir = basename(__FILE__);
    echo $P->Dropdown("./data/Studia.txt",$dir);
    echo $P->Dropdown("./data/Hobby.txt",$dir);
    echo $P->Box("Gry komputerowe");
    echo $P->HobbyBox("Snake","Lubię grać w gry i również sam je tworzyć. Poniżej można przetestować prostą grę - Snake w WebGL");
    echo '        <div id="divCanvas">
            <canvas id="canvasId" width="370" height="370" style="border:0px; margin: 0px; margin-top:10px; padding: 0px;" >
                Your browser does not support the canvas element.
            </canvas>
        </div>';   
    echo $P->End();
?>