R"(
<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width" />
    <title>VULPEioWemoD1Uno</title>
    <link href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACQAAAAkCAYAAADhAJiYAAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAK7OHOkAAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAAAZiS0dEAAAAAAAA+UO7fwAAAAlwSFlzAAAASAAAAEgARslrPgAAAeBJREFUWMPt179qFFEUBvCfGq1iI0bU3Yg2guAbKGhs1Sj49wEk2Jg+3XZqaadgJT6BsNgYxGUD+ggqWOiagF20M+6uxcyQ48JsZmcGArIfDHzLPfe73zlzz72zTDFFNeypQeM8zqV8Dd3dTqqFYfq0qort3e1s/itDN7FSYo0V3Kjb+H0MJPvk0chYS/4eysYGWK7LzN1gZoivOFrA0HF8D2MD3KpqZh6/gmgXRyao0DG8D+M/0Ry34E576DFmU97DdfyYIKENLGI9/X0QD8tW5yT+hOwu5MSNq1CGhRCzhRNlKnQb+1L+Ae/KZoa3WE35TKo9saFLgT+rYCbDyxztwobOBt6pwdBajnZhQ4cD79Vg6Fvgc2UMbQW+vwZDBwL/nRc0M0ZgHadT3sDHnLiu5HjIeB4agW+Uyei17Va9V0OFloJeOy9o3CuLk+7UYChqtMsIzCt2MBbB6MHYLCv0PAh98W/nFcUhfA46TyskpiG5EDOxTrrAJGY6Yf6m5MKthEX0g+gnXCwwb2GkMn1crWomw/KIqSHeSDrnjOSLYDblS5J7K8b28aAuM7FS8fUVfTZxpW4zGebwRNIpOxnp44UJ90zZP4pNXMNlnLLdxj1JN7bxSj134BRTTFEr/gJjkZYrYDLmgAAAACV0RVh0ZGF0ZTpjcmVhdGUAMjAxNy0wOS0wN1QwMjoxMTo1MyswMDowMAsCEWsAAAAldEVYdGRhdGU6bW9kaWZ5ADIwMTctMDktMDdUMDI6MTE6NTMrMDA6MDB6X6nXAAAAKHRFWHRzdmc6YmFzZS11cmkAZmlsZTovLy90bXAvbWFnaWNrLTdmN3p1bmtNdOrSigAAAABJRU5ErkJggg==" rel="shortcut icon">
    <!-- <link href="styles.min.css" rel="stylesheet" type="text/css"> -->
    <link href="https://fonts.googleapis.com/css?family=Roboto:400,700" rel="stylesheet">
        <style>
        body {
          font-family: 'Roboto', sans-serif;
        }
    .container {
    display: flex;
    flex-wrap: wrap;
}

.container .colour_picker {
    width: 100%;
    padding: 0 .5em;
}

.container .controller {
    width: 100%;
    display: flex;
    flex-wrap:wrap;
}

.container .footnote {
    width: 100%;
}

.controller a {
    display: block;
    width: 30%;
    margin-left: 1em;
    margin-bottom: 1em;
}

.controller button {
    width: 100%;
    border: 0px;
    border-radius:15em;
    font-size: 1em;
    overflow: hidden;
}

.controller button:hover {
    background: #000;
    color: #fff;
    cursor: pointer;
}

.category { display: flex; width: 100%; flex-wrap: wrap; border-bottom: 1px solid #efefef; padding: 10px; }
.category h1 { width: 100%; }

.container .controller .category h1 {
    font-size: 1.5em;
}

.colour_picker input {
    width: 100%;
    border: none;
    text-align: center;
    font-size: 2em;
    border-radius: 1em;
    border: 1px solid #efefef;
}
.colour_picker input:hover { cursor: pointer; }
</style>
</head>

<body>
    <div class="container">
    <div class="colour_picker">


        <input class="jscolor { hash:true, width:400, height:250 ,borderColor:'#000', insetColor:'#FFF', backgroundColor:'#000'}" 
        onchange="makeAjaxCall('/set?m=0');makeAjaxCall('/rgb?color=0x' + this.jscolor);"  
        value="#FFFFFF"/>

    
    </div>
    
    <div class="controller">
        <div class="category">
        <h1>Standard</h1>
        <a onclick='makeAjaxCall("/set?m=0")'><button>Static</button></a>
        <a onclick='makeAjaxCall("/set?m=1")'><button>Blink</button></a>
        <a onclick='makeAjaxCall("/set?m=2")'><button>Breath</button></a>
        <a onclick='makeAjaxCall("/set?m=12")'><button>Fade</button></a>
        <a onclick='makeAjaxCall("/set?m=23")'><button>Strobe</button></a>
        <a onclick='makeAjaxCall("/set?m=25")'><button>Multi Strobe</button></a>
        <a onclick='makeAjaxCall("/set?m=6")'><button>Single Dynamic</button></a>
        <a onclick='makeAjaxCall("/set?m=7")'><button>Multi Dynamic</button></a>
        </div>
        <div class="category">
        <h1>Wipe</h1>
        <a onclick='makeAjaxCall("/set?m=3")'><button>Color Wipe</button></a>
        <a onclick='makeAjaxCall("/set?m=41")'><button>Comet</button></a>
        <a onclick='makeAjaxCall("/set?m=10")'><button>Scan</button></a>
        <a onclick='makeAjaxCall("/set?m=11")'><button>Dual Scan</button></a>
        <a onclick='makeAjaxCall("/set?m=13")'><button>Theater Chase</button></a>
        <a onclick='makeAjaxCall("/set?m=15")'><button>Running Lights</button></a>
        <a onclick='makeAjaxCall("/set?m=37")'><button>Running Color</button></a>
        <a onclick='makeAjaxCall("/set?m=38")'><button>Running Red Blue</button></a>
        <a onclick='makeAjaxCall("/set?m=40")'><button>Larson Scanner</button></a>
        <a onclick='makeAjaxCall("/set?m=48")'><button>Dual Color Wipe In to Out</button></a>
        <a onclick='makeAjaxCall("/set?m=49")'><button>Dual Color Wipe In to In</button></a>
        <a onclick='makeAjaxCall("/set?m=50")'><button>Dual Color Wipe Out to Out</button></a>
        <a onclick='makeAjaxCall("/set?m=51")'><button>Dual Color Wipe Out to In</button></a>
        </div>
        <div class="category">
        <h1>Rainbow</h1>
        <a onclick='makeAjaxCall("/set?m=8")'><button>Rainbow</button></a>
        <a onclick='makeAjaxCall("/set?m=9")'><button>Rainbow Cycle</button></a>
        <a onclick='makeAjaxCall("/set?m=14")'><button>Theater Chase Rainbow</button></a>
        <a onclick='makeAjaxCall("/set?m=24")'><button>Strobe Rainbow</button></a>
        <a onclick='makeAjaxCall("/set?m=30")'><button>Chase Rainbow</button></a>
        <a onclick='makeAjaxCall("/set?m=33")'><button>Chase Rainbow White</button></a>
        <a onclick='makeAjaxCall("/set?m=35")'><button>Chase Blackout Rainbow</button></a>
        <a onclick='makeAjaxCall("/set?m=26")'><button>Blink Rainbow</button></a>
        </div>
        <div class="category">
        <h1>Spark</h1>
        <a onclick='makeAjaxCall("/set?m=16")'><button>Twinkle</button></a>
        <a onclick='makeAjaxCall("/set?m=18")'><button>Twinkle Fade</button></a>
        <a onclick='makeAjaxCall("/set?m=20")'><button>Sparkle</button></a>
        <a onclick='makeAjaxCall("/set?m=21")'><button>Flash Sparkle</button></a>
        <a onclick='makeAjaxCall("/set?m=22")'><button>Hyper Sparkle</button></a>
        </div>
        <div class="category">
        <h1>Chase</h1>
        <a onclick='makeAjaxCall("/set?m=27")'><button>Chase White</button></a>
        <a onclick='makeAjaxCall("/set?m=28")'><button>Chase Color</button></a>
        <a onclick='makeAjaxCall("/set?m=31")'><button>Chase Flash</button></a>
        <a onclick='makeAjaxCall("/set?m=34")'><button>Chase Blackout</button></a>
        </div>
        <div class="category">
        <h1>Seasonal</h1>
        <a onclick='makeAjaxCall("/set?m=42")'><button>Fireworks</button></a>
        <a onclick='makeAjaxCall("/set?m=44")'><button>Merry Christmas</button></a>
        <a onclick='makeAjaxCall("/set?m=45")'><button>Fire Flicker</button></a>
        <a onclick='makeAjaxCall("/set?m=46")'><button>Fire Flicker (soft)</button></a>
        <a onclick='makeAjaxCall("/set?m=47")'><button>Fire Flicker (intense)</button></a>
        <a onclick='makeAjaxCall("/set?m=52")'><button>Circus Combustus</button></a>
        <a onclick='makeAjaxCall("/set?m=53")'><button>Halloween</button></a>
        </div>
        <div class="category">
        <h1>Random</h1>
        <a onclick='makeAjaxCall("/set?m=5")'><button>Random Color</button></a>
        <a onclick='makeAjaxCall("/set?m=4")'><button>Color Wipe Random</button></a>
        <a onclick='makeAjaxCall("/set?m=36")'><button>Color Sweep Random</button></a>
        <a onclick='makeAjaxCall("/set?m=17")'><button>Twinkle Random</button></a>
        <a onclick='makeAjaxCall("/set?m=29")'><button>Chase Random</button></a>
        <a onclick='makeAjaxCall("/set?m=32")'><button>Chase Flash Random</button></a>
        <a onclick='makeAjaxCall("/set?m=39")'><button>Running Random</button></a>
        <a onclick='makeAjaxCall("/set?m=43")'><button>Fireworks Random</button></a>
        <a onclick='makeAjaxCall("/set?m=19")'><button>Twinkle Fade Random</button></a>
        </div>
    </div>
    
    <div class="footnote">
        Simple LED Strip Controller Version 2<br>
        <a href="https://github.com/eighthree/Simple-LED-Strip-Controller" target="_new">View Project on Github</a><br>
    </div>
    </div>
    <script src="script.min.js"></script>
    <script src="jscolor.min.js"></script>
    
</body>
</html>
)"
