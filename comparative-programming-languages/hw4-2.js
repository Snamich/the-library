// Scott Chatham

function update(from, to) {
    var from = "#" + from;
    var to = "#" + to;
    var fromUnit = from + "Unit";
    var toUnit = to + "Unit";
    
    if ($(fromUnit).val() === $(toUnit).val()) {
        var v = parseInt($(from).val());
        $(to).val(v);
    } else {
        var v = parseInt($(from).val());
        var fromV = $(fromUnit).val();
        var toV = $(toUnit).val();
        var val = null;
        
        if (fromV === "c") {
            if (toV === "f") val = celsToFahr(v);
            else val = celsToKelv(v);
        } else if (fromV === "k") {
            if (toV === "f") val = kelvToFahr(v);
            else val = kelvToCels(v);
        } else {
            if (toV === "k") val = fahrToKelv(v);
            else val = fahrToCels(v);
        }
        
        $(to).val(val);
    }
}

$("#from").keyup(function() { update("from", "to") }); // update on keyboard input
$("#to").keyup(function() { update("to", "from") });
$("#fromUnit").change(function() { update("from", "to") }); // update on unit change
$("#toUnit").change(function() { update("to", "from") });

function fahrToCels(f) {
    return (5.0 / 9.0) * (f - 32.0);
}

function celsToFahr(c) {
    return c * (9.0 / 5.0) + 32.0;
}

function celsToKelv(c) {
    return c + 273.15;
}

function kelvToCels(k) {
    return k - 273.15;
}

function fahrToKelv(f) {
    return (f + 459.67) * (5.0 / 9.0);
}

function kelvToFahr(k) {
    return k * (9.0 / 5.0) - 459.67
}
