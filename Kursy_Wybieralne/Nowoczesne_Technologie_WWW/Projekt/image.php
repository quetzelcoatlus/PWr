<?php
    session_start();
    
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

    function zle($a, $b, $c, $s1, $s2, $base){
        if($s1 == "/"){
            if($c == 0)
                return true;
        }
        if($s2 == "/"){
            $d = policz($b, $c, $s1,$base);
            if($d == 0)
                return true;
        }
        return false;
    }

    $my_img = imagecreate( 200,50 );
    $basearray = array(3,5,7,11,13,17);
    $signs = array("+","-","*","/");
    $base = $basearray[rand(0,5)];
    $check = true;
    while($check){
        $a = rand(0,$base-1);
        $b = rand(0,$base-1);
        $c = rand(0,$base-1);
        $s1 = $signs[rand(0,3)];
        $s2 = $signs[rand(0,3)];
        $check = zle($a,$b,$c,$s1,$s2,$base);
    }
    
    $_SESSION['a']="".$a;
    $_SESSION['b']="".$b;
    $_SESSION['c']="".$c;
    $_SESSION['s1']="".$s1;
    $_SESSION['s2']="".$s2;
    $_SESSION['base']="".$base;
    
    $str1 = "Oblicz w ciele Z(".$base."):";
    $str2 = "".$a." ".$b." ".$c." ".$s1." ".$s2;
    $background = imagecolorallocate( $my_img, 52, 9, 51 );
    $text_colour = imagecolorallocate( $my_img, 255,255,255);
    imagestring( $my_img, 5, 7, 5, $str1, $text_colour );
    imagestring( $my_img, 5, 50, 30, $str2, $text_colour );
    header( "Content-type: image/png" );
    imagepng( $my_img );
    imagecolordeallocate( $text_color );
    imagecolordeallocate( $background );
    imagedestroy( $my_img );
?>