<?php
    require_once("../all.php");

    header("Access-Control-Allow-Origin: *");
    header("Content-Type: application/json; charset=UTF-8");
    header("Access-Control-Allow-Methods: POST");
    header("Access-Control-Max-Age: 3600");
    header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

    $data = json_decode(file_get_contents("php://input"));

    $db = new dataBase();

    if(!empty($data->login)) {
        $question = $db->forget($data->login);
        if(!empty($question)) {
            http_response_code(200);
            echo json_encode(array("question" => $question));
        } else {
            http_response_code(400);
            echo json_encode(array("message" => "Wrong username."));
        }
    }
    else {
        http_response_code(400);
        echo json_encode(array("message" => "Data is incomplete."));
    }
?>
