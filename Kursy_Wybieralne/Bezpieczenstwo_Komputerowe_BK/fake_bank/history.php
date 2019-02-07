<?php
    require_once(__DIR__."/php/all.php");
    if(!isset($_SESSION["login"])){
        header("Location: index.php");
        die();
    }
    echo begin_page("Historia przelewów", "Wyświetlenie wykonanych przelewów");
    $db = new dataBase();
    $data = $db->get_history();
    echo history_page($data);
    echo end_page();
?>