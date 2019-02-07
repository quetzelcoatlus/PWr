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
    echo begin_page("Potwierdzenie danych", "Potwierdzenie danych przelewu");
    echo confirm_page();
    echo end_page();
?>