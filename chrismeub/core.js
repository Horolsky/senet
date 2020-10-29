function _is_def(a) {
    return typeof a !== 'undefined';
}
function _default(a, b, c) {
    if (! _is_def(a[b])) 
        a[b] = c;
    
}
function _is_object(a) {
    return a && typeof a === 'object';
}
function _is_array(a) {
    return a && a.constructor === Array;
}
function _is_string(a) {
    return !!(a === '' || (a && a.length && a === (a + '')));
}
function _is_number(a) {
    return a !== Number.NaN && (+ a) === a;
}
function _is_boolean(a) {
    return a === false || a === true;
}
function _is_null(a) {
    return a === null;
}
function _toi(a) {
    var b = parseInt(a);
    return(b === NaN || b === undefined) ? null : b;
}
function _return_false() {
    return false;
}
function _sat(a) {
    return Math.max(0, Math.min(1, a));
}
function _eql2(a, b) {
    return a[0] == b[0] && a[1] == b[1];
}
function _add2(a, b) {
    return [
        a[0] + b[0],
        a[1] + b[1]
    ];
}
function _sub2(a, b) {
    return [
        a[0] - b[0],
        a[1] - b[1]
    ];
}
function _sign2(a) {
    return [
        Math.sign(a[0]),
        Math.sign(a[1])
    ];
}
function _inc2(a, b) {
    a[0] += b[0];
    a[1] += b[1];
}
function _mul2(a, b) {
    return [
        a[0] * b[0],
        a[1] * b[1]
    ];
}
function _mag2(a) {
    return Math.sqrt(a[0] * a[0] + a[1] * a[1]);
}
function _norm2(a) {
    var b = _mag2(a);
    a[0] /= b;
    a[1] /= b;
    return a;
}
function _floor2(a) {
    return [
        Math.floor(a[0]),
        Math.floor(a[1])
    ];
}
function _swap2(a) {
    return [
        a[1], a[0]
    ];
}
function _rand2(a, b) {
    a = a || 0;
    b = b || 1;
    return [
        a + Math.random() * (b - a),
        a + Math.random() * (b - a)
    ];
}
function _set_cookie(a, b, c) {
    var d = new Date();
    d.setDate(d.getDate() + c);
    var e = escape(b) + ((c == null) ? "" : "; expires=" + d.toUTCString()) + '; path=/';
    document.cookie = a + "=" + e;
}
function _get_cookie(a) {
    var b,
        c,
        d,
        e = document.cookie.split(";");
    for (b = 0; b < e.length; b++) {
        c = e[b].substr(0, e[b].indexOf("="));
        d = e[b].substr(e[b].indexOf("=") + 1);
        c = c.replace(/^\s+|\s+$/g, "");
        if (c == a) 
            return unescape(d);
        
    }
    return null;
}
function _dpi_scale() {
    return _is_hidpi_screen() ? 2 : 1;
}
function _is_hidpi_screen() {
    var a = "(-webkit-min-device-pixel-ratio: 1.5),(min--moz-device-pixel-ratio: 1.5),(-o-min-device-pixel-ratio: 3/2),(min-resolution: 1.5dppx)";
    if (window.devicePixelRatio > 1) 
        return true;
    
    if (window.matchMedia && window.matchMedia(a).matches) 
        return true;
    
    return false;
}
