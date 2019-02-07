<?php
    require_once(__DIR__."/php/all.php");
    $error="";
    if(isset($_POST["submitted"])){
        $db = new dataBase();
        if(!isset($_POST["login"])  || !isset($_POST["pass"]) )
            $error.="Brak dostępu; ";
        if($_POST["login"]==="" || $_POST["pass"]==="")
            $error.="Pola nie mogą być puste; ";
        if($error === ""){      
            $log = $db->login($_POST["login"],$_POST["pass"]);
            if ($log === 0){
                $_SESSION['login']=$_POST["login"];
                header("Location: main.php");
                die();
            }
            else if($log === 1)
                $error .= "Błędne hasło; ";
            else if($log === 2)
                $error .= "Zły login; ";
        }
    }

    echo begin_page("Logowanie", "Logowanie do banku");
    if($error != "")
        echo "Błąd: ".$error;
    echo login_page();
    echo end_page();
?>