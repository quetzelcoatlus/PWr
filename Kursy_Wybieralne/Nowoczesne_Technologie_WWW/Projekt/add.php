<?php 
session_start();
require_once(__DIR__."/php/myPHP.php");

$nick = $opinion = $result = "";

$CREATE_TABLE =<<<EOF
    CREATE TABLE IF NOT EXISTS OPINIONS
    (DATE TEXT NOT NULL,
    NICK TEXT NOT NULL,
    OPINION TEXT NOT NULL);
EOF;

function dziel($a, $b, $base){
    while($a % $b != 0){
        $a+=$base;
    }
    return ($a/$b)%$base;
}
    
function policz($a, $b, $s,$base){
    if($s == "+")
        return ($a+$b)%$base;
    if($s == "-"){
        $r = $a-$b;
        if ($r < 0)
            $r += $base;
        return $r;
    }
    if($s == "*")
        return ($a*$b)%$base;
    
    return dziel($a,$b,$base);
}

function calculate(){
    $a = (int) $_SESSION['a'];
    $b = (int) $_SESSION['b'];
    $c = (int) $_SESSION['c'];
    $s1 = (string) $_SESSION['s1'];
    $s2 = (string) $_SESSION['s2'];
    $base = (int) $_SESSION['base'];
    $d = policz($b,$c, $s1,$base);
    $e = policz($a,$d,$s2,$base);
    unset($_SESSION['a']);
    unset($_SESSION['b']);
    unset($_SESSION['c']);
    unset($_SESSION['s1']);
    unset($_SESSION['s2']);
    unset($_SESSION['base']);
    return $e;
}

function check(){
    if(isset($_POST["nick"]) && isset($_POST["opinion"]) && isset($_POST["result"]) ){
        $_SESSION['nick'] = $_POST["nick"];
        $_SESSION['opinion'] = $_POST["opinion"];
        
        if($_POST["nick"] =="" || $_POST["opinion"] =="" || $_POST["result"] =="")
            return 1;
        $nick = $_POST["nick"];
        $opinion = $_POST["opinion"];
        $result = (int) $_POST["result"];
        //echo $_POST["result"]." ".$result;
        unset($_POST["nick"]);
        unset($_POST["opinion"]);
        unset($_POST["result"]);
        if(!isset($_SESSION['a']) || !isset($_SESSION['b']) || !isset($_SESSION['c']) || !isset($_SESSION['s1']) || !isset($_SESSION['s2']) || !isset($_SESSION['base']))
            return 2;
        $res = calculate();
        
        //echo $res." ".$result;
        if($res != $result)
            return 3;
        unset($_SESSION["nick"]);
        unset($_SESSION["opinion"]);
        global $CREATE_TABLE;
        $db = new MyDB();
        if(!$db)
            return 4;
        //$db -> exec("DROP TABLE IF EXISTS OPINIONS;");
        $ret = $db->exec($CREATE_TABLE);
        if(!$ret)
            return 5;
        $date = date('G:i:s, \A\D d-m-Y');
        $insert = "INSERT INTO OPINIONS (DATE, NICK, OPINION) VALUES('".$date."','".$nick."','".$opinion."');";
        $ret = $db->exec($insert);
        if(!$ret)
            return 6;
        $db->close();
        return 0;
    }
    return 7;
}

$P =  new myPage("Dodanie opinii");
$P->addCSS("add.css");
$P->setDescription("Dodanie opinii dotyczącej tego portalu");
echo $P->Begin();
$b = check();
if($b == 0)
   echo $P->Box("Poprawnie dodano opinię.");
else if($b == 1)
    echo $P->Box("Pola muszą być wypełnione.");
else if($b == 2)
    echo $P->Box("CAPTCHA nie została wygenerowana");
else if($b == 3)
    echo $P->Box("Błędny wynik wyrażenia.");
else if($b == 4)
    echo $P->Box("Nie udało się połączyć z bazą danych.");
else if($b == 5)
    echo $P->Box("Nie udało się stworzyć tabeli.");
else if($b == 6)
    echo $P->Box("Nie udało się wstawić rekordu do tabeli.");
else if($b == 7)
    echo $P->Box("Proszę zaprzestać włamywania się na stronę.");

echo "<br/>";
echo $P->Back("opi.php#form");
echo $P->End();
?>