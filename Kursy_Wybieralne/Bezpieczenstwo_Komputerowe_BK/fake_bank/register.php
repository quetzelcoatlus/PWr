<?php
    require_once(__DIR__."/php/all.php");

    $error="";
    if(isset($_POST["submitted"])){
        $db = new dataBase();   
        if(!isset($_POST["login"]) 
           || !isset($_POST["email"]) 
           || !isset($_POST["pass"]) 
           || !isset($_POST["pass2"]) 
           || !isset($_POST["question"]) 
           || !isset($_POST["answer"]))
            $error.="Brak dostępu; ";
        if($_POST["login"]===""
           || $_POST["email"]===""
           || $_POST["pass"]===""
           || $_POST["pass2"]===""
           || $_POST["question"]===""
           || $_POST["answer"]==="")
            $error.="Pola nie mogą być puste; ";
        if($_POST["pass"] != $_POST["pass2"])
            $error.="Hasła muszą się zgadzać; ";
		if(!ctype_alnum($_POST["login"]))
            $error.="Login musi zawierać tylko litery i cyfry; ";
        if($error === ""){
            $db->add_account($_POST["login"],$_POST["email"],$_POST["pass"],$_POST["question"],$_POST["answer"]);
            header("Location: index.php");
            die();
        }
    }

    echo begin_page("Rejestracja", "Zakladanie konta");
    if($error != "")
        echo "Błąd: ".$error;
    echo register_page();
    echo end_page();
?>