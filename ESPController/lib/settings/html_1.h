const char FILE_INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta name = "viewport" content = "width=device-width,initial-scale=1.0">
<title>DIY BMS CONTROLLER v4</title>
<script src="jquery.js" type="text/javascript"></script>

<!--jquery-3.3.1.min.js-->
<script src="jquery.js" integrity="sha384-tsQFqpEReu7ZLhBV2VZlAu7zcOV+rXbYlF2cqB8txI/8aZajjp4Bqd+V6D5IgvKT" crossorigin="anonymous"></script>

<style>
*{box-sizing:border-box}
body{margin:0;font-family:Arial,Helvetica,sans-serif}
.header{overflow:hidden;background-color:#f1f1f1;padding:20px 10px}
.header a{float:left;color:#000;text-align:center;padding:12px;text-decoration:none;font-size:18px;line-height:25px;border-radius:4px}
.header a.logo{font-size:25px;font-weight:700}
.header a:hover{background-color:#ddd;color:#000}
.header a.active{background-color:#1e90ff;color:#fff}
.header-right{float:right}
.left{float:left;}
.bar {height:100%;margin-left:0.5%;margin-right:0.5%;float:left;}
.bar .g {background-color:#3d9970;display:inline-block;}
.bar .t {text-align:center;display:inline-block;font-size:small;}
.graph {border: solid 2px gray; padding:0.1em;height:350px;background: repeating-linear-gradient(to top,#ddd,#ddd 1px,#fff 1px,#fff 5%);}
.bypassovertemp { background-image: linear-gradient(#AA0000, #3d9970) !important;}
.bypass { background-image: linear-gradient(#000050, #3d9970) !important;}
#info {border: solid 2px gray; padding:0.1em;}
.eighty { width:80%;}
.twenty { width:20%;}

@media screen and (max-width:500px){
  .header a,.header-right{float:none}
  .header a{display:block;text-align:left}
}

@media screen and (max-width:640px){
  .left{float:none;}
  .graph{height:300px; width:100%;}
}
</style>
</head>
<body>
<div class="header">
  <a href="#default" class="logo">DIY BMS Controller v4</a>
  <div class="header-right">
    <a class="active" href="#home">Home</a>
    <a href="#contact">Settings</a>
    <a href="#about">About</a>
  </div>
</div>
<div class="graph left eighty"></div>
<div id="info" class="left twenty">
<p>Vivamus eleifend, risus at ultrices ultricies, dolor risus luctus sem, ac convallis nunc diam a urna. Ut in iaculis lectus. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nunc id vestibulum odio, sed consectetur nibh. Sed neque massa, blandit a tortor et, volutpat rutrum lacus. Phasellus id orci eros. Nam consectetur ultricies mollis. Vivamus rutrum cursus ex, sed pharetra erat tincidunt vel.</p>
</div>
<script type="text/javascript">

function queryBMS() {
  $.getJSON( "monitor.json", function( data ) {
    //Allow multiple graphs for each bank
    var graph=$(".graph").first();

    if (data.cells.length!=$(graph).children().length) {
      //Need to redraw all the cells
      $(graph).empty();
      $.each(data.cells, function( index, value ) {
        $(graph).append("<div class='bar'><span class='t'></span><span class='g'></span></div>");
      });
    }

    //Cells now match number of items in graph
    var bars=$(graph).children();

    var barWidth=(100-bars.length)/bars.length;

    $.each(data.cells, function( index, value ) {
      $(bars[index]).css("width",barWidth+"%");

      //5000mV is max graph scale range
      var h=value.voltage/5000*100;
      var h1=100.0-h;
      $(bars[index]).find(".t").css("width", "100%").css("height", h1+"%");

      $(bars[index]).find(".t").html(
        (parseFloat(value.voltage)/1000.0).toFixed(3)+"V<br/>"
        +value.int
        +"&deg;C<br/>"
        +value.ext
        +"&deg;C"
      );

      $(bars[index]).find(".g").css("width", "100%").css("height", h+"%");

      if (value.bypass) {
        $(bars[index]).find(".g").addClass("bypass");

        if (value.bypassovertemp) {
          $(bars[index]).find(".g").addClass("bypassovertemp");
        }
      } else {
        $(bars[index]).find(".g").removeClass("bypass").removeClass("bypassovertemp");
      }
    });

  });

  setTimeout(queryBMS,5000);
}

$(function() {
  setTimeout(queryBMS,1000);
});
</script>
</body>
</html>
)=====";
