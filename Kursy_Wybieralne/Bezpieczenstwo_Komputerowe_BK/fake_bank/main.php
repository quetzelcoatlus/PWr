<?php
    require_once(__DIR__."/php/all.php");
    if(!isset($_SESSION["login"])){
        header("Location: index.php");
        die();
    }
    echo begin_page("Konto", "Konto użytkownika banku");
    if ($_SESSION["login"] == "admin")
        echo admin_page();   
    else
        echo main_page();
    echo end_page();
?>