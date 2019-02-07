% form.pl
% Przemyslaw Kobylanski <przemko@mac.com>

:- module(form, [build_form/2]).

% build_form(+ListOfOptions, +ListOfElements)
%
build_form(Options, Elements) :-
	open_form(Options),
	build_elements(Elements),
	close_form.

open_form(Options) :-
	format('<form'),
	check_action(Options),
	check_method(Options),
	format('>~n').

close_form :-
	format('</form>~n').

check_action(Options) :-
	(   member(action(Url), Options)
	->  format(' action="~w"', [Url])
	;   true).

check_method(Options) :-
	(   member(method(get), Options)
	->  format(' method="get"')
	;   (   member(method(post), Options)
	    -> format(' method="post"')
	    ;	(   member(method(Method), Options)
		->  throw(unrecognized(method(Method)))
		;   true))).

build_elements([]).
build_elements([Element | Rest]) :-
	build_element(Element),
	build_elements(Rest).

build_element(br) :-
	format('  <br>~n').
build_element(input(text, Name)) :-
	format('  <input type="text" name="~w">~n',
	       [Name]).
build_element(input(submit)) :-
	format('  <input type="submit" value="Submit">~n').
build_element(input(submit, Value)) :-
	format('  <input type="submit" value="~w">~n',
	       [Value]).
build_element(input(radio, Name, Value, Text)) :-
	format('  <input type="radio" name="~w" value="~w"> ~w~n',
	       [Name, Value, Text]).
build_element(input(radio(checked), Name, Value, Text)) :-
	format('  <input type="radio" name="~w" value="~w" checked="checked"> ~w~n',
	       [Name, Value, Text]).
build_element(label(For, Text)) :-
	format('  <label for="~w">~w</label>~n',
	       [For, Text]).

