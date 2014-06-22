% Scott Chatham

father(al, bud).
father(al, kelly).
mother(peggy, kelly).
mother(peggy, bud).
mother(martha, peggy).

% problem 7
grandma(X,Y) :- mother(X,Z), mother(Z,Y).

% problem 8
descendants(X,Y) :- mother(X,Y); father(X,Y); ((mother(X,Z); father(X,Z)), descendants(Z,Y)).

% problem 9
siblings(X,Y) :- X \= Y, mother(Z,X), mother(Z,Y), father(W,X), father(W,Y).

% problem 10
transition(q0,q1,a).
transition(q1,q2,b).
transition(q0,q3,a).
transition(q3,q3,a).
accepting(q2).
accepting(q3).

accepts(S, []) :- accepting(S).
accepts(S, [H|T]) :- transition(S, X, H), accepts(X,T).

