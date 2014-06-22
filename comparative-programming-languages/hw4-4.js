// Scott Chatham

// The following example code shows you how to change the appearance of states
// and how to use object-oriented programming without classes.
// Feel free to delete this and write your own solution.

// the last state selected
lastState = null;

// Constructor for State objects.
// el is the DOM element used as view
function State(el, accepting) {
    this.el = el;
    this.accepting = accepting;
    // Clicking on el calls this.highlight
    this.el.click(this.highlight.bind(this));
    this.transitions = {};
}

// Highlight a state by changing the background of its element to yellow
// or green if it's accepting
State.prototype.highlight = function() {
    if (this.accepting) this.el.css('background', '#6f6');
    else this.el.css('background', '#ff6');

    if (lastState && lastState !== this) lastState.unhighlight();
    lastState = this;
}

State.prototype.unhighlight = function() {
    this.el.css('background', '#fff');
}

// Add a transition from this state to nextState with symbol
State.prototype.addTransition = function(symbol, nextState) {
    this.transitions[symbol] = nextState;
}

// Creating the states with their elements:
var q0 = new State($("#q0"));
var q1 = new State($("#q1"));
q0.addTransition("a", q1);
var q2 = new State($("#q2"), true);
q1.addTransition("b", q2);
var q3 = new State($("#q3"), true);
q0.addTransition("c", q3);
q3.addTransition("d", q3);

// Clicking on step chops off the first character in the
// symbols text field and executes the transition
$("#step").click(function() {
    var symbols = $("#symbols").val();
    var first = symbols[0];

    // check if a state is selected
    if (lastState) {
        var next = lastState.transitions[first];
	// does it have a transition on the symbol
        if (next) {
	    // transition exists, so execute it
            next.highlight();
        } else {
	    // no transition, clear the board
            lastState.unhighlight();
            lastState = null;
        }
    }

    $("#symbols").val(symbols.substr(1));
});
