<?php
    require_once(__DIR__."/php/all.php");
    if(!isset($_SESSION["login"])){
        header("Location: index.php");
        die();
    }
    $error="";
    if(isset($_POST["submitted"])){
        if(!isset($_POST["number"])  || !isset($_POST["amount"]) || !isset($_POST["title"]))
            $error.="Brak dostępu; ";
        if($_POST["number"]==="" || $_POST["amount"]==="" || $_POST["title"]==="")
            $error.="Pola nie mogą być puste; ";
        if($error === ""){      
            $_SESSION['number']=$_POST["number"];
            $_SESSION['amount']=$_POST["amount"];
            $_SESSION['title']=$_POST["title"];
            header("Location: confirm.php");
            die();
        }
    }
    echo begin_page("Przelew", "Wykonywanie przelewu");
    if($error != "")
            echo "Błąd: ".$error;
    echo transfer_page();
    echo end_page();
?>