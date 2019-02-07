<?php
    require_once("../all.php");

    header("Access-Control-Allow-Origin: *");
    header("Content-Type: application/json; charset=UTF-8");
    header("Access-Control-Allow-Methods: POST");
    header("Access-Control-Max-Age: 3600");
    header("Access-Control-Allow-Headers: Content-Type, Access-Control-Allow-Headers, Authorization, X-Requested-With");

    $data = json_decode(file_get_contents("php://input"));

    $db = new dataBase();

    if(
    !empty($data->login) &&
    !empty($data->email) &&
    !empty($data->pass) &&
    !empty($data->question) &&
    !empty($data->answer)) {
        $reg = $db->add_account($data->login, $data->email, $data->pass, $data->question, $data->answer);
        if($reg === 0) {
            http_response_code(201);
            echo json_encode(array("message" => "Account created."));
        }
        else if($reg === 1) {
            http_response_code(400);
            echo json_encode(array("message" => "Error occured while registering."));
        }
		else if($reg === 2) {
            http_response_code(400);
            echo json_encode(array("message" => "Login already used."));
        }
    }
    else {
        http_response_code(400);
        echo json_encode(array("message" => "Unable to create an account. Data is incomplete."));
    }
?>
