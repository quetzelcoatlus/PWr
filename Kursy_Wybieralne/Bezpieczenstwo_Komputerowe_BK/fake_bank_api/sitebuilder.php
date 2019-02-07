<?php
session_start();

function debug_to_console($data) {
    $output = $data;
    if(is_array($output))
        $output = implode(',', $output);

    echo "<script>console.log('" . $output . "');</script>";
}

function test_input($data) {
    $data = htmlspecialchars($data, ENT_QUOTES);
    return $data;
}

$HEADER =<<<EOT
<!DOCTYPE html>
<html lang="pl">
<head>
    <meta charset="utf-8">
    <title>{{TITLE}}</title>
    <meta name="description" content= "{{DESCRIPTION}}">
    <meta name="author" content="Bartosz Drzazga">
    <meta name="viewport" content = "width=device-width, initial-scale=1.0"/>
    <link rel="stylesheet" href="styles.css">
</head>
<body>

EOT;

function get_header($title="", $description="") {
    global $HEADER;
    return str_replace(["{{TITLE}}", "{{DESCRIPTION}}"], [$title, $description], $HEADER);
}

$FOOTER =<<<EOT

</body>
</html>
EOT;

function get_footer() {
    global $FOOTER;
    return $FOOTER;
}

$HOME =<<<EOT
        <img src="logo.png" alt="Logo">
        <br>
        <a href="login.php" class="button">Logowanie</a><br>
        <a href="register.php" class="button">Rejestracja</a>
EOT;

function get_home_page() {
    global $HOME;
    return $HOME;
}

$REGISTER =<<<EOT
        <h1>Rejestracja</h1>{{error}}
        <form action="register.php" method="post">
            <div>
                <label>Login:</label><br>
                <input type="text" name="login"/>
            </div>
            <div>
                <label>Adres email:</label><br>
                <input type="email" name="email"/>
            </div>
            <div>
                <label>Hasło:</label><br>
                <input type="password" name="pass"/>
            </div>
            <div>
                <label>Powtórz hasło:</label><br>
                <input type="password" name="pass2"/>
            </div>
            <div>
                <label>Pytanie pomocnicze:</label><br>
                <input type="text" name="question"/>
            </div>
            <div>
                <label>Odpowiedź:</label><br>
                <input type="text" name="answer"/>
            </div>
            <br>
            <input type="hidden" name="submitted" value="1">
            <input type="submit" class="button" value="Zarejestruj">
        </form>
        <button type="button" onclick="location.href='index.php'">Anuluj</button>
EOT;

function get_register_page($error = ""){
    global $REGISTER;
    if($error !== "")
        $error = "\n<h2>".$error."</h2>";
    return str_replace("{{error}}", $error, $REGISTER);
}

$LOGIN =<<<EOT
        <h1>Logowanie</h1>{{error}}
        <form action="login.php" method="post">
            <div>
                <label>Login:</label><br>
                <input type="text" name="login"/>
            </div>
            <div>
                <label>Hasło:</label><br>
                <input type="password" name="pass"/>
            </div>
            <br>
            <input type="hidden" name="submitted" value="1">
            <input type="submit" class="button" value="Zaloguj"><br>
            <button type="button" onclick="location.href='forget.php'">Zapomniałeś hasła?</button>
        </form>
        <button type="button" onclick="location.href='index.php'">Anuluj</button>
EOT;

function get_login_page($error = ""){
    global $LOGIN;
    if($error !== "")
        $error = "\n<h2>".$error."</h2>";
    return str_replace("{{error}}", $error, $LOGIN);
}

$FORGET =<<<EOT
        <h1>Przywracanie dostępu</h1>{{error}}
        <form action="forget.php" method="post">
            <div>
                <label>Login:</label><br>
                <input type="text" name="login"/>
            </div>
            <br>
            <input type="hidden" name="submitted" value="1">
            <input type="submit" class="button" value="Dalej">
        </form>
        <button type="button" onclick="location.href='login.php'">Anuluj</button>
EOT;

function get_forget_page($error = ""){
    global $FORGET;
    if($error !== "")
        $error = "\n<h2>".$error."</h2>";
    return str_replace("{{error}}", $error, $FORGET);
}

$CHANGE =<<<EOT
        <h1>Resetowanie hasła</h1>{{error}}
        <form action="change.php" method="post">
            <div>
                <label>Odpowidz na pytanie!</label><br>
                <label>{{QUEST}}</label><br>
                <input type="text" name="answer"/>
            </div>
            <div>
                <label>Nowe hasło:</label><br>
                <input type="password" name="pass"/>
            </div>
            <div>
                <label>Powtórz hasło:</label><br>
                <input type="password" name="pass2"/>
            </div>
            <br>
            <input type="hidden" name="submitted" value="1">
            <input type="submit" class="button" value="Resetuj hasło">
        </form>
        <button type="button" onclick="location.href='index.php'">Anuluj</button>
EOT;

function get_change_page($error){
    global $CHANGE;
    if($error !== "")
        $error = "\n<h2>".$error."</h2>";
    return str_replace(["{{QUEST}}", "{{error}}"], [$_SESSION['question'], $error], $CHANGE);
}

$CONFIRM=<<<EOT
        <h1>Podsumowanie przelewu</h1>
        Rachunek odbiorcy:<br>
        {{NUMBER}}<br><br>
        Kwota przelewu:<br>
        {{AMOUNT}}<br><br>
        Tytuł przelewu:<br>
        {{TITLE}}<br><br>
        <button type="button" onclick="location.href='send.php'">Wyślij przelew</button><br>
        <button type="button" onclick="location.href='main.php'">Anuluj</button>
EOT;

function get_confirm_page(){
    global $CONFIRM;
    return str_replace(["{{NUMBER}}", "{{AMOUNT}}", "{{TITLE}}"], [$_SESSION['number'], $_SESSION['amount'], $_SESSION['title']], $CONFIRM);
}

$SEND=<<<EOT
        <h1>Wysłano przelew:</h1>
        Rachunek odbiorcy:<br>
        {{NUMBER}}<br><br>
        Kwota przelewu:<br>
        {{AMOUNT}}<br><br>
        Tytuł przelewu:<br>
        {{TITLE}}<br><br>
        <button type="button" onclick="location.href='main.php'">Powrót do strony głównej</button>
EOT;

function get_send_page(){
    global $SEND;
    return str_replace(["{{NUMBER}}", "{{AMOUNT}}", "{{TITLE}}"], [$_SESSION['number'], $_SESSION['amount'], $_SESSION['title']], $SEND);
}

$MAIN =<<<EOT
        <h1>Witaj {{USER}}!</h1>
        <a href="transfer.php" class = "button">Zleć przelew</a><br>
        <a href="history.php" class = "button">Historia przelewów</a><br>
        <a href="logout.php" class = "button">Wyloguj</a>
EOT;

function get_main_page(){
    global $MAIN;
    return str_replace("{{USER}}", $_SESSION['login'], $MAIN);
}

$TRANSFER =<<<EOT
        <h1>Przelew</h1>{{error}}
        <form name="transfer" action="transfer.php" method="post">
            <div>
                <label>Numer konta odbiorcy:</label><br>
                <input type="number" name="number"/>
            </div>
            <div>
                <label>Kwota:</label><br>
                <input type="number" step="0.01" name="amount"/>
            </div>
            <div>
                <label>Tytuł:</label><br>
                <input type="text" name="title"/>
            </div>
            <br>
            <input type="hidden" name="submitted" value="1">
            <input type="submit" class = "button" value="Wyślij">
        </form>
        <button type="button" onclick="location.href='main.php'">Anuluj</button>
EOT;

function get_transfer_page($error = "") {
    global $TRANSFER;
    if($error !== "")
        $error = "\n<h2>".$error."</h2>";
    return str_replace("{{error}}", $error, $TRANSFER);
}

$HISTORY=<<<EOT
        <h1>Historia przelewów:</h1>
        <table>
            <tr>
                <th>Data</th>
                <th>Numer konta</th>
                <th>Kwota</th>
                <th>Tytuł</th>
            </tr>
{{DATA}}
        </table>
        <br><button type="button" onclick="location.href='main.php'">Powrót do strony głównej</button>
EOT;

function get_history_page(){
    $db = new dataBase();
    $data = $db->get_history();
    global $HISTORY;
    return str_replace("{{DATA}}", $data, $HISTORY);
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
        TITLE TEXT NOT NULL
    );
EOF;

class dataBase {
    private $servername = "localhost";
    private $username = "root";
    private $password = "";
    private $connection;

    public function __construct() {
        $this->connection = mysqli_connect($this->servername, $this->username, $this->password);
        if(!$this->connection) {
            die("Connection failed: " . mysqli_connect_error());
        }

        if(!mysqli_query($this->connection, "CREATE DATABASE IF NOT EXISTS LISTA6;")) {
            echo "Error creating database: " . mysqli_error($this->connection);
        }
        if(!mysqli_query($this->connection, "USE LISTA6;")) {
            echo "Error using database: " . mysqli_error($this->connection);
        }

        global $CREATE_TABLE_ACCOUNTS;
        if(!mysqli_query($this->connection, $CREATE_TABLE_ACCOUNTS)) {
            echo "Error creating table accounts: " . mysqli_error($this->connection);
        }

        global $CREATE_TABLE_TRANSFERS;
        if(!mysqli_query($this->connection, $CREATE_TABLE_TRANSFERS)) {
            echo "Error creating table transfers: " . mysqli_error($this->connection);
        }
    }

    public function add_account($login, $email, $pass, $quest, $answer){
        $login = test_input($login);
        $email = test_input($email);
        $quest = test_input($quest);

        $query = "SELECT EMAIL FROM ACCOUNTS WHERE LOGIN='".$login."';";
        $result = mysqli_query($this->connection, $query);
        if(!$result) {
            echo "Error getting accounts: " . mysqli_error($this->connection);
        }

        if(mysqli_num_rows($result) > 0)
            return 1;

        $query = "SELECT LOGIN FROM ACCOUNTS WHERE EMAIl='".$email."';";
        $result = mysqli_query($this->connection, $query);
        if(!$result) {
            echo "Error getting accounts: " . mysqli_error($this->connection);
        }

        if(mysqli_num_rows($result) > 0)
            return 2;

        $passHash = sodium_crypto_pwhash_str($pass, SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE, SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE);
        $answerHash = sodium_crypto_pwhash_str($answer, SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE, SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE);

        $add = "INSERT INTO ACCOUNTS (LOGIN, EMAIL, PASSWORD_HASH, QUESTION, ANSWER_HASH) VALUES('".$login."','".$email."','".$passHash."','".$quest."','".$answerHash."');";
        if(!mysqli_query($this->connection, $add)) {
            echo "Error adding account: " . mysqli_error($this->connection);
        }
        return 0;
    }

    public function login($login, $pass){
        $login = test_input($login);

        $query = "SELECT PASSWORD_HASH FROM ACCOUNTS WHERE LOGIN='".$login."';";
        $result = mysqli_query($this->connection, $query);
        if(!$result) {
            echo "Error getting accounts: " . mysqli_error($this->connection);
        }

        if(mysqli_num_rows($result) > 0) {
            $row = mysqli_fetch_row($result);
            if(sodium_crypto_pwhash_str_verify($row[0], $pass))
               return 0;
            else
               return 1;
        } else {
            return 2;
        }
    }

    public function add_transfer($login, $number, $amount, $title){
        $number = test_input($number);
        $amount = test_input($amount);
        $title = test_input($title);

        $date = date('G:i:s d-m-Y');
        $add = "INSERT INTO TRANSFERS (LOGIN, DATE, NUMBER, AMOUNT, TITLE) VALUES('".$login."','".$date."','".$number."','".$amount."','".$title."');";
        if(!mysqli_query($this->connection, $add)) {
            echo "Error adding transfer: " . mysqli_error($this->connection);
        }
    }

    public function get_history(){
        $query = "SELECT DATE, NUMBER, AMOUNT, TITLE FROM TRANSFERS WHERE LOGIN='". $_SESSION['login'] ."';";
        $result = mysqli_query($this->connection, $query);
        if(!$result) {
            echo "Error getting transfers: " . mysqli_error($this->connection);
        }
        $data='';
        if(mysqli_num_rows($result) > 0)
            while($row = mysqli_fetch_row($result)){
                $data.="            <tr>\n              <td>".$row[0]."</td>\n             <td>".$row[1]."</td>\n             <td>".$row[2]."</td>\n             <td>".$row[3]."</td>\n           </tr>\n";
            }

        return $data;
    }

    public function get_raw_history(){
        $query = "SELECT DATE as date, NUMBER as number, AMOUNT as amount, TITLE as title FROM TRANSFERS WHERE LOGIN='". $_SESSION['login'] ."';";
        $result = mysqli_query($this->connection, $query);
        $rows = array();
        while($row = mysqli_fetch_assoc($result)){
            $rows[] = $row;
        }

        return $rows;
    }

    public function forget($login){
        $login = test_input($login);

        $query = "SELECT QUESTION FROM ACCOUNTS WHERE LOGIN='".$login."';";
        $result = mysqli_query($this->connection, $query);
        if(!$result) {
            echo "Error getting question: " . mysqli_error($this->connection);
        }
        $question='';
        if(mysqli_num_rows($result) > 0)
            if($row = mysqli_fetch_row($result)){
                $question=$row[0];
            }

        return $question;
    }

    public function ask($login, $answer){
        $query = "SELECT ANSWER_HASH FROM ACCOUNTS WHERE LOGIN='".$login."';";
        $result = mysqli_query($this->connection, $query);
        if(!$result) {
            echo "Error getting answer: " . mysqli_error($this->connection);
        }
        if(mysqli_num_rows($result) > 0){
            if($row = mysqli_fetch_row($result)){
                if(sodium_crypto_pwhash_str_verify($row[0], $answer))
                    return 1;
            }
        }

        return 0;
    }

    public function change_password($login, $pass){
        $passHash = sodium_crypto_pwhash_str($pass, SODIUM_CRYPTO_PWHASH_OPSLIMIT_INTERACTIVE, SODIUM_CRYPTO_PWHASH_MEMLIMIT_INTERACTIVE);

        $query = "UPDATE ACCOUNTS SET PASSWORD_HASH='".$passHash."' WHERE LOGIN='".$login."';";
        $result = mysqli_query($this->connection, $query);
        if(!$result) {
            echo "Error changing password: " . mysqli_error($this->connection);
        }
    }
}
