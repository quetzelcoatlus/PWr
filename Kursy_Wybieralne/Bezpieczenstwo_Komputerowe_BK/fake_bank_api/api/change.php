<?php
    require_once("../all.php");

    header("Access-Control-Allow-Origin: *");
    header("Content-Type: application/json; charset=UTF-8");
    header("Access-Control-Allow-Methods: POST");
    header("Access-Control-Max-Age: 3600");
    header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

    $data = json_decode(file_get_contents("php://input"));

    $db = new dataBase();

    if(!empty($data->login) && !empty($data->answer) && !empty($data->pass)) {
        $answer = $db->ask($data->login, $data->answer);
        if($answer === 1){
            $db->change_password($data->login, $data->pass);
            http_response_code(200);
            echo json_encode(array("message" => "Password changed successfully."));
        }
        else {
            http_response_code(400);
            echo json_encode(array("message" => "Wrong answer."));
        }
    }
    else {
        http_response_code(400);
        echo json_encode(array("message" => "Unable to log in. Data is incomplete."));
    }
?>
