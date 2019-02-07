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

    $data = json_decode(file_get_contents("php://input"));

    $db = new dataBase();

    if(!empty($data->number) && !empty($data->amount) && !empty($data->title)) {
        $db->add_transfer($_SESSION['login'], $data->number, $data->amount, $data->title);
        http_response_code(201);
        echo json_encode(array("message" => "Transfer sent."));
    }
    else {
        http_response_code(400);
        echo json_encode(array("message" => "Data is incomplete."));
    }
?>
