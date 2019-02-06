<?php
    session_start();
    require_once(__DIR__."/php/myPHP.php");
    if(get_browser_name($_SERVER['HTTP_USER_AGENT']) == "Internet Explorer")
        header('Location: ALERT.html');
    $P =  new myPage("Jan Brzeżański - Edukacja");
    $P->addCSS("index.css");
    $P->addJS("img.js");
    $P->setDescription("Przygody z edukacją. Pierwszy projekt na kursie o Nowoczesnych Technologiach WWW.");
    echo $P->Begin();
    echo $P->PageHeader();
    echo $P->About("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque vulputate iaculis ligula, a condimentum purus tempus in. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Nam congue massa risus, et ornare magna euismod eget. Donec vitae mauris risus. In malesuada egestas mi, ac aliquet risus rhoncus quis. Cras a dui metus. Nam viverra posuere augue in congue. Integer sollicitudin ultricies erat, hendrerit dapibus lacus varius in. Nunc sed sagittis nibh. Maecenas eu orci facilisis nunc hendrerit lobortis amet.");
    echo $P->Column("left","./data/Studia.txt");
    echo $P->Column("right","./data/Hobby.txt");
    echo $P->OpinionsLink("Opinie");
    echo GetVisitCounter("./data/Count.txt");
    echo $P->End();
?>