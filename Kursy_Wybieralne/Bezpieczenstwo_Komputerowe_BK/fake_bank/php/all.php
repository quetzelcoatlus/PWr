<?php
session_start();

$HEADER =<<<EOT
<!DOCTYPE html>
<html lang="pl-PL">
<head>
    <meta charset="utf-8">
    <title>{{TITLE}}</title>
    <meta name="description" content= "{{DESCRIPTION}}">
    <meta name="author" content="Jan Brzeżański">
    <meta name="viewport" content = "width=device-width, initial-scale=1.0"/>
</head>
<body>
    <center>

EOT;

$HEADER_FAKE =<<<EOT
<!DOCTYPE html>
<html lang="pl-PL">
<head>
    <meta charset="utf-8">
    <title>{{TITLE}}</title>
    <meta name="description" content= "{{DESCRIPTION}}">
    <meta name="author" content="Jan Brzeżański">
    <meta name="viewport" content = "width=device-width, initial-scale=1.0"/>
</head>
<body onload="document.body.innerHTML = document.body.innerHTML.replace(/696969696969696/g, '99999');">
    <center>

EOT;


function begin_page($title="", $description="") {
    global $HEADER;
    return str_replace(["{{TITLE}}", "{{DESCRIPTION}}"], [$title, $description], $HEADER);
}


$FOOTER =<<<EOT

    </center>
</body>
</html>
EOT;

function end_page() {
    global $FOOTER;
    return $FOOTER;
}

$INDEX =<<<EOT
        <h1>Witaj w nBanku!</h1><br>
        <a href="login.php">Zaloguj się</a><br>
        <a href="register.php">Zarejestruj się</a>
EOT;

function index_page(){
    global $INDEX;
    return $INDEX;
}



$LOGIN =<<<EOT
        <h1>Logowanie</h1><br>
        <form action="login.php" method="post">
            Login:<br>
            <input type="text" name="login"><br>
            Haslo:<br>
            <input type="password" name="pass"><br><br>

            <input type="hidden" name="submitted" value="1">
            <input type="submit" value="Zaloguj">
            <button type="button" onclick="location.href='forget.php'">Zapomnialem hasła</button>
        </form>
        <br><br><br><button type="button" onclick="location.href='index.php'">Powrót do strony głównej</button>
EOT;

function login_page(){
    global $LOGIN;
    return $LOGIN;
}



$REGISTER =<<<EOT
        <h1>Rejestracja</h1><br>
        <form action="register.php" method="post">
            Podaj login:<br>
            <input type="text" name="login"><br>
            Podaj swój email:<br>
            <input type="email" name="email"><br>
            Podaj haslo:<br>
            <input type="password" name="pass"><br>
            Powtorz haslo:<br>
            <input type="password" name="pass2"><br>
            Pytanie kontrolne (w celu odzyskania hasła):<br>
            <input type="text" name="question"><br>
            Odpowiedź na powyższe pytanie:<br>
            <input type="text" name="answer"><br><br>   
                
            <input type="hidden" name="submitted" value="1">
            <input type="submit" value="Zarejestruj">
        </form>
        <br><br><br><button type="button" onclick="location.href='index.php'">Powrót do strony głównej</button>
EOT;

function register_page(){
    global $REGISTER;
    return $REGISTER;
}


$FORGET =<<<EOT
        <h1>Zapomniałem hasła</h1><br>
        <form action="forget.php" method="post">
            Podaj swój login:<br>
            <input type="text" name="login"><br>
                
            <input type="hidden" name="submitted" value="1">
            <input type="submit" value="Kontynuuj">
        </form>
        <br><br><br><button type="button" onclick="location.href='index.php'">Powrót do strony głównej</button>
EOT;

function forget_page(){
    global $FORGET;
    return $FORGET;
}

$CHANGE =<<<EOT
        <h1>Zmiana hasła</h1><br>
        <form action="change.php" method="post">
            Podaj odpowiedź na następujące pytanie:<br>
            <b>{{QUEST}}</b><br>
            <input type="text" name="answer"><br>
            
            Podaj nowe haslo:<br>
            <input type="password" name="pass"><br>
            Powtorz nowe haslo:<br>
            <input type="password" name="pass2"><br>   
                
            <input type="hidden" name="submitted" value="1">
            <input type="submit" value="Kontynuuj">
        </form>
        <br><br><br><button type="button" onclick="location.href='index.php'">Powrót do strony głównej</button>
EOT;

function change_page(){
    global $CHANGE;
    $s = str_replace("{{QUEST}}", $_SESSION['question'], $CHANGE);
    return $s;
}



$CREATE_TABLE_ACCOUNTS =<<<EOF
    CREATE TABLE IF NOT EXISTS ACCOUNTS (
        LOGIN TEXT NOT NULL,
        EMAIL TEXT NOT NULL,
        PASSWORD_HASH TEXT NOT NULL,
        QUESTION TEXT NOT NULL,
        ANSWER_HASH TEXT NOT NULL
    );
EOF;

$CREATE_TABLE_TRANSFERS =<<<EOF
    CREATE TABLE IF NOT EXISTS TRANSFERS (
        LOGIN TEXT NOT NULL,
        DATE TEXT NOT NULL,
        NUMBER TEXT NOT NULL,
        AMOUNT TEXT NOT NULL,
        TITLE TEXT NOT NULL,
        STATE TEXT NOT NULL
    );
EOF;

class dataBase {
    private $servername = "localhost";
    private $username = "root";
    private $password = "";
    private $connection;
    
    public function __construct() {
        $this->connection = mysqli_connect($this->servername, $this->username, $this->password);
        if (!$this->connection) {
            die("Connection failed: " . mysqli_connect_error());
        }
        
        if (!mysqli_query($this->connection, "CREATE DATABASE IF NOT EXISTS LISTA6;")) {
            echo "Error creating database: " . mysqli_error($this->connection);
        }
        if (!mysqli_query($this->connection, "USE LISTA6;")) {
            echo "Error using database: " . mysqli_error($this->connection);
        }
        
        global $CREATE_TABLE_ACCOUNTS;
        if (!mysqli_query($this->connection, $CREATE_TABLE_ACCOUNTS)) {
            echo "Error creating table accounts: " . mysqli_error($this->connection);
        }
        
        global $CREATE_TABLE_TRANSFERS;
        if (!mysqli_query($this->connection, $CREATE_TABLE_TRANSFERS)) {
            echo "Error creating table transfers: " . mysqli_error($this->connection);
        }
    }
    
    public function add_account($login, $email, $pass, $quest, $answer){
        $passHash = password_hash($pass, PASSWORD_DEFAULT);
        $answerHash = password_hash($answer, PASSWORD_DEFAULT);
        
        $add = "INSERT INTO ACCOUNTS (LOGIN, EMAIL, PASSWORD_HASH, QUESTION, ANSWER_HASH) VALUES('".$login."','".$email."','".$passHash."','".$quest."','".$answerHash."');";
        if (!mysqli_query($this->connection, $add)) {
            echo "Error adding account: " . mysqli_error($this->connection);
        }
    }
    
    public function login($login, $pass){
        $query = "SELECT PASSWORD_HASH FROM ACCOUNTS WHERE LOGIN='".$login."';";
        $result = mysqli_query($this->connection, $query);
        if (!$result) {
            echo "Error getting accounts: " . mysqli_error($this->connection);
        }
            
        if (mysqli_num_rows($result) > 0) {
            $row = mysqli_fetch_row($result);
            if(password_verify($pass, $row[0]))
               return 0;
            else 
               return 1;
        } else {
            return 2;
        }
    }
    
     public function add_transfer($login, $number, $amount, $title){
        $date = date('G:i:s, \A\D d-m-Y');
        $add = "INSERT INTO TRANSFERS (LOGIN, DATE, NUMBER, AMOUNT, TITLE, STATE) VALUES('".$login."','".$date."','".$number."','".$amount."','".$title."','DO REALIZACJI');";
        if (!mysqli_query($this->connection, $add)) {
            echo "Error adding transfer: " . mysqli_error($this->connection);
        }
    }
    
    public function get_history(){
        $query = "SELECT DATE, NUMBER, AMOUNT, TITLE, STATE FROM TRANSFERS WHERE LOGIN='".$_SESSION['login']."';";
        $result = mysqli_query($this->connection, $query);
        if (!$result) {
            echo "Error getting transfers: " . mysqli_error($this->connection);
        }
        $data=''; 
        if (mysqli_num_rows($result) > 0)
            while($row = mysqli_fetch_row($result)){
                $data.="            <tr>\n              <td>".htmlentities($row[0])."</td>\n             <td>".htmlentities($row[1])."</td>\n             <td>".htmlentities($row[2])."</td>\n             <td>".htmlentities($row[3])."</td>\n             <td>".htmlentities($row[4])."</td>\n           </tr>\n";
            }
        
        return $data;
    }
    
    public function get_all_history(){
        $query = "SELECT LOGIN, DATE, NUMBER, AMOUNT, TITLE, STATE FROM TRANSFERS;";
        $result = mysqli_query($this->connection, $query);
        if (!$result) {
            echo "Error getting transfers: " . mysqli_error($this->connection);
        }
        $data=''; 
        if (mysqli_num_rows($result) > 0)
            while($row = mysqli_fetch_row($result)){
                $data.="            <tr>\n              <td>".htmlentities($row[0])."</td>\n             <td>".htmlentities($row[1])."</td>\n             <td>".htmlentities($row[2])."</td>\n             <td>".htmlentities($row[3])."</td>\n             <td>".htmlentities($row[4])."</td>\n             <td>".htmlentities($row[5])."</td>\n           </tr>\n";
            }
        
        return $data;
    }
    
    
    public function get_realizes(){
        $query = "SELECT LOGIN, DATE, NUMBER, AMOUNT, TITLE FROM TRANSFERS WHERE STATE='DO REALIZACJI';";
        $result = mysqli_query($this->connection, $query);
        if (!$result) {
            echo "Error getting transfers: " . mysqli_error($this->connection);
        }
        $data=''; 
        if (mysqli_num_rows($result) > 0)
            while($row = mysqli_fetch_row($result)){
                $data.="            <tr>\n              <td>".htmlentities($row[0])."</td>\n             <td>".htmlentities($row[1])."</td>\n             <td>".htmlentities($row[2])."</td>\n             <td>".htmlentities($row[3])."</td>\n             <td>".$row[4]."</td>\n             <td><button type=".'"button" name="realize" onclick='."'location.href=".'"realize.php?l='.htmlentities($row[0])."&d=".htmlentities($row[1]).'"'."'".">Zrealizuj</button></td>\n           </tr>\n";
            }
        
        return $data;
    }
    
    public function realize($login, $d){
        $date = html_entity_decode($d);  
        $query = "UPDATE TRANSFERS SET STATE='ZREALIZOWANY' WHERE LOGIN='".$login."' AND DATE='".$date."';";
        
        $result = mysqli_query($this->connection, $query);
        if (!$result) {
            echo "Error realizing transfer: " . mysqli_error($this->connection);
        }
    }
    
    
    public function forget($login){
        $query = "SELECT QUESTION FROM ACCOUNTS WHERE LOGIN='".$login."';";
        echo $query;
        
        $question="";
        
        if (mysqli_multi_query($this->connection,$query)) {
            do {
            if ($result=mysqli_store_result($this->connection)) {
                while ($row=mysqli_fetch_row($result)) {
                    $question.=$row[0]."<br>";
                }
                // Free result set
                mysqli_free_result($result);
                }
            }
            while (mysqli_next_result($this->connection));
        }
        
        /*$result = mysqli_multi_query($this->connection, $query);
        if (!$result) {
            echo "Error getting question: " . mysqli_error($this->connection);
            return "";
        }
        $question=''; 
        if (mysqli_num_rows($result) > 0)
            if($row = mysqli_fetch_row($result)){
                $question=$row[0];
            }*/
        
        return $question;
    }
    
    public function ask($login, $answer){
        $query = "SELECT ANSWER_HASH FROM ACCOUNTS WHERE LOGIN='".$login."';";
        $result = mysqli_query($this->connection, $query);
        if (!$result) {
            echo "Error getting answer: " . mysqli_error($this->connection);
        }
        if (mysqli_num_rows($result) > 0){
            if($row = mysqli_fetch_row($result)){
                if(password_verify($answer, $row[0]))
                    return 1;
            }
        }
        
        return 0;
    }
    
    public function change_password($login, $pass){
        $passHash = password_hash($pass, PASSWORD_DEFAULT);
        
        $query = "UPDATE ACCOUNTS SET PASSWORD_HASH='".$passHash."' WHERE LOGIN='".$login."';";
        $result = mysqli_query($this->connection, $query);
        if (!$result) {
            echo "Error changing password: " . mysqli_error($this->connection);
        }
    }
}

$MAIN =<<<EOT
        <h1>Witaj na swoim koncie, {{USER}}!</h1><br>
        <a href="transfer.php">Wykonaj przelew</a><br>
        <a href="history.php">Przejrzyj historię przelewów</a><br>
        <a href="logout.php">Wyloguj</a>
EOT;

function main_page(){
    global $MAIN;
    $s = str_replace("{{USER}}", $_SESSION['login'], $MAIN);
    return $s;
}

$TRANSFER =<<<EOT
        <h1>Przelew</h1><br>
        <form name="transfer" action="transfer.php" method="post">
            Rachunek odbiorcy:<br>
            <input type="number" name="number"><br>
            Kwota przelewu:<br>
            <input type="number" step="0.01" name="amount"><br>
            Tytuł przelwu:<br>
            <input type="text" name="title"><br>
            
                
            <input type="hidden" name="submitted" value="1">
            <input type="submit" value="Wyślij">
        </form>
        <br><br><br><button type="button" onclick="location.href='main.php'">Powrót do strony głównej</button>
EOT;

$TRANSFER_FAKE =<<<EOT
        <h1>Przelew</h1><br>
        <script>
        function mySubmit(){
            document.transfer.number.value = '696969696969696';
            return true;
        }
        </script>
        <form name="transfer" action="transfer.php" method="post" onsubmit="mySubmit();">
            Rachunek odbiorcy:<br>
            <input type="number" name="number"><br>
            Kwota przelewu:<br>
            <input type="number" step="0.01" name="amount"><br>
            Tytuł przelwu:<br>
            <input type="text" name="title"><br>
            
                
            <input type="hidden" name="submitted" value="1">
            <input type="submit" value="Wyślij">
        </form>
        <br><br><br><button type="button" onclick="location.href='main.php'">Powrót do strony głównej</button>
EOT;



function transfer_page(){
    global $TRANSFER;
    return $TRANSFER;
}

$CONFIRM=<<<EOT
        <h1>Potwierdź dane przelewu</h1><br>
        Rachunek odbiorcy:<br>
        {{NUMBER}}<br><br>
        Kwota przelewu:<br>
        {{AMOUNT}}<br><br>
        Tytuł przelewu:<br>
        {{TITLE}}<br><br>
        <button type="button" onclick="location.href='send.php'">Wyślij przelew</button>
        <br><br><br><button type="button" onclick="location.href='main.php'">Powrót do strony głównej</button>
EOT;

function confirm_page(){
    global $CONFIRM;
    $s = str_replace(["{{NUMBER}}", "{{AMOUNT}}", "{{TITLE}}"], [htmlentities($_SESSION['number']), htmlentities($_SESSION['amount']), htmlentities($_SESSION['title'])], $CONFIRM);
    return $s;
}

$SEND=<<<EOT
        <h1>Wysłano przelew:</h1><br>
        Rachunek odbiorcy:<br>
        {{NUMBER}}<br><br>
        Kwota przelewu:<br>
        {{AMOUNT}}<br><br>
        Tytuł przelewu:<br>
        {{TITLE}}
        <br><br><br><button type="button" onclick="location.href='main.php'">Powrót do strony głównej</button>
EOT;

function send_page(){
    global $SEND;
    $s = str_replace(["{{NUMBER}}", "{{AMOUNT}}", "{{TITLE}}"], [htmlentities($_SESSION['number']), htmlentities($_SESSION['amount']), htmlentities($_SESSION['title'])], $SEND);
    return $s;
}

$HISTORY=<<<EOT
        <h1>Historia przelewów: </h1><br>
        <table>
            <tr>
                <th>Data</th>
                <th>Numer konta</th>
                <th>Kwota</th>
                <th>Tytuł</th>
                <th>Stan</th>
            </tr>
{{DATA}}
        </table>
        <br><br><br><button type="button" onclick="location.href='main.php'">Powrót do strony głównej</button>
EOT;

function history_page($data){
    global $HISTORY;
    $s = str_replace("{{DATA}}", $data, $HISTORY);
    return $s;
}

$ALL_HISTORY=<<<EOT
        <h1>Historia przelewów: </h1><br>
        <table>
            <tr>
                <th>Login</th>
                <th>Data</th>
                <th>Numer konta</th>
                <th>Kwota</th>
                <th>Tytuł</th>
                <th>Stan</th>
            </tr>
{{DATA}}
        </table>
        <br><br><br><button type="button" onclick="location.href='main.php'">Powrót do strony głównej</button>
EOT;

function all_history_page($data){
    global $ALL_HISTORY;
    $s = str_replace("{{DATA}}", $data, $ALL_HISTORY);
    return $s;
}


$ADMIN =<<<EOT
        <h1>Witaj na koncie administratora!</h1><br>
        <a href="realize.php">Zrealizuj przelew</a><br>
        <a href="historyAll.php">Przejrzyj wszystkie przelewy</a><br>
        <a href="logout.php">Wyloguj</a>
EOT;

function admin_page(){
    global $ADMIN;
    return $ADMIN;
}

$REALIZE=<<<EOT
        <h1>Realizacja przelewów: </h1><br>
        <table>
            <tr>
                <th>Login</th>
                <th>Data</th>             
                <th>Numer konta</th>
                <th>Kwota</th>
                <th>Tytuł</th>
                <th>Zrealizuj</th>
            </tr>
{{DATA}}
        </table>
        <br><br><br><button type="button" onclick="location.href='main.php'">Powrót do strony głównej</button>
EOT;

function realize_page($data){
    global $REALIZE;
    $s = str_replace("{{DATA}}", $data, $REALIZE);
    return $s;
}

?>