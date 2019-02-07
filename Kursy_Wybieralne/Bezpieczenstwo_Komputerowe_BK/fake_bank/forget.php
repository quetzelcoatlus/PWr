<?php
    require_once(__DIR__."/php/all.php");
    $error="";
    if(isset($_POST["submitted"])){
        $db = new dataBase();
        if(!isset($_POST["login"]))
            $error.="Brak dostępu; ";
        if($_POST["login"]==="")
            $error.="Pola nie mogą być puste; ";
        if($error === ""){      
            $question = $db->forget($_POST["login"]);
            if ($question != ""){
                $_SESSION['login']=$_POST["login"];
                $_SESSION['question']=$question;
                header("Location: change.php");
                die();
            }
        }
    }

    echo begin_page("Zapomniałem hasła", "Resetowanie hasła użytkownika");
    if($error != "")
        echo "Błąd: ".$error;
    echo forget_page();
    echo end_page();
?>