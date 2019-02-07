<?php
    require_once("../all.php");

    header("Access-Control-Allow-Origin: *");
    header("Content-Type: application/json; charset=UTF-8");
    header("Access-Control-Allow-Methods: POST");
    header("Access-Control-Max-Age: 3600");
    header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

    if(!isset($_SESSION["login"])){
        http_response_code(401);
        die();
    }

    $db = new dataBase();
    $data = $db->get_raw_history();

    http_response_code(200);
    echo json_encode($data);
?>
