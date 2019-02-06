<?php
    require_once(__DIR__."/php/myPHP.php");
    if(get_browser_name($_SERVER['HTTP_USER_AGENT']) == "Internet Explorer")
        header('Location: ALERT.html');
    $P =  new myPage("Matematyka");
    $P->addCSS("hob.css");
    $P->setDescription("Moja przygoda z matematyką");
    echo $P->Begin();
    echo $P->Back("index.php");
    $dir = basename(__FILE__);
    echo $P->Dropdown("./data/Studia.txt",$dir);
    echo $P->Dropdown("./data/Hobby.txt",$dir);
    echo $P->Box("Matematyka");
    echo $P->HobbyBox("Jak to się zaczęło?","Lorem ipsum dolor sit amet, consectetur adipiscing elit. In id enim non turpis iaculis mollis. Vestibulum dapibus ut massa ut ullamcorper. In hac habitasse platea dictumst. Curabitur dictum luctus quam, sit amet vestibulum lacus tempor in. Phasellus suscipit viverra orci, eu dictum nisi egestas vitae. Integer ultrices sem justo, eget porttitor lorem vulputate aliquam. Ut ultrices enim vitae ante maximus malesuada quis id est. Vestibulum nisl leo, accumsan vel mollis sed.");
    echo $P->HobbyBox("Co lubię najbardziej?","W matematyce najbardziej lubię niestandardowe rozwiązania. Fakt, że jest wiele dróg rozwiązań, z czego każda może być dobra, ale niektóre są bardziej wysublimowane, sprawia, że rozwiązywanie zadania może być swoistą sztuką, w której autor może się wykazać.");
    echo $P->End();
?>