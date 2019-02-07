<?php
    require_once(__DIR__."/php/all.php");
    if(!isset($_SESSION["login"])){
        header("Location: index.php");
        die();
    }
   if(!isset($_SESSION['number']) || !isset($_SESSION['amount']) || !isset($_SESSION['title'])){
        header("Location: main.php");
        die();
    }
    $db = new dataBase();   
    $db->add_transfer($_SESSION['login'], $_SESSION['number'], $_SESSION['amount'], $_SESSION['title']);


    echo begin_page("Potwierdzenie przelewu", "Potwierdzenie przelewu");
    echo send_page();
    echo end_page();
    unset($_SESSION['number']);
    unset($_SESSION['amount']);  
    unset($_SESSION['title']);
?>