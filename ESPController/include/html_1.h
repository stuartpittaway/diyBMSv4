const char FILE_INDEX_HTML[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta name = "viewport" content = "width=device-width,initial-scale=1.0">
<title>DIY BMS CONTROLLER v4</title>
<!-- jquery-3.3.1.min.js -->
<script src="jquery.js" integrity="sha384-tsQFqpEReu7ZLhBV2VZlAu7zcOV+rXbYlF2cqB8txI/8aZajjp4Bqd+V6D5IgvKT" crossorigin="anonymous"></script>
<script src="echarts.simple.min.js" integrity="" crossorigin="anonymous"></script>
<style>
*{box-sizing:border-box}
body{margin:0;font-family:Arial,Helvetica,sans-serif;}
.header{overflow:hidden;background-color:#d4f0fb;padding:20px 10px}
.header a{float:left;color:#000;text-align:center;padding:12px;text-decoration:none;font-size:18px;line-height:25px;border-radius:4px}
.header a.logo{font-size:25px;font-weight:700}
.header a:hover{background-color:#ddd;color:#000}
.header a.active{background-color:#1e90ff;color:#fff}
.header-right{float:right}
.logo { float:left; background-image: url("logo.gif");  width:191px;  height:48px;}
.left{float:left;}
#info {border: solid 2px gray; padding:0.1em;}
.eighty { width:80%;}
.twenty { width:20%;}
#refreshbar { width:100%; padding:0; margin:0; height:4px; background-color:#d3f9fa;}
.ct-series-a .ct-bar {
  stroke: purple;
  stroke-width: 30px;
}

@media screen and (max-width:500px){
  .header a,.header-right{float:none}
  .header a{display:block;text-align:left}
}

@media screen and (max-width:640px){
  .left{float:none;}
}
</style>
</head>
<body>
<div class="header">
  <div class="logo">&nbsp;</div>
  <div class="header-right">
    <a class="active" href="#home">Home</a>
    <a href="#contact">Settings</a>
    <a href="#about">About</a>
  </div>
</div>
<div id='refreshbar'></div>
<div class="left eighty" style="height:768px;">
<div id="graph1" style="width:100%; height:100%;"></div>
</div>
<div id="info" class="left twenty">
<p>Vivamus eleifend, risus at ultrices ultricies, dolor risus luctus sem, ac convallis nunc diam a urna. Ut in iaculis lectus. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nunc id vestibulum odio, sed consectetur nibh. Sed neque massa, blandit a tortor et, volutpat rutrum lacus. Phasellus id orci eros. Nam consectetur ultricies mollis. Vivamus rutrum cursus ex, sed pharetra erat tincidunt vel.</p>
<p id="count"></p>
</div>
<script type="text/javascript">
var g1=null;
var g2=null;
var g3=null;

function queryBMS() {
  $.getJSON( "monitor.json", function( jsondata ) {


    var labels=[];
    var voltages=[];
    var tempint=[];
    var tempext=[];
    $.each(jsondata.cells, function( index, value ) {
      voltages.push((parseFloat(value.voltage)/1000.0));
      labels.push(index);

      tempint.push(value.int);
      tempext.push(value.ext);
    });



    if (g1==null) {

      // based on prepared DOM, initialize echarts instance
      g1 = echarts.init(document.getElementById('graph1'));

      // specify chart configuration item and data
      var option = {
        tooltip: { trigger: 'axis', axisPointer: { type: 'cross', crossStyle: { color: '#999' } } },
          legend: { data:['Voltage'] },
          xAxis: [{gridIndex: 0,type:'category' },{  gridIndex: 1,type:'category' }],
          yAxis: [
            {gridIndex: 0,name:'Volts',type:'value',min:0,max:5.0,interval:0.25,position:'left', axisLabel: { formatter: '{value}V' }}
            ,{gridIndex: 1,name:'Temperature',type:'value',interval:10,position:'right'
            ,axisLabel:{ formatter: '{value} °C' }
            ,axisLine:{show:false, lineStyle:{type:'dotted'} } } ]
          ,series: [{ name: 'Voltage', type: 'bar', data: [] }
            ,{xAxisIndex:1, yAxisIndex:1, name:'BypassTemperature',type:'bar', data: [] }
            ,{xAxisIndex:1, yAxisIndex:1, name:'CellTemperature',type:'bar',data: [] }
          ],
          grid: [{bottom: '30%'},{top: '70%'}],
      };

      // use configuration item and data specified to show chart
      g1.setOption(option);

    } else {
      g1.setOption({
          xAxis: { data: labels },
          series: [{ name: 'Voltage', data: voltages }
          ,{ name: 'BypassTemperature', data: tempint }
          ,{ name: 'CellTemperature', data: tempext }]
      });
    }

/*
    var graph1width=$('#graph1').width();

    if (g2==null) {
      var options2 = {
        chartPadding: {    top: 5,    right: 10,    bottom: 5,    left: 5  },
      };
    g2=new Chartist.Bar('#graph2', data2, options2);
  } else {
    g2.update(data2);
  }


      if (g3==null) {
        var options3 = {
          chartPadding: {    top: 5,    right: 10,    bottom: 5,    left: 5  },
        };
        g3=new Chartist.Bar('#graph3', data3, options3);
      } else {
        g3.update(data3);
      }
*/

  });
}

//var refreshcounter=1;

function countdown() {
  //refreshcounter--;
  //if (refreshcounter==0) {
    queryBMS();
    //Refresh every 5 seconds
    //refreshcounter=5;
  //}

  //$("#count").html(refreshcounter);

  //setTimeout(countdown,1000);

  $("#refreshbar").width('100%').animate({ width: '-=100%' },{ duration:5000, complete: countdown, queue:false});
}

$(function() {
  countdown();
  //setTimeout(countdown,1000);

  //$("#refreshbar").animate({ width: '-=100%', done: countdown }, 5000);

});
</script>
</body>
</html>
)=====";
