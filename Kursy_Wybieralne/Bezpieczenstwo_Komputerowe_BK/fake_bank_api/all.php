<?php
session_start();

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
        
        $query = "SELECT LOGIN FROM ACCOUNTS WHERE LOGIN='".$login."';";
        $result = mysqli_query($this->connection, $query);
        if(!$result) {
            echo "Error getting accounts: " . mysqli_error($this->connection);
        }

        if(mysqli_num_rows($result) > 0)
            return 2;
        
        $add = "INSERT INTO ACCOUNTS (LOGIN, EMAIL, PASSWORD_HASH, QUESTION, ANSWER_HASH) VALUES('".$login."','".$email."','".$passHash."','".$quest."','".$answerHash."');";
        if (!mysqli_query($this->connection, $add)) {
            echo "Error adding account: " . mysqli_error($this->connection);
            return 1;
        }
        return 0;
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
    
    public function get_raw_history(){
        $query = "SELECT DATE as date, NUMBER as number, AMOUNT as amount, TITLE as title, STATE as state FROM TRANSFERS WHERE LOGIN='". $_SESSION['login'] ."';";
        $result = mysqli_query($this->connection, $query);
        $rows = array();
        while($row = mysqli_fetch_assoc($result)){
            $rows[] = $row;
        }

        return $rows;
    }
    
    
    public function forget($login){
        $query = "SELECT QUESTION FROM ACCOUNTS WHERE LOGIN='".$login."';";
        
        $question="";
        
        if (mysqli_multi_query($this->connection,$query)) {
            do {
            if ($result=mysqli_store_result($this->connection)) {
                while ($row=mysqli_fetch_row($result)) {
                    $question.=$row[0];
                }
                // Free result set
                mysqli_free_result($result);
                }
            }
            while (mysqli_next_result($this->connection));
        }
        
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

?>