
let ws = new WebSocket('ws://'+window.location.hostname+':81/');
console.log('ws://'+window.location.hostname+':81/');

let clickSfx = new Audio('tammy-the-brown/audio/click3.wav');
clickSfx.volume = 0.35;

ws.onopen = (evt) => {
    let json = '{"userInfo":"webApp"}';
    ws.send(json);
};

function buttonPress(buf) {
    clickSfx.play();
    let json = `{"buttonPressed":"${buf}"}`;
    console.log(json);
    ws.send(json);
}