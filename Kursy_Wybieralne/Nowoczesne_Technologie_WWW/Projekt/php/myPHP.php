<?php
$HEADER =<<<EOT
<!DOCTYPE html>
<html lang="pl-PL">
<head>
    <meta charset="utf-8">
    <title>{{TITLE}}</title>
    <meta name="description" content= "{{DESCRIPTION}}">
    <meta name="author" content="Jan Brzeżański">
    <meta name="viewport" content = "width=device-width, initial-scale=1.0"/>
{{STYLES}}
{{SCRIPTS}}
</head>
<body>
    <div class="back">

EOT;

$MATHJAX =<<<EOT
        <script type="text/x-mathjax-config">
            MathJax.Hub.Config({
                  tex2jax: {inlineMath: [['$','$'], ['\\(','\\)']]}, 
                  CommonHTML: { linebreaks: { automatic: true } }, 
                  "HTML-CSS": { linebreaks: { automatic: true } }, 
                  SVG: { linebreaks: { automatic: true } } 
            })
        </script>
        <script async 
        src="https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.4/MathJax.js?config=TeX-AMS-MML_HTMLorMML">
        </script>

EOT;

$PAGE_HEADER =<<<EOT
        <header class="head">
            <h1>Jan Brzeżański</h1>
            <h2>moje przygody z edukacją</h2>
            {{SUBTITLE}}
        </header>

EOT;

$FOOTER =<<<EOT
    </div>
</body>
</html>
EOT;


$ABOUT =<<<EOT
        <div class=about>
            <img class=serafis id="myimage" alt="Serafis">
            <p class=text> {{text}} </p>
        </div>

EOT;

$COLUMN =<<<EOT
        <nav class={{column}}>
            <div class=title>{{title}}</div>
            <ul>
{{links}}
            </ul>
        </nav>

EOT;


$DROPDOWN =<<<EOT
        <div class="dropdown">
            <button class="dropbtn">{{title}}</button>
            <nav class="dropdown-content">
{{links}}
            </nav>
        </div>

EOT;


$SUBJECT =<<<EOT
            <div class="box">
                <h2>{{title}}</h2>
                <div class="thing">
                    <div class="title">Czego się nauczyłem?</div>
                    <ul>
{{answers1}}
                    </ul>
                </div>
                <div class="thing">
                    <div class="title">Czego muszę się douczyć?</div>
                    <ul>
{{answers2}}
                    </ul>
                </div>
                <div class="thing">
                    <div class="title">Jak oceniam kurs?</div>
                    <ul>
{{answers3}}
                    </ul>
                </div>
                {{wzorek}}
            </div>

EOT;

$HOBBYBOX =<<<EOT
        <div class="box">
            <div class="title"> <h2>{{title}}</h2> </div>
            <p>{{text}}</p>
        </div>

EOT;

$OPINIONS_LINK=<<<EOT
        <div class="head">
            <h2><a href="opi.php"> {{NAME}} </a></h2>
        </div>

EOT;

$FORM=<<<EOT
         <div class="box">
            <form method="post" action="add.php" id="form">
                <label>Nick</label> <br/>
                <input type="text" spellcheck="false" maxlength="20" name="nick" value="{{NICK}}">
                <br/>
                <label>Opinia</label> <br/>
                <textarea name="opinion" spellcheck="false" maxlength="250" rows=9 cols=80 >{{OPINION}}</textarea>
                <br/>
                <img src="image.php" alt="CAPTCHA" width="200" height="50"> <br/>
                <label style="vertical-align: top;">Wynik </label> <br>
                <input style="vertical-align: top;" spellcheck="false" maxlength="2" type="text" name="result">
                <br/>
                <input class="linkbut" type="submit" value="Dodaj opinię">
        </div>

EOT;


$OPINIONS=<<<EOT
        <div class="box">
{{OPINIONS}}
        </div>

EOT;

$OPINION=<<<EOT
            <div class="opinion">
                <label class="left">{{NICK}}</label>
                <label class="right">{{DATE}}</label>
                <br/>
                <pre>{{OPINION}}</pre>
            </div>

EOT;
/**
* Klasa sluzaca do generowania stron ustalonej witryny
* @package myPage
* @author Jacek Cichon
* @author Jan Brzeżański
*/
class myPage {
    private $title          = "";
    private $description    = "";
    private $root           = "";
    private $cssfiles       = [];
    private $jsfiles        = [];
    private $toAlignSingle  = [];
    private $toAlignDouble  = [];

    /**
    * Dodaje specyficzne style do strony
    * @param string $filename
    * @return void
    */
    public function AddCSS($filename) {
        $this->cssfiles[] = $filename;
    }

    /**
    * Ustalamy skrypty umieszczane w nagłówku
    * @param string $filename
    * @return void
    */
    public function AddJS($filename) {
        $this->jsfiles[] = $filename;
    }

    /**
    * Ustawienie opisu strony (description)
    * @param string $s
    * @return void
    */
    public function SetDescription($s) {
        $this->description = $s;
    }

    /**
    * Ustawienie opisu strony
    * @param string $title - tytul strony
    * @param string $root - sciezka do glownego katalogu witryny
    * @return void
    */
    public function __construct($title = "") {
        $this->title = $title;
        $this->AddCSS("reset.css");
    }

    /**
    * Zwraca lancuch z poczatkiem strony
    * @return string
    */
    public function Begin() {
        global $HEADER;
        $s = str_replace(["{{TITLE}}", "{{DESCRIPTION}}"], [$this->title, $this->description], $HEADER);

        //dodajemy style
        $X = [];
        $C = $this->cssfiles;
        $TMP = '    <link rel="stylesheet" href="./css/{{CSS}}">' . "\n";
        for ($i = 0; $i < count($C); $i++){
          $X[]= (string) str_replace("{{CSS}}", (string) $C[$i], $TMP);
        }
        $s= str_replace("{{STYLES}}", join("\n",$X), $s);

        // dodajemy skrypty
        $X = [];
        $C = $this->jsfiles;
        $T = '    <script src="./js/{{JS}}"></script>' . "\n";
        for ($i = 0; $i < count($C); $i++){
          $X[]= str_replace("{{JS}}", (string) $C[$i], $T);
        }
        $s= str_replace("{{SCRIPTS}}", join("\n",$X), $s);

        // usuwamy puste linie
        //return preg_replace('/^[ \t]*[\r\n]+/m', '', $s);
        return preg_replace('/^\h*\v+/m', '', $s);
        // \h : dowolny poziomy pusty znak
        // \v : dowolny pionowy pusty znak
        // /m : multiline
    }

    
    public function Mathjax(){
        global $MATHJAX;
        return $MATHJAX;
    }
    
    /**
    * Zwraca lancuch z naglowkiem strony
    * @return string
    */
    public function PageHeader($subtitle=""){
        global $PAGE_HEADER;
        $s = str_replace("{{SUBTITLE}}", $subtitle, $PAGE_HEADER);
        return preg_replace('/^\h*\v+/m', '', $s);
    }


    /**
    * Zwraca lancuch z zamknieciem strony
    * @return string
    */
    public function End() {
        global $FOOTER;
        return preg_replace('/^\h*\v+/m', '', $FOOTER);
    }
    
    /**
    * Zwraca lancuch z informacją o mnie
    * @param string $source - adres obrazka
    * @param string $text - tekst
    * @return string
    */
    public function About($source="", $text=""){
        global $ABOUT;
        $s = str_replace(["{{text}}"], [$source,$text], $ABOUT);
        return preg_replace('/^\h*\v+/m', '', $s);
    }

    /**
    * Zwraca lancuch z kolumną z linkami
    * @param string $column - lewa lub prawa kolumna
    * @param string $file - plik, z ktorego czytamy menu
    * @return string
    */
    public function Column($column="", $file=""){
        global $COLUMN;
        $lines = file($file);
        $size = count($lines);
        for($j=0; $j<$size; $j++)
            $lines[$j] = str_replace("\r\n", "", $lines[$j]);
        $s = str_replace(["{{column}}","{{title}}"], [$column, (string) $lines[0]], $COLUMN);
        $X = [];
        $T = '              <li><a href="{{link}}">{{desc}}</a></li>'."\n";
        for($i=1; $i<$size; $i = $i+2)
            $X[]= str_replace(["{{link}}", "{{desc}}"], [(string) $lines[$i], (string) $lines[$i+1]], $T);
        $s2 = str_replace("{{links}}", join("\n",$X), $s);
        return preg_replace('/^\h*\v+/m', '', $s2);
    }


    /**
    * Zwraca lancuch z przyciskiem powrotu
    * @param string $des - dokad wracamy
    * @return string
    */
    public function Back($des=""){
        $s='        <a href='.$des.' class="linkbut">Powrót</a>'."\n";
        return preg_replace('/^\h*\v+/m', '', $s);      
    }
    
    /**
    * Zwraca lancuch z rozwijanym menu
    * @param string $file - plik z menu
    * @param string $dir - obecna sciezka
    * @return string
    */
    public function Dropdown($file="",$dir=""){
        global $DROPDOWN;
        $lines = file($file);
        $size = count($lines);
        for($j=0; $j<$size; $j++)
            $lines[$j] = str_replace("\r\n", "", $lines[$j]);
        $s = str_replace(["{{title}}"], (string) $lines[0], $DROPDOWN);
        $X = [];
        $T = '                <a href="{{link}}">{{desc}}</a>'."\n";
        $T2 = '                <a href="#">{{desc}}</a>'."\n";
        for($i=1; $i<$size; $i = $i+2){
            if ($dir === $lines[$i])
                $X[]= str_replace("{{desc}}", (string) $lines[$i+1], $T2);
            else
                $X[]= str_replace(["{{link}}", "{{desc}}"], [(string) $lines[$i], (string) $lines[$i+1]], $T);
        }
        $s2 = str_replace("{{links}}", join("\n",$X), $s);
        return preg_replace('/^\h*\v+/m', '', $s2);
    }

    
    /**
    * Zwraca lancuch z prostym pudelkiem
    * @param string $text - tekst pudelka
    * @return string
    */
    public function Box($text=""){
        $s = "        <div class='box'><h1>".$text."</h1></div>\n";
        return preg_replace('/^\h*\v+/m', '', $s);
    }

    
    /**
    * Zwraca lancuch z pudelkiem przedmiotu
    * @param string $title - nazwa przedmiotu
    * @param array of strings $answers1...$answers3 - tablice z danymi
    * @return string
    */
    public function Subject($title="",$answers1=[],$answers2=[],$answers3=[],$wzorek=""){
        global $SUBJECT;
        $s1 = str_replace("{{title}}",$title,$SUBJECT);
        $size1=count($answers1);
        $A1 = [];
        $T = '                        <li>{{text}}</li>'."\n";
        for($i = 0; $i < $size1; $i++)
            $A1[] = str_replace("{{text}}",$answers1[$i],$T);
        $s2 = str_replace("{{answers1}}", join("\n",$A1),$s1);

        $size2=count($answers2);
        $A2 = [];
        for($j = 0; $j < $size2; $j++)
            $A2[] = str_replace("{{text}}",$answers2[$j],$T);
        $s3 = str_replace("{{answers2}}", join("\n",$A2),$s2);

        $size3=count($answers3);
        $A3 = [];
        for($k = 0; $k < $size3; $k++)
            $A3[] = str_replace("{{text}}",$answers3[$k],$T);
        $s4 = str_replace("{{answers3}}", join("\n",$A3),$s3);
        
        if($wzorek=="")
            $s5 = str_replace("{{wzorek}}","",$s4);
        else
            $s5 = str_replace("{{wzorek}}",'<div class="tablica">'.$wzorek.'</div>',$s4);
        
        return preg_replace('/^\h*\v+/m', '', $s5);
    } 

    /**
    * Zwraca lancuch z prostym pudelkiem z tytulem
    * @param string $title - tytul pudelka
    * @param string $text - tekst pudelka
    * @return string
    */
    public function HobbyBox($title="",$text=""){
        global $HOBBYBOX;
        $s = str_replace(["{{title}}","{{text}}"], [$title,$text],$HOBBYBOX);
        return preg_replace('/^\h*\v+/m', '', $s) . "\n";
    }
    
    
    public function OpinionsLink($name="Opinie") {
        global $OPINIONS_LINK;
        $s = str_replace(["{{NAME}}"], (string) $name, $OPINIONS_LINK);
        return preg_replace('/^\h*\v+/m', '', $s);
    }
    
    public function Form() {
        global $FORM;
        if(isset($_SESSION['nick']))
            $s1 = str_replace("{{NICK}}",$_SESSION['nick'],$FORM);
        else
            $s1 = str_replace("{{NICK}}","",$FORM);
        if(isset($_SESSION['opinion']))
            $s2 = str_replace("{{OPINION}}",$_SESSION['opinion'],$s1);
        else
            $s2 = str_replace("{{OPINION}}","",$s1);
        return $s2;
    }
    
    public function OPINIONS(){
        global $OPINIONS;
        global $OPINION;
        $db = new MyDB();
        $ret = $db->query("SELECT * FROM OPINIONS;");

        $X = [];
        while($row = $ret->fetchArray(SQLITE3_ASSOC) ) {
            $X[] = str_replace(["{{NICK}}","{{DATE}}","{{OPINION}}"], [htmlentities((string) $row['NICK']), htmlentities((string) $row['DATE']), htmlentities((string) $row['OPINION'])], $OPINION);
        }
        $s2 = str_replace("{{OPINIONS}}", join("\n",$X), $OPINIONS);
        $db->close();
        return preg_replace('/^\h*\v+/m', '', $s2);
    }
    
}

class MyDB extends SQLite3 {
    function __construct() {
        $this->open('./db/opinions.db');
    }
}

   
   
function get_browser_name($user_agent) {
    if (strpos($user_agent, 'Opera') || strpos($user_agent, 'OPR/')) return 'Opera';
    elseif (strpos($user_agent, 'Edge')) return 'Microsoft Edge';
    elseif (strpos($user_agent, 'Chrome')) return 'Google Chrome';
    elseif (strpos($user_agent, 'Safari')) return 'Safari';
    elseif (strpos($user_agent, 'Firefox')) return 'Mozilla Firefox';
    elseif (strpos($user_agent, 'MSIE') || strpos($user_agent, 'Trident/7')) return 'Internet Explorer';
    
    return 'Other';
}  
  
/**
* Zwraca licznik unikatowych odwiedzin
* @param string $file - adres pliku z licznikiem
* @return string
*/
function GetVisitCounter($file) {
    $count = 0;
    try {
        if (file_exists($file)) {
            $fp = fopen($file, "r");
            $i = fgets($fp, 10);  //max 10 znaków
            if ($i === NULL){
                $i = "0";
            }
            $count = intval($i);
            fclose($fp);
            if(!isset($_SESSION['hasVisited'])){
                $_SESSION['hasVisited']="yes";
                $count=  $count + 1 ;
            }
        }
        $fp = fopen($file,"w");
        fwrite($fp, $count);
        fclose($fp);
    } catch(Exception $e) {
        echo 'Wyjątek: ',  $e->getMessage(), "\n";
    }
   /* if(isset($_SESSION['a']))
        return "        <p style='margin: 10px 10px 0px 10px; text-align:left; font-size:12px; font-weight: bold;'>Licznik odwiedzajacych: ".$count."\n".$_SESSION['a']." ".$_SESSION['b']." ".$_SESSION['c']." ".$_SESSION['s1']." ".$_SESSION['s2']."</p>\n";
    else*/
    return "        <p style='margin: 10px 10px 0px 10px; text-align:left; font-size:12px; font-weight: bold;'>Licznik odwiedzajacych: ".$count."<br>Przeglądarka: ".get_browser_name($_SERVER['HTTP_USER_AGENT'])."</p>\n";
}
?>
