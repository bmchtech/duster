var rom_name = 'Duster.gba';
var release_tag = 'v0.1';
var releases_url = 'https://github.com/redthing1/duster/releases/download/';

var DUSTER_ROM_LOC = `${releases_url}/${release_tag}/${rom_name}`;

var LOADED_GAME_ROM = null;

function on_get_rom_click() {
    window.open(DUSTER_ROM_LOC);
}

window.addEventListener('load', function () {
    document.getElementById('file').onchange = function (e) {
        var gameUrl = URL.createObjectURL(new Blob([e.target.files[0]]))
        EJS_gameUrl = gameUrl;
        console.log('new file ready: ', EJS_gameUrl);

        var script = document.createElement('script');
        script.src = 'data/loader.js';
        document.body.appendChild(script);

        document.getElementById('loader-buttons').hidden = true;
        document.getElementById('instructions').hidden = true;
    }
})