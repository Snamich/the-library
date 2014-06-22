// Scott Chatham

function update() {
    if ($("#fromUnit").val() === $("#toUnit").val()) {
        // The same unit for both "from" and "to"
        var v = parseInt($("#from").val());
        // Simply update #to without conversion
        $("#to").val(v);
    } else {
        var v = parseInt($("#from").val());
        var from = $("#fromUnit").val();
        var to = $("#toUnit").val();
        var val = null;
        
        if (from === "c") {
            if (to === "f") val = celsToFahr(v);
            else val = celsToKelv(v);
        } else if (from === "k") {
            if (to === "f") val = kelvToFahr(v);
            else val = kelvToCels(v);
        } else {
            if (to === "k") val = fahrToKelv(v);
            else val = fahrToCels(v);
        }
        
        $("#to").val(val);
    }
}

$("#from").keyup(update); // update on keyboard input
$("#fromUnit,#toUnit").change(update); // update on unit change

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
