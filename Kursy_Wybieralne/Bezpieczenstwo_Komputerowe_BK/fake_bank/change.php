<?php
    require_once(__DIR__."/php/all.php");
    $error="";
    if(isset($_POST["submitted"])){
        $db = new dataBase();
        if(!isset($_POST["answer"]) || !isset($_POST["pass"]))
            $error.="Brak dostępu; ";
        if($_POST["answer"]==="" || $_POST["pass"]==="" || $_POST["pass2"]==="")
            $error.="Pola nie mogą być puste; ";
        if($_POST["pass"] != $_POST["pass2"])
            $error.="Hasła muszą się zgadzać; ";
        
        if($error === ""){      
            $answer = $db->ask($_SESSION["login"], $_POST["answer"]);
            if ($answer === 1){
                $db->change_password($_SESSION["login"], $_POST["pass"]);
                header("Location: index.php");
                die();
            }
            else
                $error.="Zła odpowiedź; ";
        }
    }

    echo begin_page("Ustawienie nowego hasla", "Resetowanie hasła użytkownika");
    if($error != "")
        echo "Błąd: ".$error;
    echo change_page();
    echo end_page();
?>