:- use_module(library(http/thread_httpd)).
:- use_module(library(http/http_dispatch)).
:- use_module(library(http/http_client)).
:- use_module(library(http/html_write)).
:- use_module(library(http/http_files)).

:- use_module(form).
:- use_module(queens).

:- http_handler(root(.), handler_queens, []).  
:- http_handler(root(queens), handler_queens, []).           
:- http_handler(root(solution), handler_solution, []).                  
:- http_handler(root(.), http_reply_from_files('pic', []), [prefix]).   


server(Port) :-
    http_server(http_dispatch, [port(Port)]).                           

handler_queens(_Request) :-
    format('Content-type: text/html~n~n<!DOCTYPE html>~n<html>~n<head>~n<title>Queens</title>~n<meta http-equiv="content-type" content="text/html; charset=UTF-8">~n</head>~n<body>~n<h1>Please insert size of the problem:</h1>~n',[]),  
    build_form([action(solution), method(post)],  
               [    
                    label(size, 'Size:'), input(text, size), br, br,    
                    input(submit)                                       
               ]),
    format('</body>~n</html>~n', []).                                   


handler_solution(Request) :-                                            
    member(method(post), Request), !,                                   
    http_read_data(Request, Data, []),    
    member(size=Size, Data),
    (   atom_number(Size, SizeNumber)                                   
    ->  (   SizeNumber =< 100                                          
        ->  (   queens(SizeNumber, P)                                   
            ->  atom_concat('Solution for size = ', Size, S),               
                reply_html_page([title('Solution'), style('img{display:block}')], [h1(S), table([cellspacing="0", cellpadding="0"],\queensTable(P, SizeNumber))]) 
            ;   atom_concat("It's not possible to solve this problem for size = ", Size, S1),
                reply_html_page(title('No solution'), h1(S1))     
            )
        ;   atom_concat("Oh, no! I'm afraid that size of ", Size, S1), atom_concat(S1," it's too big for me! :(", S2), 
            reply_html_page(title('No solution'), h1(S2))              
        )
    ;   reply_html_page(title('No solution'), h1("Wrong input in Size field. :/"))    
    ).
    

queensTable([], _) --> [].                              
queensTable([H|T], S) -->                               
    html(tr(\tableData(H, S))),                         
    queensTable(T, S).                                  
    
    
tableData(_, 0) --> [].                                 

tableData(H, H) -->                  
    {H2 is H-1},
    html(td(img(src="./queen.png"))),                                        
    tableData(H,H2).                                    
    
tableData(H, S) -->        
    {S > 0, S2 is S-1},
    html(td(img(src="./empty.png"))),                                      
    tableData(H,S2).                                    