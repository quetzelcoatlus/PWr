<?php
    require_once(__DIR__."/php/all.php");
    if(!isset($_SESSION["login"]) || $_SESSION["login"] != "admin"){
        header("Location: index.php");
        die();
    }
    $db = new dataBase();
    if(isset($_GET["l"]) && isset($_GET["d"])){
        $db->realize($_GET["l"], $_GET["d"]);        
    }
    echo begin_page("Zrealizuj przelew", "Możliwość realizacji przelewu przez administratora");
    
    $data = $db->get_realizes();
    echo realize_page($data);
    echo end_page();
?>